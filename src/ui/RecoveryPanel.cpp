#include "../../include/ui/recoveryPanel.h"
#include "../../include/recovery/devices.h"
#include <iostream>

RecoveryPanel::RecoveryPanel() : panel(nullptr),
                                 device_combo(nullptr),
                                 quick_scan_check(nullptr),
                                 deep_scan_check(nullptr),
                                 progress_bar(nullptr),
                                 tree_view(nullptr),
                                 recover_button(nullptr),
                                 scan_combo(nullptr)
{

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
}

RecoveryPanel::~RecoveryPanel()
{
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
}

/**
 * Crea la sección de opciones de recuperación en el panel
 *
 */
// void RecoveryPanel::create_options_section()
// {
//     GtkWidget *options_frame = gtk_frame_new("Opciones de recuperación");
//     gtk_box_pack_start(GTK_BOX(panel), options_frame, FALSE, FALSE, 10);

//     GtkWidget *options_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
//     gtk_container_add(GTK_CONTAINER(options_frame), options_box);
//     gtk_widget_set_margin_start(options_box, 10);
//     gtk_widget_set_margin_end(options_box, 10);
//     gtk_widget_set_margin_top(options_box, 10);
//     gtk_widget_set_margin_bottom(options_box, 10);

//     quick_scan_check = gtk_check_button_new_with_label("Escaneo rápido");
//     // gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(quick_scan_check), TRUE);
//     gtk_box_pack_start(GTK_BOX(options_box), quick_scan_check, FALSE, FALSE, 0);
//     g_signal_connect(quick_scan_check, "toggled", G_CALLBACK(on_scan_option_toggled), this);

//     deep_scan_check = gtk_check_button_new_with_label("Escaneo profundo");
//     gtk_box_pack_start(GTK_BOX(options_box), deep_scan_check, FALSE, FALSE, 0);
//     g_signal_connect(deep_scan_check, "toggled", G_CALLBACK(on_scan_option_toggled), this);

//     recover_button = gtk_button_new_with_label("Iniciar Recuperación");
//     gtk_widget_set_margin_top(recover_button, 10);
//     gtk_box_pack_start(GTK_BOX(panel), recover_button, FALSE, FALSE, 0);
//     g_signal_connect(recover_button, "clicked", G_CALLBACK(on_recover_button_clicked), this);

//     /**
//      * Inicialmente deshabilitado hasta que se seleccione una opción de escaneo
//      * habilitar el botón de recuperación.
//      */
//     gtk_widget_set_sensitive(recover_button, FALSE);
// }

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

    g_signal_connect(toggle_renderer, "toggled", G_CALLBACK(on_toggle_checkbox), store);

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
    gtk_widget_set_sensitive(recover_selected_button, FALSE);
    gtk_box_pack_start(GTK_BOX(recovery_box), recover_selected_button, FALSE, FALSE, 0);

    
    // g_signal_connect(recover_selected_button, "clicked",
    //                  G_CALLBACK(on_recover_selected_button_clicked), this);
}

// void RecoveryPanel::on_scan_option_toggled(GtkWidget *widget, gpointer data)
// {
//     RecoveryPanel *panel = static_cast<RecoveryPanel *>(data);

//     if (widget == panel->quick_scan_check &&
//         gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(panel->quick_scan_check)))
//     {
//         gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(panel->deep_scan_check), FALSE);
//         gtk_widget_set_sensitive(panel->recover_button, TRUE);
//     }
//     else if (widget == panel->deep_scan_check &&
//              gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(panel->deep_scan_check)))
//     {
//         gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(panel->quick_scan_check), FALSE);
//         gtk_widget_set_sensitive(panel->recover_button, TRUE);
//     }
//     else
//     {
//         gtk_widget_set_sensitive(panel->recover_button, FALSE);
//     }
// }

/**
 * Callback para el cambio de estado del checkbox en la vista de archivos
 */
void RecoveryPanel::on_toggle_checkbox(GtkCellRendererToggle *cell,
                                       gchar *path_str,
                                       gpointer user_data)
{
    GtkTreeView *view = GTK_TREE_VIEW(user_data);
    GtkTreeModel *model = gtk_tree_view_get_model(view);
    if (!model) return;

    GtkTreePath *path = gtk_tree_path_new_from_string(path_str);
    if (!path) return;

    GtkTreeIter iter;
    if (gtk_tree_model_get_iter(model, &iter, path)) {
        gboolean active = FALSE;
        gtk_tree_model_get(model, &iter, COLUMN_CHECK, &active, -1);

        if (GTK_IS_LIST_STORE(model))
            gtk_list_store_set(GTK_LIST_STORE(model), &iter, COLUMN_CHECK, !active, -1);
    }

    gtk_tree_path_free(path);
}
