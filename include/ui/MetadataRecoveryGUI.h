
#ifndef metadata_recovery_gui_h
#define metadata_recovery_gui_h

#include <gtk/gtk.h>
#include <string>
#include <memory>
using namespace std;

class MetadataRecoveryEngine;
class MetadataPanel;
class RecoveryPanel;

class MetadataRecoveryGUI
{
private:
    GtkWidget *window;
    GtkWidget *m_header_bar;
    GtkWidget *main_container;
    GtkWidget *m_button_header;
    GtkWidget *current_panel;

    unique_ptr<MetadataRecoveryEngine> engine;
    unique_ptr<MetadataPanel> metadata_panel;
    unique_ptr<RecoveryPanel> recovery_panel;

    /**
     * @brief Crea e inicializa la interfaz gráfica
     */
    void crear();
    
    /**
     * @brief Cambia el icono de la ventana principal
     */
    void cambiar_icono_ventana();

     /**
     * @brief Crea los botones de la barra de herramientas
     */
    void create_header_buttons();

    /**
     * @brief Cambia entre paneles de la aplicación
     * @param panel Panel a mostrar
     */
    void switch_to_panel(GtkWidget *panel);

     /**
     * @brief Limpia el panel actual de forma segura
     */
    void clear_current_panel();
public:
    /**
     * @brief Constructor de la clase MetadataRecoveryGUI.
     */
    MetadataRecoveryGUI();
    /**
     * @brief Destructor de la clase MetadataRecoveryGUI.
     *
     */
    ~MetadataRecoveryGUI();

     /**
     * @brief Muestra la ventana principal
     */
    void mostrar() noexcept;

    /**
     * @brief Callback para el botón de metadatos
     * @param widget Widget que generó la señal
     * @param data Puntero a la instancia de MetadataRecoveryGUI
     */
    static void on_metadata_button_clicked(GtkWidget* widget, gpointer data);
    
    /**
     * @brief Callback para el botón de recuperación
     * @param widget Widget que generó la señal
     * @param data Puntero a la instancia de MetadataRecoveryGUI
     */
    static void on_recovery_button_clicked(GtkWidget* widget, gpointer data);
    
};

#endif
