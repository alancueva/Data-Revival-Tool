#include "../../include/ui/recoveryPanel.h"
#include "../../include/recovery/devices.h"
#include "../../include/recovery/smart_info.h"
#include "../../include/utils/device_physical.h"
#include <iostream>

RecoveryPanel::RecoveryPanel() : panel(nullptr),
                                 device_combo(nullptr),
                                 quick_scan_check(nullptr),
                                 deep_scan_check(nullptr),
                                 progress_bar(nullptr),
                                 tree_view(nullptr),
                                 recover_button(nullptr),
                                 scan_combo(nullptr),
                                 health_indicator(nullptr),
                                 device_monitor_timer(0)
{
    // Configurar CSS para los indicadores de salud
    GtkCssProvider *provider = gtk_css_provider_new();
    const char *css = ""
        ".health-good { background-color: #00FF00; }"
        ".health-medium { background-color: #FFFF00; }"
        ".health-bad { background-color: #FF0000; }";
    
    gtk_css_provider_load_from_data(provider, css, -1, NULL);
    gtk_style_context_add_provider_for_screen(
        gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );
    g_object_unref(provider);

    panel = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_margin_start(panel, 20);
    gtk_widget_set_margin_end(panel, 20);
    gtk_widget_set_margin_top(panel, 20);
    gtk_widget_set_margin_bottom(panel, 20);

    GtkWidget *title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title), "<span size='x-large' weight='bold'>Recuperación de Datos</span>");
    gtk_box_pack_start(GTK_BOX(panel), title, FALSE, FALSE, 10);

    // Crear las secciones del panel
    create_device_selection_section();
    // create_options_section();
    create_progress_section();
    create_files_section();
    create_recovery_section();
    
    // Iniciar monitoreo automático de dispositivos
    start_device_monitoring();
}

RecoveryPanel::~RecoveryPanel()
{
    stop_device_monitoring();
}

/**
 * Crea la sección de selección de dispositivo en el panel
 */
void RecoveryPanel::create_device_selection_section()
{
    // Grupo de selección de dispositivo
    GtkWidget *device_frame = gtk_frame_new("Seleccionar dispositivo");
    gtk_box_pack_start(GTK_BOX(panel), device_frame, FALSE, FALSE, 10);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(device_frame), box);
    gtk_widget_set_margin_start(box, 10);
    gtk_widget_set_margin_end(box, 10);
    gtk_widget_set_margin_top(box, 10);
    gtk_widget_set_margin_bottom(box, 10);

    GtkWidget *row_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start(GTK_BOX(box), row_box, FALSE, FALSE, 0);
    
    GtkWidget *device_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(box), device_box, FALSE, FALSE, 5);

    // Crear un modelo de datos para el ComboBox (Columna 0: Texto a mostrar, Columna 1: Ruta real)
    GtkListStore *store = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);

    /**
     * Obtiene la lista de discos disponibles en el sistema.
     * @return Un vector de objetos DiskInfo que representan los discos disponibles.
     */
    std::vector<DiskInfo> disks = get_disks();
    for (const auto &disk : disks)
    {
        GtkTreeIter iter;
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter,
                           0, disk.display_name.c_str(),
                           1, disk.path.c_str(),
                           -1);
    }

    device_combo = gtk_combo_box_new_with_model(GTK_TREE_MODEL(store));
    g_object_unref(store);

    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
    gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(device_combo), renderer, TRUE);
    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(device_combo), renderer, "text", 0, NULL);

    if (!disks.empty())
    {
        gtk_combo_box_set_active(GTK_COMBO_BOX(device_combo), 0);
    }

    gtk_box_pack_start(GTK_BOX(row_box), device_combo, TRUE, TRUE, 0);

    // Añadir esta línea para conectar el callback
    g_signal_connect(device_combo, "changed", G_CALLBACK(on_device_combo_changed), this);

    GtkListStore *scan_store = gtk_list_store_new(1, G_TYPE_STRING);
    GtkTreeIter iter;

    gtk_list_store_append(scan_store, &iter);
    gtk_list_store_set(scan_store, &iter, 0, "Escaneo rápido", -1);

    gtk_list_store_append(scan_store, &iter);
    gtk_list_store_set(scan_store, &iter, 0, "Escaneo profundo", -1);

    scan_combo = gtk_combo_box_new_with_model(GTK_TREE_MODEL(scan_store));
    g_object_unref(scan_store);

    GtkCellRenderer *scan_renderer = gtk_cell_renderer_text_new();
    gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(scan_combo), scan_renderer, TRUE);
    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(scan_combo), scan_renderer, "text", 0, NULL);

    gtk_combo_box_set_active(GTK_COMBO_BOX(scan_combo), 0);
    gtk_box_pack_start(GTK_BOX(row_box), scan_combo, FALSE, FALSE, 0);

    // --- Botón de recuperación ---
    recover_button = gtk_button_new_with_label("Iniciar Recuperación");
    gtk_box_pack_start(GTK_BOX(row_box), recover_button, FALSE, FALSE, 0);

    g_signal_connect(recover_button, "clicked", G_CALLBACK(on_recover_button_clicked), this);
    
    // Añadir indicador de salud del disco
    health_indicator = gtk_progress_bar_new();
    gtk_progress_bar_set_text(GTK_PROGRESS_BAR(health_indicator), "Estado de salud: Desconocido");
    gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(health_indicator), TRUE);
    gtk_box_pack_start(GTK_BOX(device_box), health_indicator, FALSE, FALSE, 5);
    
    // Botón para escanear particiones eliminadas
    // GtkWidget *scan_partitions_button = gtk_button_new_with_label("Escanear particiones eliminadas");
    // g_signal_connect(scan_partitions_button, "clicked", G_CALLBACK(on_scan_partitions_clicked), this);
    // gtk_box_pack_start(GTK_BOX(device_box), scan_partitions_button, FALSE, FALSE, 5);
}


