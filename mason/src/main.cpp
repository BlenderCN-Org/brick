#include <iostream>
#include <assert.h>
#include <math.h>

#include <gtk/gtk.h>
#include <gtk/gtkgl.h>

#include <GL/glew.h>
#include <GL/gl.h>


//ui includes
#include "app.h"

//game code includes
#include "level.h"
#include "graphics/glutility.h"

using namespace std;

App *app = NULL;

void init()
{
    bool res = false;

    app = new App;
    res = app->init();
    assert(res);
}

void show_all_ui()
{
    app->show_all_ui();
}

void cleanup()
{
    delete app;
}

int main(int argc, char **argv)
{
    gtk_init(&argc, &argv);
	gtk_gl_init(&argc, &argv);

    /*GLenum res = glewInit();
    if(res != GLEW_OK)
    {
        cout<<glewGetErrorString(res);
        return 0;
    }*/

    //everything should be initialized by this point
    init();
    show_all_ui();

	//main loop
	gtk_main();

    cleanup();

	return 0;
}
