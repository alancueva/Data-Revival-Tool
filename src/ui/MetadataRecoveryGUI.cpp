#include "../../include/ui/MetadataRecoveryGUI.h"
#include "../../include/core/MetadataRecoveryEngine.h"
#include <iostream>

MetadataRecoveryGUI::MetadataRecoveryGUI() : window(nullptr),
                                             file_entry(nullptr),
                                             file_button(nullptr),
                                             file_chooser(nullptr),
                                             file_label(nullptr),
                                             file_frame(nullptr),
                                             file_grid(nullptr),
                                             scan_combo(nullptr),
                                             options_grid(nullptr),
                                             options_frame(nullptr),
                                             results_vbox(nullptr),
                                             scroll_window(nullptr),
                                             file_info_label(nullptr),
                                             text_view(nullptr),
                                             progress_bar(nullptr),
                                             scan_label(nullptr),
                                             status_label(nullptr),
                                             engine(new MetadataRecoveryEngine()),
                                             basic_scan_button(nullptr),
                                             recover_button(nullptr),
                                             deep_scan_button(nullptr),
                                             image_filter(nullptr),
                                             doc_filter(nullptr),
                                             all_filter(nullptr),
                                             zip_filter(nullptr)
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
    GError *error = NULL;
    if (!gtk_window_set_icon_from_file(GTK_WINDOW(window), icon_path, &error))
    {
        g_warning("No se pudo cargar el icono: %s", error->message);
        g_error_free(error);
    }
    // Liberar memoria
    g_free(icon_path);
    g_free(dir);
}

/*
 * @brief Maneja el evento de clic en el botón de archivo.
 * @param widget Puntero al widget que emitió la señal.
 * @param data Datos adicionales (puntero a la instancia de MetadataRecoveryGUI).
 * @return void
 */
void MetadataRecoveryGUI::on_file_button_clicked(GtkWidget *widget, gpointer data)
{
    /**
     * * Abre el cuadro de diálogo para seleccionar un archivo.
     * * Al seleccionar un archivo, se carga y analiza el archivo.
     */
    static_cast<MetadataRecoveryGUI *>(data)->on_load_file_clicked();
}

/**
 * @brief Crea la ventana principal de la GUI.
 * @return void
 */
