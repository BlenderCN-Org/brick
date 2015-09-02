#ifndef __PROPERTY_PANEL_H__
#define __PROPERTY_PANEL_H__

#include <gtk/gtk.h>

#include "widget.h"

class MainWindow;

class PropertyPanel : public Widget {
    friend class MainWindow;
    public:
        PropertyPanel();
        PropertyPanel(App *a);
        ~PropertyPanel();

        bool init();
        void show();
    protected:
};

#endif // __PROPERTY_PANEL_H__
