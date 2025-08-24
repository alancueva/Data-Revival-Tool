#ifndef RECOVERY_PANEL_H
#define RECOVERY_PANEL_H

#include <gtk/gtk.h>

class RecoveryPanel {
private:
    GtkWidget *panel;
    GtkWidget *device_combo;
    GtkWidget *quick_scan_check;
    GtkWidget *deep_scan_check;
    GtkWidget *progress_bar;
    GtkWidget *tree_view;
    GtkWidget *recover_button;

    void create_device_selection_section();
    void create_options_section();
    void create_progress_section();
    void create_files_section();
    void populate_sample_data();

public:
    RecoveryPanel();
    ~RecoveryPanel();

    GtkWidget* get_panel();

    static void on_recover_button_clicked(GtkWidget *widget, gpointer data);
    static void on_scan_option_toggled(GtkWidget *widget, gpointer data);
};

#endif