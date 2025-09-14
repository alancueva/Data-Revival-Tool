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
    GtkWidget *scan_combo;
    guint device_monitor_timer;

    void create_device_selection_section();
    // void create_options_section();
    void create_recovery_section();
    void create_progress_section();
    void create_files_section();
    void populate_sample_data();
    void refresh_device_list();
    void start_device_monitoring();
    void stop_device_monitoring();

    /**
     * Definición de columnas para el modelo de datos del TreeView
    */
    enum
    {
        COLUMN_CHECK,
        COLUMN_NAME,
        COLUMN_SIZE,
        COLUMN_DATE,
        NUM_COLUMNS
    };


public:
    RecoveryPanel();
    ~RecoveryPanel();

    GtkWidget* get_panel();

    static void on_recover_button_clicked(GtkWidget *widget, gpointer data);
    static void on_recover_selected_clicked(GtkWidget *widget, gpointer data);
    static gboolean on_device_monitor_timeout(gpointer user_data);
    // static void on_scan_option_toggled(GtkWidget *widget, gpointer data);
    static void on_toggle_checkbox(GtkCellRendererToggle *cell,
                                    gchar *path_str,
                                    gpointer user_data);
};

#endif