/**
 * Crea la sección de progreso en el panel
 */
void RecoveryPanel::create_progress_section()
{
    GtkWidget *progress_frame = gtk_frame_new("Progreso");
    gtk_box_pack_start(GTK_BOX(panel), progress_frame, FALSE, FALSE, 10);

    GtkWidget *progress_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(progress_frame), progress_box);
    gtk_widget_set_margin_start(progress_box, 10);
    gtk_widget_set_margin_end(progress_box, 10);
    gtk_widget_set_margin_top(progress_box, 10);
    gtk_widget_set_margin_bottom(progress_box, 10);

    progress_bar = gtk_progress_bar_new();
    gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progress_bar), "0%");
    gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(progress_bar), TRUE);
    gtk_box_pack_start(GTK_BOX(progress_box), progress_bar, FALSE, FALSE, 0);
}

/**
 * Crea la sección de archivos encontrados en una tabla
 */
void RecoveryPanel::create_files_section()
{
    // Lista de archivos encontrados
    GtkWidget *files_frame = gtk_frame_new("Archivos encontrados");
    gtk_box_pack_start(GTK_BOX(panel), files_frame, TRUE, TRUE, 10);

    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(files_frame), scroll);

    // Crear el modelo
    GtkListStore *store = gtk_list_store_new(NUM_COLUMNS,
                                             G_TYPE_BOOLEAN, // checkbox
                                             G_TYPE_STRING,  // nombre
                                             G_TYPE_STRING,  // tamaño
                                             G_TYPE_STRING); // fecha

    // Crear el tree view con el modelo
    tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    gtk_container_add(GTK_CONTAINER(scroll), tree_view);

    // ---- Columna Checkbox ----
    GtkCellRenderer *toggle_renderer = gtk_cell_renderer_toggle_new();
    g_object_set(toggle_renderer, "activatable", TRUE, NULL);

    GtkTreeViewColumn *toggle_column = gtk_tree_view_column_new_with_attributes(
        " ", toggle_renderer,
        "active", COLUMN_CHECK,
        NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), toggle_column);

    g_signal_connect(toggle_renderer, "toggled", G_CALLBACK(on_toggle_checkbox), tree_view);

    // Crear las columnas del tree view
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();

    GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes("Nombre", renderer, "text", COLUMN_NAME, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);

    column = gtk_tree_view_column_new_with_attributes("Tamaño", renderer, "text", COLUMN_SIZE, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);

    column = gtk_tree_view_column_new_with_attributes("Fecha", renderer, "text", COLUMN_DATE, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);

    g_object_unref(store);
}

