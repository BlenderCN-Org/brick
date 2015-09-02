#include <assert.h>

#include <gtk/gtk.h>

#include "property_panel.h"

PropertyPanel::PropertyPanel() : Widget() {}
PropertyPanel::PropertyPanel(App *a) : Widget(a) {}
PropertyPanel::~PropertyPanel() {}

bool PropertyPanel::init()
{
    GtkWidget *bt_load_file = NULL;
    GtkWidget *tb_filename = NULL;
    GtkWidget *lb_filename = NULL;

    GtkWidget *sb_scale_x = NULL;

    GtkWidget *hbox = NULL;

    w = gtk_vbox_new(FALSE, 0);
    assert(w);

    hbox = gtk_hbox_new(FALSE, 0);
    assert(hbox);

    bt_load_file = gtk_button_new_with_label("...");
    assert(bt_load_file);

    tb_filename = gtk_entry_new();
    assert(tb_filename);

    lb_filename = gtk_label_new("File name:");
    assert(lb_filename);

    gtk_box_pack_start(GTK_BOX(hbox), bt_load_file, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), lb_filename, FALSE, FALSE, 0);

   // sb_scale_x = gtk_spin_button_new()

    gtk_box_pack_start(GTK_BOX(w), hbox, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(w), tb_filename, FALSE, FALSE, 0);

    return true;
}

void PropertyPanel::show()
{

}
