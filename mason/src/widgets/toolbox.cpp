#include <assert.h>

#include "toolbox.h"

Toolbox::Toolbox() : Widget() {}
Toolbox::Toolbox(App *a) : Widget(a) {}
Toolbox::~Toolbox() {}

bool Toolbox::init()
{
    w = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    assert(w);

	gtk_window_set_default_size(GTK_WINDOW(w), 100, 300);

    return true;
}

void Toolbox::show()
{
    gtk_widget_show(w);
}