/**
 * Simulación de archivos recuperados:
 * recorre todo el vector y los muestra en el tree view
 */
void RecoveryPanel::populate_sample_data()
{
    // Supongamos que estos son los archivos recuperados
    std::vector<std::tuple<std::string, std::string, std::string>> recovered_files = {
        {"documento.pdf", "1.2 MB", "01/05/2025"},
        {"imagen.jpg", "850 KB", "28/04/2025"},
        {"archivo.txt", "15 KB", "30/04/2025"}};

    GtkListStore *store = gtk_list_store_new(3, G_TYPE_BOOLEAN,
                                             G_TYPE_STRING,
                                             G_TYPE_STRING,
                                             G_TYPE_STRING);

    for (const auto &file : recovered_files)
    {
        GtkTreeIter iter;
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter,
                           COLUMN_CHECK, FALSE,                     // checkbox desmarcado por defecto
                           COLUMN_NAME, std::get<0>(file).c_str(), // nombre
                           COLUMN_SIZE, std::get<1>(file).c_str(), // tamaño
                           COLUMN_DATE, std::get<2>(file).c_str(), // fecha
                           -1);
    }

    gtk_tree_view_set_model(GTK_TREE_VIEW(tree_view), GTK_TREE_MODEL(store));
    g_object_unref(store);
}

/**
 * Obtiene el panel de recuperación.
 */
GtkWidget *RecoveryPanel::get_panel()
{
    return panel;
}

/**
 * Callback para el botón de recuperación.
 * Inicia la simulación de la recuperación de archivos y actualiza la barra de progreso.
 */
void RecoveryPanel::on_recover_button_clicked(GtkWidget *widget, gpointer data)
{
    RecoveryPanel *panel = static_cast<RecoveryPanel *>(data);

    // Obtener la ruta real del dispositivo seleccionado desde el modelo del ComboBox
    gchar *device_path = nullptr;
    GtkTreeIter iter;
    if (gtk_combo_box_get_active_iter(GTK_COMBO_BOX(panel->device_combo), &iter))
    {
        GtkTreeModel *model = gtk_combo_box_get_model(GTK_COMBO_BOX(panel->device_combo));
        // Obtener el valor de la columna 1 (la ruta real)
        gtk_tree_model_get(model, &iter, 1, &device_path, -1);
    }

    if (!device_path)
    {
        // No hay dispositivo seleccionado o algo salió mal
        return;
    }
    std::cout << "Iniciando recuperación en: " << device_path << std::endl;

    // Simulación de avance de la recuperación con una barra de progreso suave
    struct ProgressData
    {
        RecoveryPanel *panel;
        double fraction;
    };

    /**
     * Callback para actualizar la barra de progreso.
     * Este callback se llama periódicamente para actualizar la barra de progreso
     * durante la simulación de la recuperación de archivos.
     */
    auto progress_callback = [](gpointer user_data) -> gboolean
    {
        ProgressData *data = static_cast<ProgressData *>(user_data);
        if (data->fraction >= 1.0)
        {
            gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(data->panel->progress_bar), 1.0);
            gtk_progress_bar_set_text(GTK_PROGRESS_BAR(data->panel->progress_bar), "100 %");
            gtk_widget_set_sensitive(data->panel->recover_button, TRUE);
            gtk_widget_set_sensitive(data->panel->quick_scan_check, TRUE);
            gtk_widget_set_sensitive(data->panel->deep_scan_check, TRUE);
            // Mostrar los datos recuperados al finalizar
            data->panel->populate_sample_data();
            delete data;
            return G_SOURCE_REMOVE;
        }
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(data->panel->progress_bar), data->fraction);
        int percent = static_cast<int>(data->fraction * 100);
        std::string text = std::to_string(percent) + " %";
        gtk_progress_bar_set_text(GTK_PROGRESS_BAR(data->panel->progress_bar), text.c_str());
        // Incrementa el progreso
        data->fraction += 0.05;
        return G_SOURCE_CONTINUE;
    };

    // Deshabilita controles mientras progresa
    gtk_widget_set_sensitive(panel->recover_button, FALSE);
    gtk_widget_set_sensitive(panel->quick_scan_check, FALSE);
    gtk_widget_set_sensitive(panel->deep_scan_check, FALSE);

    // Limpia los datos previos antes de iniciar la recuperación
    gtk_tree_view_set_model(GTK_TREE_VIEW(panel->tree_view), NULL);

    ProgressData *progress_data = new ProgressData{panel, 0.0};
    g_timeout_add(100, progress_callback, progress_data);

    g_free(device_path);
}


