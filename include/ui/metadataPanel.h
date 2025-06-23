#ifndef METADATAPANEL_H
#define METADATAPANEL_H

#include <gtk/gtk.h>
#include <memory>
#include <string>
using namespace std;

class MetadataRecoveryEngine;

/**
 * @brief Clase Panel que representa un panel en la interfaz gráfica de usuario (GUI).
 * Esta clase es una clase abstracta que define la estructura y comportamiento básico de un panel.
 * Las clases derivadas de Panel deben implementar los métodos virtuales para crear y gestionar la interfaz de usuario del panel.;
 * @file MetadataPanel.h
 * @brief Clase MetadataPanel que hereda de Panel.
 * Esta clase representa un panel de metadatos en la interfaz gráfica de usuario (GUI).
 * Proporciona métodos para crear y gestionar la interfaz de usuario del panel de metadatos.
 * Esta clase también incluye funcionalidades para cargar y guardar metadatos.
 * Además, permite la visualización y edición de metadatos existentes.
 */
class MetadataPanel
{
private:
    GtkWidget *panel;
    GtkWidget *file_entry;
    GtkWidget *text_view;

    unique_ptr<MetadataRecoveryEngine> engine;

    void create_file_selection_section();

    void create_results_section();

    /**
     * @brief Método que se llama cuando se hace clic en el botón de eliminar metadatos.
     * @param widget El widget que generó la señal.
     */
    static void on_delete_metadata_button_clicked(GtkWidget *widget, gpointer data);

    /**
     * @brief Método que se llama cuando se hace clic en el botón de examinar.
     * @param widget El widget que generó la señal.
     */
    static void on_browse_button_clicked(GtkWidget *widget, gpointer data);
    
    /**
     * @brief Método que se llama cuando se hace clic en el botón de analizar.
     * @param widget El widget que generó la señal.
     */
    static void on_analyze_button_clicked(GtkWidget *widget, gpointer data);

public:
    MetadataPanel();
    ~MetadataPanel();
    GtkWidget *get_panel();
};

#endif