#include <iostream>
#include <iomanip>

#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include <GL/glew.h>
#include <GL/gl.h>

#include "graphics/glutility.h"

#include "app.h"
#include "object/level-platform.h"

using namespace std;

App::App()
{
    main_window = NULL;

    grid_size = 1.0f;
    scroll_state[0] = scroll_state[1] = 0.0f;
    zoom_state = 1.0f;
    current_level = NULL;

    world = new World;

    current_level_fname[0] = '\0';
}

App::~App()
{
    if(main_window)
    {
        delete main_window;
    }

    if(current_level)
    {
        delete current_level;
    }

    if(world)
    {
        delete world;
    }
}

bool App::init()
{
    bool res = false;

    //current_level = new Level;
    //res = current_level->read_from_file(TEST_LEVEL_FILE);
    //assert(res);
    //strcpy(current_level_fname, TEST_LEVEL_FILE);


    main_window = new MainWindow(this);
    res = main_window->init();
    assert(res);

    GLenum result = glewInit();
    if(result != GLEW_OK)
    {
        cout<<glewGetErrorString(result)<<endl;
        return false;
    }

    world->init();

    //has to happen *after* we init the main window
    read_from_file(TEST_LEVEL_FILE);

    return res;
}

void App::show_all_ui()
{
    main_window->show();
}

bool App::read_from_file(const char *fname)
{
    if(current_level) {
        delete current_level;
    }
    current_level = new Level;
    assert(current_level);

    bool res = current_level->read_from_file(fname);
    assert(res);

    assert(world);
    assert(current_level);
    current_level->init(world);

    strcpy(current_level_fname, fname);
    main_window->set_titlebar_fname(current_level_fname);

    return res;
}

bool App::write_to_file(const char *fname)
{
    ofstream f;
    f<<fixed<<setw(11)<<setprecision(6);
    f.open(fname, ios::out);
    if(!f.is_open())
    {
        return false;
    }

    f<<"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>"<<endl;

    assert(current_level);
    bool res = current_level->write_to_file(f);
    if(!res)
    {
        f.close();
        return false;
    }

    f.close();
    return true;
}

float App::get_grid_size() const
{
    return grid_size;
}

void App::scroll(const float x, const float y)
{
    scroll_state[0] += x;
    scroll_state[1] += y;
}

void App::scroll_reset()
{
    scroll_state[0] = scroll_state[1] = 0.0f;
}

void App::get_scroll_state(float &x, float &y)
{
    x = scroll_state[0];
    y = scroll_state[1];
}

void App::zoom(const float z)
{
    zoom_state *= z;
}

void App::zoom_reset()
{
    zoom_state = 1.0f;
}

void App::get_zoom_state(float &z)
{
    z = zoom_state;
}

void App::select(float p[2], bool append)
{
    if(!append)
    {
        selection.clear();
    }
    //p should be in world space already
    assert(current_level);
    //Object2D *current_level->select_hit_test(p2);
    std::vector<LevelPlatform2D *>::iterator lpit;
    for(lpit = current_level->platforms.begin(); lpit != current_level->platforms.end(); lpit++)
    {
        LevelPlatform2D *lp = *(lpit);
        assert(lp);

        if(lp->hit_test_2d(p))
        {
            cout<<p[0]<<", "<<p[1]<<endl;
            float foo_pos[2];
            lp->get_pos(foo_pos[0], foo_pos[1]);
            cout<<foo_pos[0]<<", "<<foo_pos[1]<<endl;
            selection.push_back(lp);
        }
    }
}

void App::move_selection(float pos[2])
{
    std::vector<Object2D *>::iterator it;
    for(it = selection.begin(); it != selection.end(); it++)
    {
        Object2D *op = *(it);
        assert(op);

        //pos[0] -= grid_size / 2.0f;
        //pos[1] -= grid_size / 2.0f;

        float remainder_x = (pos[0] / grid_size) - (float)((int)(pos[0] / grid_size));
        float remainder_y = (pos[1] / grid_size) - (float)((int)(pos[1] / grid_size));

        op->set_pos(pos[0] - remainder_x, pos[1] - remainder_y);
    }
}

void App::render_gl()
{
    //render selection rectangles
    std::vector<Object2D *>::iterator it;
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    for(it = selection.begin(); it != selection.end(); it++)
    {
        Object2D *op = *(it);
        assert(op);
        //cout<<"selected!"<<endl;
        float pos[2], dim[2];
        op->get_pos(pos[0], pos[1]);
        op->get_scale(dim[0], dim[1]);

        ObjectType ot = op->get_object_type();
        switch(ot)
        {
            case OBJECT_LEVEL_PLATFORM:
            {
                LevelPlatform2D *lp = (LevelPlatform2D *)op;
                assert(lp);
                glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
                lp->render_gl();
                break;
            }
        }

        /*
        glColor3f(1.0f, 1.0f, 1.0f);
        glPushMatrix();
        gl_cube(pos, dim);
        glPopMatrix();
        */
    }
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

Level *App::get_current_level()
{
    return current_level;
}

World *App::get_world()
{
    return world;
}

MainWindow *App::get_main_window()
{
    return main_window;
}