/**
 * Seccion de recuperación
 * estará debajo de la tabla de vista de archivos
 */
void RecoveryPanel::create_recovery_section()
{
    GtkWidget *recovery_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start(GTK_BOX(panel), recovery_box, FALSE, FALSE, 10);

    GtkWidget *status_label = gtk_label_new("Seleccione un archivo para recuperar:");
    gtk_box_pack_start(GTK_BOX(recovery_box), status_label, TRUE, FALSE, 0);

    GtkWidget *recover_selected_button = gtk_button_new_with_label("Recuperar");
    gtk_widget_set_sensitive(recover_selected_button, TRUE);
    gtk_box_pack_start(GTK_BOX(recovery_box), recover_selected_button, FALSE, FALSE, 0);
    
    g_signal_connect(recover_selected_button, "clicked", G_CALLBACK(on_recover_selected_clicked), this);

}

/**
 * Callback para el cambio de estado del checkbox en la vista de archivos
 */
void RecoveryPanel::on_toggle_checkbox(GtkCellRendererToggle *cell,
                                       gchar *path_str,
                                       gpointer user_data)
{
    GtkTreeView *view = GTK_TREE_VIEW(user_data);
    GtkTreeModel *model = gtk_tree_view_get_model(view);
    if (!model) {
        std::cerr << "Error: No se pudo obtener el modelo del TreeView" << std::endl;
        return;
    }

    GtkTreePath *path = gtk_tree_path_new_from_string(path_str);
    if (!path) {
        std::cerr << "Error: No se pudo crear el path desde string: " << path_str << std::endl;
        return;
    }

    GtkTreeIter iter;
    if (gtk_tree_model_get_iter(model, &iter, path)) {
        gboolean current_state = FALSE;
        gtk_tree_model_get(model, &iter, COLUMN_CHECK, &current_state, -1);

        // Cambiar el estado del checkbox
        gboolean new_state = !current_state;
        
        if (GTK_IS_LIST_STORE(model)) {
            gtk_list_store_set(GTK_LIST_STORE(model), &iter, COLUMN_CHECK, new_state, -1);
            
            // Obtener información del archivo para debug
            gchar *filename = nullptr;
            gtk_tree_model_get(model, &iter, COLUMN_NAME, &filename, -1);
            /**
             * Liberar la memoria asignada al filename
             */
            if (filename) g_free(filename);
        } else {
            std::cerr << "Error: El modelo no es un ListStore" << std::endl;
        }
    } else {
        std::cerr << "Error: No se pudo obtener el iterador para el path: " << path_str << std::endl;
    }

    gtk_tree_path_free(path);
}

/**
 * Actualiza la lista de dispositivos en el ComboBox
 * Mantiene la selección actual si el dispositivo sigue disponible
 * 
 */
