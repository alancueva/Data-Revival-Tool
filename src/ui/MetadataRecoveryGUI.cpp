#include "../../include/ui/MetadataRecoveryGUI.h"
#include "../../include/core/MetadataRecoveryEngine.h"
#include "../../include/ui/metadataPanel.h"
#include "../../include/ui/recoveryPanel.h"
#include <iostream>
#include <stdexcept>
#include <string>
#include <array>

MetadataRecoveryGUI::MetadataRecoveryGUI() : window(nullptr),
                                             m_header_bar(nullptr),
                                             m_button_header(nullptr),
                                             main_container(nullptr),
                                             current_panel(nullptr),
                                             engine(std::make_unique<MetadataRecoveryEngine>()),
                                             metadata_panel(std::make_unique<MetadataPanel>()),
                                             recovery_panel(std::make_unique<RecoveryPanel>())
{
    try
    {
        crear();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error en constructor de MetadataRecoveryGUI: " << e.what() << std::endl;
        throw;
    }
}

MetadataRecoveryGUI::~MetadataRecoveryGUI()
{
}

/**
 * @brief Cambia el ícono de la ventana principal.
 * @throws std::runtime_error si ocurre un error crítico
 */
void MetadataRecoveryGUI::cambiar_icono_ventana()
{
    // RAII wrapper para gchar* para asegurar la liberación de memoria correctamente
    struct GCharDeleter {
        void operator()(gchar* ptr) const { g_free(ptr); }
    };
    
    std::unique_ptr<gchar, GCharDeleter> dir(g_get_current_dir());
    if (!dir) {
        std::cerr << "Warning: Could not get current directory for icon loading" << std::endl;
        return;
    }
    
    std::unique_ptr<gchar, GCharDeleter> icon_path(
        g_build_filename(dir.get(), "resources", "icon",
                        "icons8-herramientas-del-administrador-50.png", nullptr));
    
    if (!icon_path) {
        std::cerr << "Warning: Could not build icon path" << std::endl;
        return;
    }

    GError *error = nullptr;
    if (!gtk_window_set_icon_from_file(GTK_WINDOW(window), icon_path.get(), &error)) {
        if (error) {
            std::cerr << "Warning: Could not load window icon: " << error->message << std::endl;
            g_error_free(error);
        }
    }
}

/**
 * @brief Crea e inicializa la ventana principal de la interfaz gráfica.
 * @throws std::runtime_error si falla la creación de la ventana
 */
void MetadataRecoveryGUI::crear()
{
     // Constantes de configuración de la ventana
    static constexpr int DEFAULT_WIDTH = 640;
    static constexpr int DEFAULT_HEIGHT = 480;
    static constexpr int BORDER_WIDTH = 10;
    static const char* WINDOW_TITLE = "Data Recovery Tool v1.0";

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    if (!window) {
        throw std::runtime_error("Failed to create main window");
    }

    gtk_window_set_title(GTK_WINDOW(window), WINDOW_TITLE);
    gtk_widget_set_size_request(window, DEFAULT_WIDTH, DEFAULT_HEIGHT);
    gtk_window_maximize(GTK_WINDOW(window));
    gtk_container_set_border_width(GTK_CONTAINER(window), BORDER_WIDTH);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), nullptr);

    // Cambia el ícono de la ventana
    cambiar_icono_ventana();

    // Crea la barra de encabezado
    m_header_bar = gtk_header_bar_new();
    if (!m_header_bar) {
        throw std::runtime_error("Failed to create header bar");
    }
    gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(m_header_bar), TRUE);
    gtk_window_set_titlebar(GTK_WINDOW(window), m_header_bar);

    create_header_buttons();

    // Crea el contenedor principal usando una pila para el intercambio de paneles
    main_container = gtk_stack_new();
    if (!main_container) {
        throw std::runtime_error("Failed to create main container");
    }
    gtk_container_add(GTK_CONTAINER(window), main_container);

    // Agrega los paneles a la pila
    gtk_stack_add_named(GTK_STACK(main_container), metadata_panel->get_panel(), "metadata");
    gtk_stack_add_named(GTK_STACK(main_container), recovery_panel->get_panel(), "recovery");

    // Muestra el panel inicial
    switch_to_panel("metadata");
}

/**
 * @brief Crea botones de la barra de encabezado con iconos y tooltips.
 * Utiliza prácticas modernas de C++ y gestión apropiada de recursos.
 */
