#ifndef __WIDGET_H__
#define __WIDGET_H__

#include <gtk/gtk.h>

class App;

//abstract class all widgets inherit from
class Widget {
    public:
        Widget();
        Widget(App *a);
        ~Widget();

        virtual bool init() = 0;
        virtual void show() = 0;

        GtkWidget *get_widget();
    protected:
        GtkWidget *w;
        App *app;
};

#endif //__WIDGET_H__
