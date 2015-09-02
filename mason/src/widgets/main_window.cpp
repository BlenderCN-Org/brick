#include <iostream>

#include <assert.h>
#include <string.h>

#include <gtk/gtk.h>
#include <gtk/gtkgl.h>

#include <GL/gl.h>

#include "../app.h"
#include "main_window.h"

#include "math/math-utility.h"

#define VIEWPORT_BOUNDS 25.0f

#define ZOOM_SPEED          0.75f
#define ZOOM_MULTIPLIER     0.5f
#define SCROLL_SPEED        0.8f
#define SCROLL_MULTIPLIER   4.0f

#define UI_ASSERT(A)    assert((A)); if(!(A)) { return false; }

using namespace std;

MainWindow::MainWindow() : Widget()
{
    clear_color[0] = 0.8f;
    clear_color[1] = 0.9f;
    clear_color[2] = 1.0f;
    clear_color[3] = 1.0f;

    main_toolbar = new MainToolbar;
    prop_panel = new PropertyPanel;
}

MainWindow::MainWindow(App *a) : Widget(a)
{
    clear_color[0] = 0.8f;
    clear_color[1] = 0.9f;
    clear_color[2] = 1.0f;
    clear_color[3] = 1.0f;

    main_toolbar = new MainToolbar(a);
    prop_panel = new PropertyPanel(a);
}

MainWindow::~MainWindow()
{
    delete main_toolbar;
    delete prop_panel;
}

static gboolean render(gpointer user_data)
{
	GtkWidget *da = GTK_WIDGET(user_data);

	gdk_window_invalidate_rect(da->window, &da->allocation, FALSE);
	gdk_window_process_updates(da->window, FALSE);

	return TRUE;
}

//TODO: wrap this into the MainWindow class... it's weird that it's
//referencing a bunch of MainWindow members but not part of the class
//yet getting passed an app.... yeah.... wtf.
static void gl_draw_grid(App *app, float x0, float x1, float y0, float y1, float win_width, float win_height)
{
    float grid_size = app->get_grid_size();
    float border_count = 5.0f;

    MainWindow *win = app->get_main_window();
    assert(win);

    x0 = (float)((int)(x0 / grid_size)) * grid_size - (border_count * grid_size);
    x1 = (float)((int)(x1 / grid_size)) * grid_size + (border_count * grid_size);

    y0 = (float)((int)(y0 / grid_size)) * grid_size - (border_count * grid_size);
    y1 = (float)((int)(y1 / grid_size)) * grid_size + (border_count * grid_size);

    int x_count = (int)((x1 - x0) / grid_size);
    int y_count = (int)((y1 - y0) / grid_size);

    float density = (x_count) / (win_width);

    float clear_col[4];
    win->get_clear_color(clear_col);

    //fade lines out when they get too dense
    float max_density = 0.15f;
    float min_density = 0.05f;

    float lerp_val = Math::clamp(1.0f - (density - min_density) / (max_density - min_density), 0.0f, 1.0f);
    float line_color[3];
    line_color[0] = Math::lerp(clear_col[0], 0.8f, lerp_val);
    line_color[1] = Math::lerp(clear_col[1], 0.8f, lerp_val);
    line_color[2] = Math::lerp(clear_col[2], 1.0f, lerp_val);


    //TODO: as we zoom in, thicken or darken the main lines and render a new set @ 0.1, 0.2... etc.
    float line_width = 1.0f;//Math::lerp(1.0f, 2.0f, lerp_val);

    if(density > max_density)
    {
        return;
    }

    //ok, draw!
    glLineWidth(line_width);
    glColor3f(line_color[0], line_color[1], line_color[2]);
    glBegin(GL_LINES);

    int i;
    for(i = 0; i < x_count; i++)
    {
        float line_x = x0 + (float)i * grid_size;
        glVertex3f(line_x, y0, 0.0f);
        glVertex3f(line_x, y1, 0.0f);
    }

    for(i = 0; i < y_count; i++)
    {
        float line_y = y0 + (float)i * grid_size;
        glVertex3f(x0, line_y, 0.0f);
        glVertex3f(x1, line_y, 0.0f);
    }

    glEnd();
}

