#ifndef METADATAPANEL_H
#define METADATAPANEL_H

#include <gtk/gtk.h>
#include <string>
using namespace std;

class FileMetadata;

/**
 * @brief Clase Panel que representa un panel en la interfaz gráfica de usuario (GUI).
 * Esta clase es una clase abstracta que define la estructura y comportamiento básico de un panel.
 * Las clases derivadas de Panel deben implementar los métodos virtuales para crear y gestionar la interfaz de usuario del panel.;
/**
 * @file MetadataPanel.h
 * @brief Clase MetadataPanel que hereda de Panel.
 * Esta clase representa un panel de metadatos en la interfaz gráfica de usuario (GUI).
 * Proporciona métodos para crear y gestionar la interfaz de usuario del panel de metadatos.
 * Esta clase también incluye funcionalidades para cargar y guardar metadatos.
 * Además, permite la visualización y edición de metadatos existentes.
 */
class MetadataPanel {
private:
    GtkWidget *file_frame;
    GtkWidget *file_grid;
    GtkWidget *file_button;
    GtkWidget *file_label;

    GtkWidget *file_chooser;
    GtkWidget *image_filter;
    GtkWidget *all_filter;
    GtkWidget *doc_filter;

    GtkWidget *scroll_window;
    GtkWidget *file_info_label;
    GtkWidget *progress_bar;
    GtkWidget *status_label;
    GtkWidget *main_vbox;
    GtkWidget *results_vbox;
    GtkWidget *results_frame;

    GtkWidget *panel;
    GtkWidget *file_entry;
    GtkWidget *text_view;
    
    FileMetadata *file_metadata;

    void create_file_selection_section();
    void create_results_section();

public:
    MetadataPanel();
    ~MetadataPanel() ;
    GtkWidget* get_panel();


    static void on_browse_button_clicked(GtkWidget *widget, gpointer data);
    static void on_analyze_button_clicked(GtkWidget *widget, gpointer data);
};

#endif