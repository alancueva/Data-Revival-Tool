#include "../../include/ui/MetadataRecoveryGUI.h"
#include "../../include/core/MetadataRecoveryEngine.h"
#include "../../include/ui/metadataPanel.h"
#include "../../include/ui/recoveryPanel.h"
#include <iostream>
#include <stdexcept>

MetadataRecoveryGUI::MetadataRecoveryGUI() : window(nullptr),
                                             m_header_bar(nullptr),
                                             m_button_header(nullptr),
                                             main_container(nullptr),
                                             current_panel(nullptr),
                                             engine(make_unique<MetadataRecoveryEngine>()),
                                             metadata_panel(make_unique<MetadataPanel>()),
                                             recovery_panel(make_unique<RecoveryPanel>())
{
    try
    {
        crear();
        if (metadata_panel)
        {
            switch_to_panel(metadata_panel->get_panel());
        }
    }
    catch (const exception &e)
    {
        cerr << "Error en constructor de MetadataRecoveryGUI: " << e.what() << std::endl;
        throw;
    }
}

MetadataRecoveryGUI::~MetadataRecoveryGUI()
{
}

/**
 * @brief cambia el icono de la ventana principal de la GUI.
 */
void MetadataRecoveryGUI::cambiar_icono_ventana()
{
    gchar *dir = g_get_current_dir();
    gchar *icon_path = g_build_filename(dir, "resources", "icon",
                                        "icons8-herramientas-del-administrador-50.png", NULL);
    if (!icon_path)
    {
        g_free(dir);
        return;
    }

    GError *error = NULL;
    if (!gtk_window_set_icon_from_file(GTK_WINDOW(window), icon_path, &error))
    {
        if (error)
        {
            // g_warning("No se pudo cargar el icono: %s", error->message);
            cerr << "Warning: No se pudo cargar el icono de la ventana: "
                 << error->message << endl;
            g_error_free(error);
        }
    }

    // Liberar memoria
    g_free(icon_path);
    g_free(dir);
}

/**
 * @brief Crea la ventana principal de la GUI.
 * @return void
 */
void MetadataRecoveryGUI::crear()
{

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    gtk_window_set_title(GTK_WINDOW(window), "Herramienta de recuperación de datos v1.0");
    gtk_widget_set_size_request(window, 640, 480);
    gtk_window_maximize(GTK_WINDOW(window));
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // icono de la ventana
    cambiar_icono_ventana();

    m_header_bar = gtk_header_bar_new();
    gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(m_header_bar), TRUE);
    gtk_header_bar_set_title(GTK_HEADER_BAR(m_header_bar), "Herramienta");
    gtk_window_set_titlebar(GTK_WINDOW(window), m_header_bar);

    create_header_buttons();

    main_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), main_container);
}

void MetadataRecoveryGUI::create_header_buttons()
{

    // Botones de la barra de menú
    const char *button_labels[] = {"Metadatos", "Recuperación"};
    const char *button_tips[] = {"Recuperación de Metadatos de un Archivos",
                                 "Recuperar Metadatos de Archivos Eliminados"};
    const char *button_icons[] = {
        "data-recovery-30.png",
        "recuperación-de-datos-30.png"};

    // tamaño de los iconos
    // Se puede cambiar el tamaño de los iconos según sea necesario
    const int icon_width = 20;
    const int icon_height = 20;

    for (int i = 0; i < 2; i++)
    {
        m_button_header = gtk_button_new_with_label(button_labels[i]);
        gtk_button_set_relief(GTK_BUTTON(m_button_header), GTK_RELIEF_NONE);
        gtk_widget_set_tooltip_text(m_button_header, button_tips[i]);

        gchar *dir = g_get_current_dir();
        gchar *icon_path = g_build_filename(dir, "resources", "icon", button_icons[i], NULL);
        GError *error = NULL;
        GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file_at_scale(icon_path, icon_width, icon_height, TRUE, &error);
        GtkWidget *icon = gtk_image_new_from_pixbuf(pixbuf);

        if (error)
        {
            g_warning("No se pudo cargar el icono %s: %s", icon_path, error->message);
            g_error_free(error);
            // Usar un icono por defecto de GTK
            if (i == 0)
            {
                GtkWidget *icon = gtk_image_new_from_icon_name("document-properties", GTK_ICON_SIZE_BUTTON);
                gtk_button_set_image(GTK_BUTTON(m_button_header), icon);
            }
            else
            {
                GtkWidget *icon = gtk_image_new_from_icon_name("drive-harddisk", GTK_ICON_SIZE_BUTTON);
                gtk_button_set_image(GTK_BUTTON(m_button_header), icon);
            }
        }
        else
        {
            GtkWidget *icon = gtk_image_new_from_pixbuf(pixbuf);
            gtk_button_set_image(GTK_BUTTON(m_button_header), icon);
            g_object_unref(pixbuf);
        }

        gtk_button_set_image_position(GTK_BUTTON(m_button_header), GTK_POS_LEFT);
        gtk_button_set_always_show_image(GTK_BUTTON(m_button_header), TRUE);
        gtk_button_set_use_underline(GTK_BUTTON(m_button_header), TRUE);
        gtk_header_bar_pack_start(GTK_HEADER_BAR(m_header_bar), m_button_header);

        if (i == 0)
        {
            g_signal_connect(m_button_header, "clicked", G_CALLBACK(on_metadata_button_clicked), this);
        }
        else
        {
            g_signal_connect(m_button_header, "clicked", G_CALLBACK(on_recovery_button_clicked), this);
        }

        if (pixbuf)
            g_object_unref(pixbuf);
        g_free(icon_path);
        g_free(dir);
    }
}

void MetadataRecoveryGUI::clear_current_panel()
{
    if (current_panel && GTK_IS_WIDGET(current_panel))
    {
        GtkWidget *parent = gtk_widget_get_parent(current_panel);
        if (parent == main_container)
        {
            gtk_container_remove(GTK_CONTAINER(main_container), current_panel);
        }
    }
}

/**
 * @brief Muestra la ventana principal de la GUI.
 */
void MetadataRecoveryGUI::mostrar() noexcept
{
    gtk_widget_show_all(window);
}

void MetadataRecoveryGUI::on_metadata_button_clicked(GtkWidget *widget, gpointer data)
{
    if (!data)
        return;

    auto *app = static_cast<MetadataRecoveryGUI *>(data);
    if (app->metadata_panel)
    {
        app->switch_to_panel(app->metadata_panel->get_panel());
    }
}

void MetadataRecoveryGUI::on_recovery_button_clicked(GtkWidget *widget, gpointer data)
{
    if (!data)
        return;

    auto *app = static_cast<MetadataRecoveryGUI *>(data);
    if (app->recovery_panel)
    {
        app->switch_to_panel(app->recovery_panel->get_panel());
    }
}

/**
 * @brief Cambia el panel actual de la GUI.
 * @param panel El panel a mostrar.
 * @return void
 * @note Esta función se utiliza para cambiar el panel actual de la GUI.
 * @note El panel actual se elimina de la ventana principal y se agrega el nuevo panel.
 */
void MetadataRecoveryGUI::switch_to_panel(GtkWidget *panel)
{
    // Limpiar panel actual
    clear_current_panel();

    g_object_ref(panel);
    gtk_box_pack_start(GTK_BOX(main_container), panel, TRUE, TRUE, 0);

    current_panel = panel;
    gtk_widget_show_all(main_container);
}