static gboolean expose(GtkWidget *da, GdkEventExpose *event, gpointer user_data)
{
    App *app = (App *)user_data;
    assert(app);

    MainWindow *win = app->get_main_window();
    assert(win);

    float clear_col[4];
    win->get_clear_color(clear_col);

    float aspect_ratio = (float)da->allocation.width / (float)da->allocation.height;

	glLoadIdentity();
	glViewport(0, 0, da->allocation.width, da->allocation.height);

    float scroll_x, scroll_y, zoom;
    app->get_scroll_state(scroll_x, scroll_y);
    app->get_zoom_state(zoom);

    float x0 = zoom * -VIEWPORT_BOUNDS * aspect_ratio + scroll_x;
    float x1 = zoom * VIEWPORT_BOUNDS * aspect_ratio + scroll_x;
    float y0 = zoom * -VIEWPORT_BOUNDS + scroll_y;
    float y1 = zoom * VIEWPORT_BOUNDS + scroll_y;

    glOrtho(x0, x1, y0, y1, -200, 1000);

	GdkGLContext *glcontext = gtk_widget_get_gl_context(da);
	GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable(da);

    gboolean res = gdk_gl_drawable_gl_begin(gldrawable, glcontext);
    assert(res);

	/* draw in here */
	glClearColor(clear_col[0], clear_col[1], clear_col[2], clear_col[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();

    gl_draw_grid(app, x0, x1, y0, y1, da->allocation.width, da->allocation.height);
    app->get_current_level()->render_gl();

    //anything that needs to render on top of the level...
    //selection rectangles, etc...
    app->render_gl();

	glPopMatrix();

	if(gdk_gl_drawable_is_double_buffered(gldrawable)) {
		gdk_gl_drawable_swap_buffers(gldrawable);
	} else {
		glFlush();
	}

	gdk_gl_drawable_gl_end(gldrawable);

	return TRUE;
}

static gboolean configure (GtkWidget *da, GdkEventConfigure *event, gpointer user_data) {
	GdkGLContext *gl_context = gtk_widget_get_gl_context(da);
	assert(gl_context);

	GdkGLDrawable *gl_drawable = gtk_widget_get_gl_drawable(da);
    assert(gl_drawable);

	gboolean res = gdk_gl_drawable_gl_begin(gl_drawable, gl_context);
	assert(res);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	gdk_gl_drawable_gl_end(gl_drawable);

	return TRUE;
}

//
//key handling
gint key_press_cb(GtkWidget *widget, GdkEventKey *kevent, gpointer data)  {
    App *app = (App *)data;

    //use this to figure out what key you're pressing
    /*if(kevent->type == GDK_KEY_PRESS)  {
        g_message("%d, %c;", kevent->keyval, kevent->keyval);
    }*/

    float z;
    app->get_zoom_state(z);

    switch(kevent->keyval) {
        case 'a':
            app->scroll(-SCROLL_SPEED * z, 0.0f);
            break;
        case 'A':
            app->scroll(-SCROLL_SPEED * SCROLL_MULTIPLIER * z, 0.0f);
            break;
        case 's':
            app->scroll(0.0f, -SCROLL_SPEED * z);
            break;
        case 'S':
            app->scroll(0.0f, -SCROLL_SPEED * SCROLL_MULTIPLIER * z);
            break;
        case 'd':
            app->scroll(SCROLL_SPEED * z, 0.0f);
            break;
        case 'D':
            app->scroll(SCROLL_SPEED * SCROLL_MULTIPLIER * z, 0.0f);
            break;
        case 'w':
            app->scroll(0.0f, SCROLL_SPEED * z);
            break;
        case 'W':
            app->scroll(0.0f, SCROLL_SPEED * SCROLL_MULTIPLIER * z);
            break;
        case 'q':
            app->zoom(ZOOM_SPEED);
            break;
        case 'e':
            app->zoom(1.0f / ZOOM_SPEED);
            break;

        case 65360: //HOME
            app->scroll_reset();
            app->zoom_reset();
            break;

        case 'P':
            app->write_to_file(TEST_LEVEL_FILE_WRITE);
            break;
    }

    // void g_signal_emit_by_name(GObject *object, const gchar *name, ... );
    // g_signal_emit_by_name(G_OBJECT(btn), "clicked", NULL);
    //g_signal_emit_by_name(G_OBJECT(widget), "clicked", NULL);

    return TRUE;
}

static void screen_to_world(float (&pos)[2], GtkWidget *da, App *app)
{
    float aspect_ratio = (float)da->allocation.width / (float)da->allocation.height;

    //convert screen coordinates to world coordinates
    pos[0] = (pos[0] / da->allocation.width) * VIEWPORT_BOUNDS * 2.0f - VIEWPORT_BOUNDS;
    pos[1] = ((da->allocation.height - pos[1]) / da->allocation.height) * VIEWPORT_BOUNDS * 2.0f - VIEWPORT_BOUNDS;

    float z;
    app->get_zoom_state(z);

    float scroll[2];
    app->get_scroll_state(scroll[0], scroll[1]);

    pos[0] = z * pos[0] * aspect_ratio + scroll[0];
    pos[1] = z * pos[1] + scroll[1];
}

static gboolean button_press_cb(GtkWidget *da, GdkEventButton *event, gpointer data)
{
    App *app = (App *)data;
    assert(app);

    //reclaim focus
    MainWindow *main_window = app->get_main_window();
    assert(main_window);
    gtk_window_set_focus(GTK_WINDOW(main_window->get_widget()), da);

    float click_pos[2] = { event->x, event->y };
    screen_to_world(click_pos, da, app);

    if(event->button == 1) {
        cout<<"left ";
    } else if(event->button == 2) {
        cout<<"middle ";
    } else if(event->button == 3) {
        cout<<"right ";
    }
    //cout<<"click! ("<<event->x<<", "<<event->y<<")"<<endl;
    //cout<<"world: ("<<click_pos[0]<<", "<<click_pos[1]<<")"<<endl;

    app->select(click_pos, false);

    return TRUE;
}

static gboolean mouse_move_cb(GtkWidget *da, GdkEventMotion *event, gpointer data)
{
    float pos[2];
    GdkModifierType state;

    App *app = (App *)data;
    assert(app);

    //reclaim focus
    MainWindow *main_window = app->get_main_window();
    assert(main_window);
    gtk_window_set_focus(GTK_WINDOW(main_window->get_widget()), da);


    if(event->is_hint)
    {
        int x, y;
        gdk_window_get_pointer(event->window, &x, &y, &state);
        pos[0] = (float)x;
        pos[1] = (float)y;
    }
    else
    {
        pos[0] = (float)event->x;
        pos[1] = (float)event->y;
        state = (GdkModifierType)event->state;
    }

    if(state & GDK_BUTTON1_MASK)
    {
        //cout<<"left mouse drag!"<<endl;
        screen_to_world(pos, da, app);
        app->move_selection(pos);
    }

    if(state & GDK_BUTTON2_MASK)
    {
        screen_to_world(pos, da, app);
        cout<<"button2: "<<pos[0]<<" "<<pos[1]<<endl;
    }

    return TRUE;
}

static gboolean scroll_cb(GtkWidget *w, GdkEventScroll *event, gpointer data)
{
    //cout<<"scrollin"<<endl;
    App *app = (App *)data;
    assert(app);

    float scroll_speed = SCROLL_SPEED;
    if(event->state & GDK_SHIFT_MASK)
    {
        scroll_speed *= SCROLL_MULTIPLIER;
    }
    else if(event->state & GDK_CONTROL_MASK)
    {
        scroll_speed /= SCROLL_MULTIPLIER;
    }

    switch(event->direction)
    {
        case GDK_SCROLL_RIGHT:
            app->scroll(scroll_speed, 0.0f);
            break;
        case GDK_SCROLL_LEFT:
            app->scroll(-scroll_speed, 0.0f);
            //cout<<scroll_speed<<endl;
            break;
        case GDK_SCROLL_UP:
            app->scroll(0.0f, scroll_speed);
            break;
        case GDK_SCROLL_DOWN:
            app->scroll(0.0f, -scroll_speed);
            break;
    }
    return TRUE;
}

//
// programing is like rock climbing. Maybe more like trad
// climbing than sport climbing... you build yourself all
// these systems of redundant protection in case you fall...
// in case the program crashes and comes tumbling down.
// You want to be able to catch that fall and analyze it
//
// CF - 6.26.13
//
bool MainWindow::init()
{
	GtkWidget *da = NULL;
	GdkGLConfig *gl_config = NULL;

    GtkWidget *vbox = NULL;
    GtkWidget *hbox = NULL;

    //menu bar
    GtkWidget *menu_bar = NULL;
    GtkWidget   *file_menu = NULL;
    GtkWidget       *file = NULL;
    GtkWidget       *new_level = NULL;
    GtkWidget       *open_level = NULL;
    GtkWidget       *quit = NULL;

	char buffer[256];

	w = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size(GTK_WINDOW(w), 800, 600);

	sprintf(buffer, "Mason %i.%i", MASON_VERSION_MAJOR, MASON_VERSION_MINOR);
	gtk_window_set_title(GTK_WINDOW(w), buffer);

	vbox = gtk_vbox_new(FALSE, 0);
	UI_ASSERT(vbox);
	gtk_container_add(GTK_CONTAINER(w), vbox);

	hbox = gtk_hbox_new(FALSE, 0);
	UI_ASSERT(hbox);

	menu_bar = gtk_menu_bar_new();
	UI_ASSERT(menu_bar);

	file_menu = gtk_menu_new();
	UI_ASSERT(file_menu);

	file = gtk_menu_item_new_with_label("File");
	UI_ASSERT(file);

	new_level = gtk_menu_item_new_with_label("New");
	UI_ASSERT(new_level);

	open_level = gtk_menu_item_new_with_label("Open...");
	UI_ASSERT(open_level);

    quit = gtk_menu_item_new_with_label("Quit");
    UI_ASSERT(quit);

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file), file_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), new_level);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), open_level);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), quit);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), file);
    gtk_box_pack_start(GTK_BOX(vbox), menu_bar, FALSE, FALSE, 0);

    //add the toolbar
    assert(main_toolbar);
    main_toolbar->init();
    gtk_box_pack_start(GTK_BOX(vbox), main_toolbar->w, FALSE, TRUE, 0);

    //set up the hbox for the drawing area and property panel
    gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 0);

    //add the drawing area to the hbox
	da = gtk_drawing_area_new();
	UI_ASSERT(da);
	gtk_box_pack_start(GTK_BOX(hbox), da, TRUE, TRUE, 0);

    //add the property panel
    assert(prop_panel);
    prop_panel->init();
    gtk_box_pack_start(GTK_BOX(hbox), prop_panel->w, FALSE, FALSE, 0);

	g_signal_connect_swapped(w,
                             "destroy",
                             G_CALLBACK(gtk_main_quit),
                             NULL);

    gtk_widget_set_can_focus(da, true);
	gtk_widget_set_events(da, GDK_EXPOSURE_MASK |
                              GDK_BUTTON_PRESS_MASK |
                              GDK_POINTER_MOTION_MASK |
                              GDK_POINTER_MOTION_HINT_MASK);

	int cm = GDK_GL_MODE_RGB | GDK_GL_MODE_DEPTH | GDK_GL_MODE_DOUBLE;
	gl_config = gdk_gl_config_new_by_mode((GdkGLConfigMode)cm);
    UI_ASSERT(gl_config);

	gboolean res = gtk_widget_set_gl_capability(da, gl_config, NULL, TRUE, GDK_GL_RGBA_TYPE);
	UI_ASSERT(res);

	//gtk_widget_create_gl_context(da, NULL, TRUE, GDK_GL_RGBA_TYPE);

	g_signal_connect(da, "configure-event", G_CALLBACK(configure), (gpointer)app);
	g_signal_connect(da, "expose-event", G_CALLBACK(expose), (gpointer)app);

	g_signal_connect(G_OBJECT(da), "key_press_event", G_CALLBACK(key_press_cb), (gpointer)app);
	g_signal_connect(G_OBJECT(da), "button_press_event", G_CALLBACK(button_press_cb), (gpointer)app);
	g_signal_connect(G_OBJECT(da), "motion_notify_event", G_CALLBACK(mouse_move_cb), (gpointer)app);
    g_signal_connect(G_OBJECT(da), "scroll_event", G_CALLBACK(scroll_cb), (gpointer)app);

    g_timeout_add(1000 / 60, render, da);

    //this line is required for glewInit() to succeed
    //because the gl context doesn't exist until the widget
    //is realized. I *just* realized this.... ;)
    gtk_widget_realize(da);

    return true;
}

void MainWindow::show()
{
    main_toolbar->show();
    prop_panel->show();
    gtk_widget_show_all(w);
}

void MainWindow::get_clear_color(float (&c)[4]) const
{
    c[0] = clear_color[0];
    c[1] = clear_color[1];
    c[2] = clear_color[2];
    c[3] = clear_color[3];
}

void MainWindow::set_titlebar_fname(const char *fname)
{
    char buffer[1024];

    sprintf(buffer, "Mason %i.%i - ", MASON_VERSION_MAJOR, MASON_VERSION_MINOR);
    strcat(buffer, fname);
	gtk_window_set_title(GTK_WINDOW(w), buffer);
}