void RecoveryPanel::refresh_device_list()
{
    if (!device_combo) {
        std::cerr << "Error: device_combo es nulo" << std::endl;
        return;
    }

    // Guardar la selección actual
    gchar *current_device_path = nullptr;
    GtkTreeIter iter;
    if (gtk_combo_box_get_active_iter(GTK_COMBO_BOX(device_combo), &iter)) {
        GtkTreeModel *model = gtk_combo_box_get_model(GTK_COMBO_BOX(device_combo));
        if (model) {
            gtk_tree_model_get(model, &iter, 1, &current_device_path, -1);
        }
    }

    // Crear nuevo modelo con dispositivos actualizados
    GtkListStore *new_store = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
    std::vector<DiskInfo> disks = get_disks();

    int new_selection_index = -1;
    int index = 0;
    
    for (const auto &disk : disks) {
        GtkTreeIter new_iter;
        gtk_list_store_append(new_store, &new_iter);
        gtk_list_store_set(new_store, &new_iter,
                           0, disk.display_name.c_str(),
                           1, disk.path.c_str(),
                           -1);
        
        // Verificar si este es el dispositivo previamente seleccionado
        if (current_device_path && disk.path == std::string(current_device_path)) {
            new_selection_index = index;
        }
        index++;
    }

    // Actualizar el modelo del ComboBox
    gtk_combo_box_set_model(GTK_COMBO_BOX(device_combo), GTK_TREE_MODEL(new_store));
    g_object_unref(new_store);

    // Restaurar selección o seleccionar el primero
    if (new_selection_index >= 0) {
        gtk_combo_box_set_active(GTK_COMBO_BOX(device_combo), new_selection_index);
    } else if (!disks.empty()) {
        gtk_combo_box_set_active(GTK_COMBO_BOX(device_combo), 0);
    }

    // Añadir estas líneas para actualizar la información de salud
    // después de seleccionar un dispositivo
    gchar *selected_device_path = nullptr;
    if (gtk_combo_box_get_active_iter(GTK_COMBO_BOX(device_combo), &iter)) {
        GtkTreeModel *model = gtk_combo_box_get_model(GTK_COMBO_BOX(device_combo));
        gtk_tree_model_get(model, &iter, 1, &selected_device_path, -1);
        
        if (selected_device_path) {
            update_device_health(selected_device_path);
            g_free(selected_device_path);
        }
    }

    if (current_device_path) g_free(current_device_path);
}

/**
 * Inicia el monitoreo automático de dispositivos
 * Revisa cada 3 segundos si hay cambios en los dispositivos
 */
void RecoveryPanel::start_device_monitoring()
{
    if (device_monitor_timer != 0) {
        stop_device_monitoring();
    }
    
    // Timer que se ejecuta cada 3000ms (3 segundos)
    device_monitor_timer = g_timeout_add(3000, on_device_monitor_timeout, this);
}

/**
 * Detiene el monitoreo automático de dispositivos
 * Elimina el timer y lo establece en 0
 */
void RecoveryPanel::stop_device_monitoring()
{
    if (device_monitor_timer != 0) {
        g_source_remove(device_monitor_timer);
        device_monitor_timer = 0;
    }
}

/**
 * Callback del timer de monitoreo automático
 * Se ejecuta periódicamente para verificar cambios en dispositivos
 */