void MetadataRecoveryGUI::crear()
{

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Herramienta de recuperación de datos v1.0");
    // gtk_window_fullscreen(GTK_WINDOW(window));

    gtk_widget_set_size_request(window, 640, 480);
    gtk_window_maximize(GTK_WINDOW(window));

    gtk_container_set_border_width(GTK_CONTAINER(window), 10);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    // icono de la ventana
    cambiar_icono_ventana();
    // Layout principal
    main_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(window), main_vbox);
    // Sección de selección de archivo
    file_frame = gtk_frame_new("Selección de Archivo");
    gtk_box_pack_start(GTK_BOX(main_vbox), file_frame, FALSE, FALSE, 0);

    GtkWidget *file_grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(file_grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(file_grid), 5);
    gtk_container_set_border_width(GTK_CONTAINER(file_grid), 10);
    gtk_container_add(GTK_CONTAINER(file_frame), file_grid);

    file_label = gtk_label_new("Ruta:");
    gtk_grid_attach(GTK_GRID(file_grid), file_label, 0, 0, 1, 1);

    file_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(file_entry), "Selecciona un archivo...");
    // gtk_entry_set_editable(GTK_ENTRY(file_entry), FALSE);
    gtk_grid_attach(GTK_GRID(file_grid), file_entry, 1, 0, 1, 1);

    /**
     * Boton Explorar...
     * @brief Botón para cargar el archivo seleccionado.
     */
    file_button = gtk_button_new_with_label("Explorar...");
    g_signal_connect(file_button, "clicked", G_CALLBACK(on_file_button_clicked), this);
    gtk_grid_attach(GTK_GRID(file_grid), file_button, 2, 0, 1, 1);

    // Sección de opciones de análisis
    options_frame = gtk_frame_new("Opciones de Análisis");
    gtk_box_pack_start(GTK_BOX(main_vbox), options_frame, FALSE, FALSE, 0);

    options_grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(options_grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(options_grid), 5);
    gtk_container_set_border_width(GTK_CONTAINER(options_grid), 10);
    gtk_container_add(GTK_CONTAINER(options_frame), options_grid);

    scan_label = gtk_label_new("Tipo de Análisis:");
    gtk_grid_attach(GTK_GRID(options_grid), scan_label, 0, 0, 1, 1);

    // ComboBox para seleccionar el tipo de análisis
    scan_combo = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(scan_combo), "Análisis Rápido");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(scan_combo), "Análisis Estándar");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(scan_combo), "Análisis Profundo");
    gtk_combo_box_set_active(GTK_COMBO_BOX(scan_combo), 1); // Seleccionar "Estándar" por defecto
    gtk_grid_attach(GTK_GRID(options_grid), scan_combo, 1, 0, 2, 1);

    // Sección de resultados
    results_frame = gtk_frame_new("Resultados");
    gtk_box_pack_start(GTK_BOX(main_vbox), results_frame, TRUE, TRUE, 0);

    results_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(results_vbox), 10);
    gtk_container_add(GTK_CONTAINER(results_frame), results_vbox);

    file_info_label = gtk_label_new("Información del archivo: -");
    gtk_widget_set_halign(file_info_label, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(results_vbox), file_info_label, FALSE, FALSE, 0);

    // Área de texto con scroll para mostrar resultados
    GtkWidget *scroll_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_size_request(scroll_window, -1, 300);
    gtk_box_pack_start(GTK_BOX(results_vbox), scroll_window, TRUE, TRUE, 0);

    text_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_WORD);
    gtk_container_add(GTK_CONTAINER(scroll_window), text_view);

    // Barra de progreso
    progress_bar = gtk_progress_bar_new();
    gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progress_bar), "Progreso");
    gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(progress_bar), TRUE);
    gtk_box_pack_start(GTK_BOX(results_vbox), progress_bar, FALSE, FALSE, 0);

    // Etiqueta de estado
    status_label = gtk_label_new("Estado: Esperando archivo...");
    gtk_widget_set_halign(status_label, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(results_vbox), status_label, FALSE, FALSE, 0);

    // Mostrar mensaje de bienvenida
    set_text_view_content(GTK_TEXT_VIEW(text_view),
                          "Bienvenido al Software de Recuperación de Metadatos\n\n"
                          "Este programa permite analizar archivos y recuperar metadatos que han sido sobrescritos o eliminados.\n\n"
                          "Instrucciones:\n"
                          "1. Haga clic en 'Explorar...' para seleccionar un archivo.\n"
                          "2. Seleccione el tipo de análisis deseado.\n"
                          "3. Use los botones para realizar diferentes tipos de escaneo.\n\n"
                          "Compatible con imágenes (JPG, PNG, TIFF...) y documentos (PDF, DOC, DOCX...).\n\n"
                          "Software libre y de código abierto (GNU GPL v3).");
}

/**
 * @brief Muestra la ventana principal de la GUI.
 */
void MetadataRecoveryGUI::mostrar()
{
    gtk_widget_show_all(window);
}

/**
 * @brief Callback para el evento de cierre de la ventana.
 * @param widget Puntero al widget que emitió la señal.
 * @param data Datos adicionales (no se utilizan en este caso).
 * @return void
 */
gboolean MetadataRecoveryGUI::on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
    if (event->keyval == GDK_KEY_F11)
    {
        static gboolean is_fullscreen = FALSE;

        if (is_fullscreen)
        {
            gtk_window_unfullscreen(GTK_WINDOW(window));
            is_fullscreen = FALSE;
        }
        else
        {
            gtk_window_fullscreen(GTK_WINDOW(window));
            is_fullscreen = TRUE;
        }

        return TRUE;
    }

    return FALSE;
}

/**
 * @brief Establece el contenido del GtkTextView.
 * @param text_view Puntero al GtkTextView.
 * @param content Contenido a establecer.
 * @return void
 */
void MetadataRecoveryGUI::set_text_view_content(GtkTextView *text_view, const std::string &content)
{
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(text_view);
    gtk_text_buffer_set_text(buffer, content.c_str(), -1);
}

/**
 * @brief Carga un archivo seleccionado por el usuario y lo analiza.
 * @return void
 */
