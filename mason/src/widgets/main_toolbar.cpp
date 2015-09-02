#include <assert.h>
#include <string.h>

#include "../app.h"
#include "object/level-platform.h"

#include "main_toolbar.h"

MainToolbar::MainToolbar() : Widget() {}
MainToolbar::MainToolbar(App *a) : Widget(a) {}
MainToolbar::~MainToolbar() {}

static void item_open_clicked_cb(GtkWidget *widget, gpointer data)
{
    g_print("clicked\n");

    App *app = (App *)data;
    assert(app);

    MainWindow *main_win = app->get_main_window();
    assert(main_win);

    GtkWidget *dialog;

    dialog = gtk_file_chooser_dialog_new("Open File",
                                         GTK_WINDOW(main_win->get_widget()),
                                         GTK_FILE_CHOOSER_ACTION_OPEN,
                                         GTK_STOCK_CANCEL,
                                         GTK_RESPONSE_CANCEL,
                                         GTK_STOCK_OPEN,
                                         GTK_RESPONSE_ACCEPT,
                                         NULL);

    if(gtk_dialog_run(GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
    {
        char filename[512];

        strcpy(filename, gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog)));
        g_print(filename);
        //open_file(filename);
        //g_free (filename);
        app->read_from_file(filename);
    }

    gtk_widget_destroy(dialog);
}

static void item_create_clicked_cb(GtkWidget *widget, gpointer data)
{
    App *app = (App *)data;
    assert(app);

    World *w = app->get_world();

    Level *lp = app->get_current_level();
    assert(lp);

    LevelPlatform2D *pp = new LevelPlatform2D;
    pp->set_pos(0.0f, 0.0f);
    pp->set_scale(1.0f, 1.0f);
    pp->set_mesh(w->get_mesh(0));
    pp->init();
    lp->add_platform(pp);

    //lp->add_
    //widget->grab_focus();
    //gtk_widget_grab_focus(widget);
}

bool MainToolbar::init()
{
    GtkToolItem *item_new = NULL;
    GtkToolItem *item_open = NULL;
    GtkToolItem *item_save = NULL;

    GtkToolItem *item_sep = NULL;

    GtkToolItem *item_undo = NULL;
    GtkToolItem *item_redo = NULL;
    GtkToolItem *item_create = NULL;

    w = gtk_toolbar_new();
    assert(w);

    gtk_toolbar_set_style(GTK_TOOLBAR(w), GTK_TOOLBAR_ICONS);
    gtk_container_set_border_width(GTK_CONTAINER(w), 2);
    gtk_toolbar_set_orientation(GTK_TOOLBAR(w), GTK_ORIENTATION_HORIZONTAL);

    item_new = gtk_tool_button_new_from_stock(GTK_STOCK_NEW);
    gtk_toolbar_insert(GTK_TOOLBAR(w), item_new, -1);

    item_open = gtk_tool_button_new_from_stock(GTK_STOCK_OPEN);
    gtk_toolbar_insert(GTK_TOOLBAR(w), item_open, -1);

    item_save = gtk_tool_button_new_from_stock(GTK_STOCK_SAVE);
    gtk_toolbar_insert(GTK_TOOLBAR(w), item_save, -1);

    item_sep = gtk_separator_tool_item_new();
    gtk_toolbar_insert(GTK_TOOLBAR(w), item_sep, -1);

    item_undo = gtk_tool_button_new_from_stock(GTK_STOCK_UNDO);
    gtk_toolbar_insert(GTK_TOOLBAR(w), item_undo, -1);

    item_redo = gtk_tool_button_new_from_stock(GTK_STOCK_REDO);
    gtk_toolbar_insert(GTK_TOOLBAR(w), item_redo, -1);

    item_create = gtk_tool_button_new_from_stock(GTK_STOCK_ADD);
    gtk_toolbar_insert(GTK_TOOLBAR(w), item_create, -1);

    //gtk_toolbar_append_item(w, "foo", "Foo tooltip", "Foo Private Text", )

    g_signal_connect(G_OBJECT(item_open), "clicked", G_CALLBACK(item_open_clicked_cb), (gpointer)app);
    g_signal_connect(G_OBJECT(item_create), "clicked", G_CALLBACK(item_create_clicked_cb), (gpointer)app);

    return true;
}

void MainToolbar::show()
{

}