gboolean RecoveryPanel::on_device_monitor_timeout(gpointer user_data)
{
    RecoveryPanel *panel = static_cast<RecoveryPanel *>(user_data);
    if (!panel) {
        std::cerr << "Error: Panel es nulo en monitor timeout" << std::endl;
        return G_SOURCE_REMOVE; // Detener el timer
    }

    // Obtener dispositivos actuales
    std::vector<DiskInfo> current_disks = get_disks();
    
    // Obtener dispositivos del ComboBox actual
    std::vector<std::string> combo_devices;
    if (panel->device_combo) {
        GtkTreeModel *model = gtk_combo_box_get_model(GTK_COMBO_BOX(panel->device_combo));
        if (model) {
            GtkTreeIter iter;
            gboolean valid = gtk_tree_model_get_iter_first(model, &iter);
            
            while (valid) {
                gchar *device_path = nullptr;
                gtk_tree_model_get(model, &iter, 1, &device_path, -1);
                if (device_path) {
                    combo_devices.push_back(std::string(device_path));
                    g_free(device_path);
                }
                valid = gtk_tree_model_iter_next(model, &iter);
            }
        }
    }
    
    // Verificar si hay cambios (diferentes cantidades o dispositivos diferentes)
    bool devices_changed = false;
    if (current_disks.size() != combo_devices.size()) {
        devices_changed = true;
    } else {
        // Verificar si los dispositivos son los mismos
        for (const auto& disk : current_disks) {
            bool found = false;
            for (const auto& combo_device : combo_devices) {
                if (disk.path == combo_device) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                devices_changed = true;
                break;
            }
        }
    }
    
    // Si hay cambios, actualizar la lista
    if (devices_changed) {
        std::cout << "🔄 Cambios detectados en dispositivos - Actualizando automáticamente..." << std::endl;
        panel->refresh_device_list();
    }
    
    return G_SOURCE_CONTINUE; // Continuar el timer
}

/**
 * Callback para el botón "Recuperar Seleccionados"
 * Procesa solo los archivos que tienen el checkbox marcado
 */
void RecoveryPanel::on_recover_selected_clicked(GtkWidget *widget, gpointer data)
{
    // Validación de parámetros
    if (!widget || !data) {
        std::cerr << "Error: Parámetros inválidos en on_recover_selected_clicked" << std::endl;
        return;
    }

    RecoveryPanel *panel = static_cast<RecoveryPanel *>(data);
    if (!panel || !panel->tree_view) {
        std::cerr << "Error: Panel o TreeView son nulos" << std::endl;
        return;
    }

    // Obtener el modelo actual
    GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(panel->tree_view));
    if (!model) {
        std::cerr << "Error: No hay modelo en el TreeView" << std::endl;
        return;
    }

    // Contar archivos seleccionados
    std::vector<std::string> selected_files;
    GtkTreeIter iter;
    gboolean valid = gtk_tree_model_get_iter_first(model, &iter);

    while (valid) {
        gboolean checked = FALSE;
        gchar *filename = nullptr;
        
        gtk_tree_model_get(model, &iter, 
                          COLUMN_CHECK, &checked,
                          COLUMN_NAME, &filename,
                          -1);

        if (checked && filename) {
            selected_files.push_back(std::string(filename));
            std::cout << "Archivo seleccionado para recuperar: " << filename << std::endl;
        }

        if (filename) g_free(filename);
        valid = gtk_tree_model_iter_next(model, &iter);
    }

    if (selected_files.empty()) {
        std::cout << "No hay archivos seleccionados para recuperar" << std::endl;
        return;
    }

    std::cout << "Iniciando recuperación de " << selected_files.size() << " archivo(s) seleccionado(s)" << std::endl;
    
    // Por ahora, solo simulamos el proceso
    for (const auto& file : selected_files) {
        std::cout << "Recuperando: " << file << "... ✓" << std::endl;
    }
    
    std::cout << "Recuperación completada exitosamente!" << std::endl;
}

/**
 * Callback para el botón de escanear particiones eliminadas
 */
// void RecoveryPanel::on_scan_partitions_clicked(GtkWidget *widget, gpointer data)
// {
//     RecoveryPanel *panel = static_cast<RecoveryPanel *>(data);
//     if (!panel) {
//         std::cerr << "Error: Panel es nulo en on_scan_partitions_clicked" << std::endl;
//         return;
//     }
    
//     // Obtener la ruta del dispositivo seleccionado
//     gchar *device_path = nullptr;
//     GtkTreeIter iter;
//     if (gtk_combo_box_get_active_iter(GTK_COMBO_BOX(panel->device_combo), &iter)) {
//         GtkTreeModel *model = gtk_combo_box_get_model(GTK_COMBO_BOX(panel->device_combo));
//         gtk_tree_model_get(model, &iter, 1, &device_path, -1);
//     }
    
//     if (!device_path) {
//         std::cerr << "Error: No hay dispositivo seleccionado" << std::endl;
//         return;
//     }
    
