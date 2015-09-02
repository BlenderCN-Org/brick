#include "widget.h"

Widget::Widget()
{
    app = NULL;
    w = NULL;
}

Widget::Widget(App *a)
{
    app = a;
    w = NULL;
}

Widget::~Widget()
{

}

GtkWidget *Widget::get_widget()
{
    return w;
}
