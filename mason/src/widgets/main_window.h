#ifndef __MAIN_WINDOW_H__
#define __MAIN_WINDOW_H__

#include <gtk/gtk.h>

#include "widget.h"
#include "main_toolbar.h"
#include "property_panel.h"

class MainWindow : public Widget {
    public:
        MainWindow();
        MainWindow(App *a);
        ~MainWindow();

        bool init();
        void show();

        void get_clear_color(float (&c)[4]) const;
        void set_titlebar_fname(const char *fname);
    private:
        float clear_color[4];
        MainToolbar *main_toolbar;
        PropertyPanel *prop_panel;
};

#endif // __MAIN_WINDOW_H__
