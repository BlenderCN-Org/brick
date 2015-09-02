#ifndef _TOOLBOX_H_
#define _TOOLBOX_H_

#include <gtk/gtk.h>

#include "widget.h"

class MainWindow;

class Toolbox : public Widget {
    friend class MainWindow;
    public:
        Toolbox();
        Toolbox(App *a);
        ~Toolbox();

        bool init();
        void show();
};

#endif // TOOLBOX_H
