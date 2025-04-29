
#ifndef metadata_recovery_gui_h
#define metadata_recovery_gui_h

#include <gtk/gtk.h>
#include <string>
#include <iostream>

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
    
    

public:
    MetadataRecoveryGUI(); 
    
    void crear();
    void mostrar();
    void cambiar_icono_ventana();

    void set_text_view_content(GtkTextView *text_view, const std::string &content);
    
    gboolean on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer data);
};

#endif 
