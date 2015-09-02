#ifndef __MAIN_TOOLBAR_H__
#define __MAIN_TOOLBAR_H__

#include <gtk/gtk.h>

#include "widget.h"

class MainWindow;

class MainToolbar : public Widget {
    friend class MainWindow;
    public:
        MainToolbar();
        MainToolbar(App *a);
        ~MainToolbar();

        bool init();
        void show();
    protected:
};

#endif // __MAIN_TOOLBAR_H__