//     std::cout << "Escaneando particiones eliminadas en: " << device_path << std::endl;
//     // Aquí iría la lógica para escanear particiones eliminadas
    
//     g_free(device_path);
// }

/**
 * Callback para cuando cambia la selección del dispositivo
 */
void RecoveryPanel::on_device_combo_changed(GtkComboBox *combo, gpointer data)
{
    RecoveryPanel *panel = static_cast<RecoveryPanel *>(data);
    if (!panel) {
        std::cerr << "Error: Panel es nulo en on_device_combo_changed" << std::endl;
        return;
    }
    
    // Obtener la ruta del dispositivo seleccionado
    gchar *device_path = nullptr;
    GtkTreeIter iter;
    if (gtk_combo_box_get_active_iter(combo, &iter)) {
        GtkTreeModel *model = gtk_combo_box_get_model(combo);
        gtk_tree_model_get(model, &iter, 1, &device_path, -1);
        
        if (device_path) {
            // Actualizar información de salud
            panel->update_device_health(device_path);
            g_free(device_path);
        }
    }
}

/**
 * Actualiza la información de salud del dispositivo seleccionado
 */
void RecoveryPanel::update_device_health(const std::string& device_path)
{
     if (!health_indicator || !GTK_IS_PROGRESS_BAR(health_indicator)) {
        std::cerr << "Error: health_indicator no está inicializado o no es un GtkProgressBar." << std::endl;
        return;
    }

    if (!gtk_widget_get_realized(health_indicator)) {
        std::cerr << "Error: health_indicator no está agregado a un contenedor GTK." << std::endl;
        return;
    }

    GtkStyleContext *context = gtk_widget_get_style_context(health_indicator);
    if (!context) {
        std::cerr << "Error: No se pudo obtener el contexto de estilo." << std::endl;
        return;
    }

    int health = -1, temp = -1;

    /**
     * Verifica si el dispositivo es físico antes de intentar obtener información SMART
     * Esto evita errores al intentar leer información de dispositivos virtuales
     */
    if (!DevicePhysical::is_physical_disk(device_path)) {
        gtk_style_context_remove_class(context, "health-good");
        gtk_style_context_remove_class(context, "health-medium");
        gtk_style_context_remove_class(context, "health-bad");
        gtk_progress_bar_set_text(GTK_PROGRESS_BAR(health_indicator), "Estado de salud: No disponible");
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(health_indicator), 0.0);
        return;
    }

    if (get_smart_info(device_path, health, temp)) {
        std::cout << "Salud obtenida: " << health << "%, Temperatura: " << temp << "°C" << std::endl;
        std::string health_text = "Estado de salud: " + std::to_string(health) + "%";
        if (temp > 0) {
            health_text += " | Temperatura: " + std::to_string(temp) + "°C";
        }
        
        gtk_progress_bar_set_text(GTK_PROGRESS_BAR(health_indicator), health_text.c_str());
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(health_indicator), health / 100.0);

        // Primero, eliminar clases anteriores si existen
        gtk_style_context_remove_class(context, "health-good");
        gtk_style_context_remove_class(context, "health-medium");
        gtk_style_context_remove_class(context, "health-bad");
        
        // Añadir la clase apropiada según el estado de salud
        if (health < 50) {
            // Rojo para mala salud
            gtk_style_context_add_class(context, "health-bad");
        } else if (health < 80) {
            // Amarillo para salud media
            gtk_style_context_add_class(context, "health-medium");
        } else {
            // Verde para buena salud
            gtk_style_context_add_class(context, "health-good");
        }
    } else {
        gtk_style_context_remove_class(context, "health-good");
        gtk_style_context_remove_class(context, "health-medium");
        gtk_style_context_remove_class(context, "health-bad");
        gtk_widget_set_name(health_indicator, "");
        gtk_progress_bar_set_text(GTK_PROGRESS_BAR(health_indicator), "Estado de salud: No disponible");
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(health_indicator), 0.0);
    }
}