void MetadataRecoveryGUI::on_load_file_clicked()
{

    /**
     * Abre el cuadro de diálogo para seleccionar un archivo.
     * Al seleccionar un archivo, se carga y analiza el archivo.
     */
    file_chooser = gtk_file_chooser_dialog_new("Abrir Archivo",
                                               GTK_WINDOW(window),
                                               GTK_FILE_CHOOSER_ACTION_OPEN,
                                               "_Cancelar", GTK_RESPONSE_CANCEL,
                                               "_Abrir", GTK_RESPONSE_ACCEPT,
                                               NULL);

    // Filtros para tipos de archivos
    image_filter = gtk_file_filter_new();
    gtk_file_filter_set_name(image_filter, "Imágenes (*.jpg, *.png, *.tiff, *.bmp)");
    gtk_file_filter_add_pattern(image_filter, "*.jpg");
    gtk_file_filter_add_pattern(image_filter, "*.jpeg");
    gtk_file_filter_add_pattern(image_filter, "*.png");
    gtk_file_filter_add_pattern(image_filter, "*.tiff");
    gtk_file_filter_add_pattern(image_filter, "*.bmp");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(file_chooser), image_filter);

    doc_filter = gtk_file_filter_new();
    gtk_file_filter_set_name(doc_filter, "Documentos (*.pdf, *.doc, *.docx)");
    gtk_file_filter_add_pattern(doc_filter, "*.pdf");
    gtk_file_filter_add_pattern(doc_filter, "*.doc");
    gtk_file_filter_add_pattern(doc_filter, "*.docx");
    gtk_file_filter_add_pattern(doc_filter, "*.xls");
    gtk_file_filter_add_pattern(doc_filter, "*.xlsx");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(file_chooser), doc_filter);

    all_filter = gtk_file_filter_new();
    gtk_file_filter_set_name(all_filter, "Todos los archivos (*)");
    gtk_file_filter_add_pattern(all_filter, "*");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(file_chooser), all_filter);

    if (gtk_dialog_run(GTK_DIALOG(file_chooser)) == GTK_RESPONSE_ACCEPT)
    {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_chooser));
        gtk_entry_set_text(GTK_ENTRY(file_entry), filename);
        gtk_label_set_text(GTK_LABEL(status_label), "Estado: Cargando archivo...");

        /**
         * Simular progreso de carga
         * Se puede eliminar en la versión final o mejorarlo adaptando segun la carga del archivo.
         */
        guint timeout_id = g_timeout_add(50, update_progress_bar_static, this);
        while (g_main_context_iteration(NULL, FALSE))
            ;

        // Cargar archivo en el motor
        if (engine->loadFile(filename))
        {
            gtk_label_set_text(GTK_LABEL(status_label), "Estado: Archivo cargado correctamente");
            gtk_label_set_text(GTK_LABEL(file_info_label), ("Información: " + engine->getFileType(filename)).c_str());

            // Habilitar botones
            gtk_widget_set_sensitive(basic_scan_button, TRUE);
            gtk_widget_set_sensitive(recover_button, TRUE);
            gtk_widget_set_sensitive(deep_scan_button, TRUE);

            // Mostrar metadata básica automáticamente
            std::string metadata = engine->getBasicMetadata();
            set_text_view_content(GTK_TEXT_VIEW(text_view), metadata);


        }
        else
        {
            gtk_label_set_text(GTK_LABEL(status_label), "Estado: Error al cargar el archivo o formato no compatible");
            show_message_dialog(GTK_WINDOW(window),
                                "No se pudo analizar el archivo seleccionado o el formato no es compatible.",
                                GTK_MESSAGE_WARNING);
        }

        g_source_remove(timeout_id);
        g_free(filename);
    }

    gtk_widget_destroy(file_chooser);
}


gboolean MetadataRecoveryGUI::update_progress_bar_static(gpointer data)
{
    return static_cast<MetadataRecoveryGUI *>(data)->update_progress_bar();
}

/**
 * @brief Actualiza la barra de progreso.
 * @param data Puntero a los datos de la barra de progreso.
 * @return TRUE si la barra de progreso debe seguir actualizándose, FALSE si se debe detener.
 */
gboolean MetadataRecoveryGUI::update_progress_bar()
{
    static int progress = 0;
    if (progress <= 100)
    {
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress_bar), progress / 100.0);
        progress += 5;
        return TRUE;
    }
    else
    {
        progress = 0;
        return FALSE;
    }
}

/**
 * @brief Muestra un cuadro de diálogo con un mensaje.
 * @param parent Ventana padre del cuadro de diálogo.
 * @param message Mensaje a mostrar.
 * @param type Tipo de mensaje (información, advertencia, error).
 * @return void
 */
void MetadataRecoveryGUI::show_message_dialog(GtkWindow *parent, const gchar *message, GtkMessageType type)
{
    GtkWidget *dialog = gtk_message_dialog_new(parent,
                                               GTK_DIALOG_DESTROY_WITH_PARENT,
                                               type,
                                               GTK_BUTTONS_OK,
                                               "%s", message);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}