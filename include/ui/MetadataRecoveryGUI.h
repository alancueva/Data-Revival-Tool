#pragma once

#include <gtk/gtk.h>
#include "../core/MetadataRecoveryEngine.h"

class MetadataRecoveryGUI
{
private:
    GtkWidget *window;
    GtkWidget *file_entry;
    GtkWidget *text_view;
    GtkWidget *progress_bar;
    GtkWidget *status_label;
    GtkWidget *file_info_label;
    GtkWidget *scan_combo;
    GtkWidget *basic_scan_button;
    GtkWidget *recover_button;
    GtkWidget *deep_scan_button;
    MetadataRecoveryEngine *engine;
    bool fileLoaded;

    MetadataRecoveryEngine engine; // Instancia del motor de recuperación de metadatos

    void setup_file_selection(GtkWidget *container);
    void setup_analysis_options(GtkWidget *container);
    void setup_results_area(GtkWidget *container);

    void setup_scan_options(GtkWidget *container);
    void setup_results_display(GtkWidget *container);
    void setup_progress_bar(GtkWidget *container);
    void setup_status_label(GtkWidget *container);
    void setup_buttons(GtkWidget *container);


    static void on_load_file_clicked(GtkWidget *widget, gpointer data);
    static void on_basic_scan_clicked(GtkWidget *widget, gpointer data);
    static void on_recover_clicked(GtkWidget *widget, gpointer data);
    static void on_deep_scan_clicked(GtkWidget *widget, gpointer data);
    static gboolean update_progress_bar(gpointer data);

    void update_status(const std::string &message);
    void set_text_view_content(const std::string &content);
    void enable_scan_buttons(bool enable);

    MetadataRecoveryGUI(const MetadataRecoveryGUI&) = delete; // Deshabilitar copia
    MetadataRecoveryGUI& operator=(const MetadataRecoveryGUI&) = delete; // Deshabilitar asignación
    MetadataRecoveryGUI(MetadataRecoveryGUI&&) = delete; // Deshabilitar movimiento
    MetadataRecoveryGUI& operator=(MetadataRecoveryGUI&&) = delete; // Deshabilitar movimiento

public:
    MetadataRecoveryGUI();
    ~MetadataRecoveryGUI();

    void create();
    void show();
    void hide();
    void run(); // Ejecutar el bucle principal de GTK
    GtkWidget* get_window() const { return window; }
    bool is_file_loaded() const { return fileLoaded; }

    GtkWidget* get_progress_bar() const;
    GtkWidget* get_text_view() const; 
    GtkWidget* get_status_label() const;
    GtkWidget* get_file_info_label() const;
    GtkWidget* get_scan_combo() const;
    MetadataRecoveryEngine* get_engine();


};
