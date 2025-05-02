
#ifndef metadata_recovery_gui_h
#define metadata_recovery_gui_h

#include <gtk/gtk.h>
#include <string>
#include <iostream>
using namespace std;

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
    /**
     * @brief Constructor de la clase MetadataRecoveryGUI.
     */
    MetadataRecoveryGUI();
    /**
     * @brief Destructor de la clase MetadataRecoveryGUI.
     *
     */
    //~MetadataRecoveryGUI();

    /*
     * @brief Crea la ventana principal de la GUI.
     */
    void crear();
    /*
     * @brief Muestra la ventana principal de la GUI.
     */
    void mostrar();
    /*
     * @brief cambia el icono de la ventana principal de la GUI.
     */
    void cambiar_icono_ventana();

    /**
     * @brief Establece el contenido del GtkTextView.
     * @param text_view Puntero al GtkTextView.
     * @param content Contenido a establecer.
     * @return void
     */
    void set_text_view_content(GtkTextView *text_view, const string &content);

    /**
     * @brief Muestra un cuadro de diálogo con un mensaje.
     * @param parent Ventana padre del cuadro de diálogo.
     * @param message Mensaje a mostrar.
     * @param type Tipo de mensaje (información, advertencia, error).
     * @return void
     */
    void show_message_dialog(GtkWindow *parent, const gchar *message, GtkMessageType type);

    /**
     * @brief Callback para el evento de cierre de la ventana.
     * @param widget Puntero al widget que emitió la señal.
     * @param data Datos adicionales (no se utilizan en este caso).
     * @return void
     */
    gboolean on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer data);

    /**
     * @brief Carga un archivo seleccionado por el usuario y lo analiza.
     * @return void
     */
    void on_load_file_clicked();

    /*
     * @brief Maneja el evento de clic en el botón de archivo.
     * @param widget Puntero al widget que emitió la señal.
     * @param data Datos adicionales (puntero a la instancia de MetadataRecoveryGUI).
     * @return void
     */
    static void on_file_button_clicked(GtkWidget *widget, gpointer data);

    /**
     * @brief Actualiza la barra de progreso.
     * @return TRUE si la barra de progreso debe seguir actualizándose, FALSE si se debe detener.
     */
    gboolean update_progress_bar();

    /*
    * Función estática que actúa como intermediaria para la función de actualización de la barra de progreso.
    * @brief Actualiza la barra de progreso de forma estática.
    * @param data Puntero a los datos de la barra de progreso.
    * @return TRUE si la barra de progreso debe seguir actualizándose, FALSE si se debe detener.
    * 
    */
    static gboolean update_progress_bar_static(gpointer data);
    
};

#endif
