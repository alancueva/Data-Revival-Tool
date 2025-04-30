
#ifndef metadata_recovery_gui_h
#define metadata_recovery_gui_h

#include <gtk/gtk.h>
#include <string>
#include <iostream>

class MetadataRecoveryEngine;

class MetadataRecoveryGUI
{
private:
    GtkWidget *window;
    GtkWidget *main_vbox;
    GtkWidget *file_frame; 
    GtkWidget *file_grid;
    GtkWidget *file_label; 
    GtkWidget *file_button;
    GtkWidget *file_entry;
    GtkWidget *file_info_label;
    GtkWidget *file_chooser;

    GtkWidget *text_view;

    GtkWidget *scan_combo;

    GtkWidget *options_grid;
    GtkWidget *options_frame;

    GtkWidget *results_frame;
    GtkWidget *results_vbox;
    GtkWidget *scroll_window;
    GtkWidget *progress_bar;
    GtkWidget *scan_label;
    GtkWidget *status_label;
    
    GtkFileFilter *image_filter;
    GtkFileFilter *doc_filter;
    GtkFileFilter *all_filter;
    GtkFileFilter *zip_filter;

    GtkWidget *basic_scan_button;
    GtkWidget *recover_button;
    GtkWidget *deep_scan_button;
    
    MetadataRecoveryEngine *engine;

public:
    MetadataRecoveryGUI(); 
    
    void crear();
    void mostrar();
    void cambiar_icono_ventana();
    // void on_load_file_clicked(GtkWidget *widget, gpointer data);
    void set_text_view_content(GtkTextView *text_view, const std::string &content);
    void show_message_dialog(GtkWindow *parent, const gchar *message, GtkMessageType type);
    // gboolean update_progress_bar(gpointer data);
    gboolean on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer data);
    //GtkWidget *widget, gpointer data
    void on_load_file_clicked();
    
    // Static callback for connecting to the GTK signal
    static void on_file_button_clicked(GtkWidget *widget, gpointer data);
    static gboolean update_progress_bar(gpointer data);
};

#endif 