void MetadataRecoveryGUI::create_header_buttons()
{
    // Datos de configuración de los botones
    struct ButtonConfig {
        const char* label;
        const char* tooltip;
        const char* icon_file;
        const char* fallback_icon;
        void (*callback)(GtkWidget*, gpointer);
    };
    
    static constexpr int ICON_SIZE = 20;
    static constexpr size_t BUTTON_COUNT = 2;
    
    const std::array<ButtonConfig, BUTTON_COUNT> buttons = {{
        {"Metadata", "Extract metadata from files", "data-recovery-30.png", 
         "document-properties", on_metadata_button_clicked},
        {"Recovery", "Recover metadata from deleted files", "recuperación-de-datos-30.png", 
         "drive-harddisk", on_recovery_button_clicked}
    }};

    // RAII wrapper para gchar*
    struct GCharDeleter {
        void operator()(gchar* ptr) const { g_free(ptr); }
    };
    
    std::unique_ptr<gchar, GCharDeleter> dir(g_get_current_dir());
    if (!dir) {
        std::cerr << "Warning: Could not get current directory for button icons" << std::endl;
        return;
    }

    for (const auto& btn_config : buttons) {
        GtkWidget* button = gtk_button_new_with_label(btn_config.label);
        if (!button) {
            std::cerr << "Warning: Failed to create button: " << btn_config.label << std::endl;
            continue;
        }
        
        gtk_button_set_relief(GTK_BUTTON(button), GTK_RELIEF_NONE);
        gtk_widget_set_tooltip_text(button, btn_config.tooltip);

        // Intenta cargar el icono personalizado
        GtkWidget* icon = nullptr;
        std::unique_ptr<gchar, GCharDeleter> icon_path(
            g_build_filename(dir.get(), "resources", "icon", btn_config.icon_file, nullptr));
        
        if (icon_path) {
            GError* error = nullptr;
            GdkPixbuf* pixbuf = gdk_pixbuf_new_from_file_at_scale(
                icon_path.get(), ICON_SIZE, ICON_SIZE, TRUE, &error);
            
            if (error) {
                std::cerr << "Warning: Could not load icon " << icon_path.get() 
                         << ": " << error->message << std::endl;
                g_error_free(error);
                // Usa el icono de respaldo
                icon = gtk_image_new_from_icon_name(btn_config.fallback_icon, GTK_ICON_SIZE_BUTTON);
            } else {
                icon = gtk_image_new_from_pixbuf(pixbuf);
                g_object_unref(pixbuf); // GtkImage ahora tiene la propiedad del pixbuf
            }
        } else {
            // Usa el icono de respaldo si la creación del path falla
            icon = gtk_image_new_from_icon_name(btn_config.fallback_icon, GTK_ICON_SIZE_BUTTON);
        }

        if (icon) {
            gtk_button_set_image(GTK_BUTTON(button), icon);
            gtk_button_set_image_position(GTK_BUTTON(button), GTK_POS_LEFT);
            gtk_button_set_always_show_image(GTK_BUTTON(button), TRUE);
        }
        
        gtk_button_set_use_underline(GTK_BUTTON(button), TRUE);
        gtk_header_bar_pack_start(GTK_HEADER_BAR(m_header_bar), button);
        
        // Conecta la señal del callback
        g_signal_connect(button, "clicked", G_CALLBACK(btn_config.callback), this);
    }
}

/**
 * @brief Muestra la ventana principal de la GUI.
 * @note Esta función está marcada como noexcept ya que no debe lanzar excepciones
 */
void MetadataRecoveryGUI::mostrar() noexcept
{
    if (window) {
        gtk_widget_show_all(window);
    } else {
        std::cerr << "Error: Attempting to show null window" << std::endl;
    }
}

void MetadataRecoveryGUI::on_metadata_button_clicked(GtkWidget* widget, gpointer data)
{
    if (auto* app = static_cast<MetadataRecoveryGUI*>(data)) {
        app->switch_to_panel("metadata");
    }
}

void MetadataRecoveryGUI::on_recovery_button_clicked(GtkWidget* widget, gpointer data)
{
    if (auto* app = static_cast<MetadataRecoveryGUI*>(data)) {
        app->switch_to_panel("recovery");
    }
}

/**
 * @brief Cambia el panel actual en la GUI.
 * @param panel_name El nombre del panel a mostrar ("metadata" o "recovery")
 * @note Esta función cambia el panel visible y actualiza el título de la barra de encabezado
 */
void MetadataRecoveryGUI::switch_to_panel(const char* panel_name)
{
    if (!panel_name || !main_container || !m_header_bar) {
        std::cerr << "Error: Invalid parameters in switch_to_panel" << std::endl;
        return;
    }
    
    gtk_stack_set_visible_child_name(GTK_STACK(main_container), panel_name);

    // Actualiza el título de la barra de encabezado según el panel activo
    if (g_strcmp0(panel_name, "metadata") == 0) {
        gtk_header_bar_set_title(GTK_HEADER_BAR(m_header_bar), "Metadata");
    } else if (g_strcmp0(panel_name, "recovery") == 0) {
        gtk_header_bar_set_title(GTK_HEADER_BAR(m_header_bar), "Recovery");
    } else {
        std::cerr << "Warning: Unknown panel name: " << panel_name << std::endl;
    }
}
