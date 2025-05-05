#include "../../include/ui/recoveryPanel.h"
#include <iostream>

RecoveryPanel::RecoveryPanel() : 
    panel(nullptr),
    device_combo(nullptr),
    quick_scan_check(nullptr),
    deep_scan_check(nullptr),
    progress_bar(nullptr),
    tree_view(nullptr) {

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
    create_options_section();
    create_progress_section();
    create_files_section();
}

RecoveryPanel::~RecoveryPanel() {
    
}

void RecoveryPanel::create_device_selection_section() {
    // Grupo de selección de dispositivo
    GtkWidget *device_frame = gtk_frame_new("Seleccionar dispositivo");
    gtk_box_pack_start(GTK_BOX(panel), device_frame, FALSE, FALSE, 10);
    
    GtkWidget *device_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(device_frame), device_box);
    gtk_widget_set_margin_start(device_box, 10);
    gtk_widget_set_margin_end(device_box, 10);
    gtk_widget_set_margin_top(device_box, 10);
    gtk_widget_set_margin_bottom(device_box, 10);
    
    // Combo box para seleccionar dispositivo
    // simulacion de dispositivos
    // En una aplicación real, aquí se cargarían los dispositivos disponibles
    // y se añadirían al combo box.
    // Por simplicidad, se añaden algunos dispositivos de ejemplo.
    device_combo = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(device_combo), "/dev/sda1");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(device_combo), "/dev/sdb1");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(device_combo), "/dev/sdc1");
    gtk_combo_box_set_active(GTK_COMBO_BOX(device_combo), 0);
    gtk_box_pack_start(GTK_BOX(device_box), device_combo, FALSE, FALSE, 0);
}

void RecoveryPanel::create_options_section() {
    GtkWidget *options_frame = gtk_frame_new("Opciones de recuperación");
    gtk_box_pack_start(GTK_BOX(panel), options_frame, FALSE, FALSE, 10);
    
    GtkWidget *options_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(options_frame), options_box);
    gtk_widget_set_margin_start(options_box, 10);
    gtk_widget_set_margin_end(options_box, 10);
    gtk_widget_set_margin_top(options_box, 10);
    gtk_widget_set_margin_bottom(options_box, 10);

    quick_scan_check = gtk_check_button_new_with_label("Escaneo rápido");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(quick_scan_check), TRUE);
    gtk_box_pack_start(GTK_BOX(options_box), quick_scan_check, FALSE, FALSE, 0);
    g_signal_connect(quick_scan_check, "toggled", G_CALLBACK(on_scan_option_toggled), this);
    
    deep_scan_check = gtk_check_button_new_with_label("Escaneo profundo");
    gtk_box_pack_start(GTK_BOX(options_box), deep_scan_check, FALSE, FALSE, 0);
    g_signal_connect(deep_scan_check, "toggled", G_CALLBACK(on_scan_option_toggled), this);
    
    GtkWidget *recover_button = gtk_button_new_with_label("Iniciar Recuperación");
    gtk_widget_set_margin_top(recover_button, 10);
    gtk_box_pack_start(GTK_BOX(panel), recover_button, FALSE, FALSE, 0);
    g_signal_connect(recover_button, "clicked", G_CALLBACK(on_recover_button_clicked), this);
}

void RecoveryPanel::create_progress_section() {
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

void RecoveryPanel::create_files_section() {
    // Lista de archivos encontrados
    GtkWidget *files_frame = gtk_frame_new("Archivos encontrados");
    gtk_box_pack_start(GTK_BOX(panel), files_frame, TRUE, TRUE, 10);
    
    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(files_frame), scroll);
    
    // Crear el tree view para mostrar archivos
    tree_view = gtk_tree_view_new();
    gtk_container_add(GTK_CONTAINER(scroll), tree_view);
    
    // Crear las columnas del tree view
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
    GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes("Nombre", renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);
    
    column = gtk_tree_view_column_new_with_attributes("Tamaño", renderer, "text", 1, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);
    
    column = gtk_tree_view_column_new_with_attributes("Fecha", renderer, "text", 2, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);
    
    // Añadir datos de ejemplo
    populate_sample_data();
}

//Simulación de datos de ejemplo
// En una aplicación real, aquí se cargarían los datos reales de los archivos recuperados
// y se añadirían al modelo del tree view.
// En este caso, se añaden datos de ejemplo para mostrar cómo funcionaría la interfaz.
void RecoveryPanel::populate_sample_data() {
    // Crear un modelo para el tree view
    GtkListStore *store = gtk_list_store_new(3, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
    gtk_tree_view_set_model(GTK_TREE_VIEW(tree_view), GTK_TREE_MODEL(store));
    
    // Añadir algunos datos de ejemplo
    GtkTreeIter iter;
    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter, 0, "documento.pdf", 1, "1.2 MB", 2, "01/05/2025", -1);
    
    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter, 0, "imagen.jpg", 1, "850 KB", 2, "28/04/2025", -1);
    
    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter, 0, "archivo.txt", 1, "15 KB", 2, "30/04/2025", -1);
    
    // Liberar el modelo
    g_object_unref(store);
}

GtkWidget* RecoveryPanel::get_panel() {
    return panel;
}


void RecoveryPanel::on_recover_button_clicked(GtkWidget *widget, gpointer data) {
    RecoveryPanel *panel = static_cast<RecoveryPanel*>(data);
    
    // Obtener el dispositivo seleccionado
    gchar *device = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(panel->device_combo));
    
    // Simular inicio de recuperación (en una aplicación real, aquí comenzaría el proceso)
    std::cout << "Iniciando recuperación en " << device << std::endl;
    
    // Actualizar la barra de progreso (simulación)
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(panel->progress_bar), 0.25);
    gtk_progress_bar_set_text(GTK_PROGRESS_BAR(panel->progress_bar), "25%");
    
    g_free(device);
}

void RecoveryPanel::on_scan_option_toggled(GtkWidget *widget, gpointer data) {
    RecoveryPanel *panel = static_cast<RecoveryPanel*>(data);

    if (widget == panel->quick_scan_check && 
        gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(panel->quick_scan_check))) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(panel->deep_scan_check), FALSE);
    } else if (widget == panel->deep_scan_check && 
               gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(panel->deep_scan_check))) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(panel->quick_scan_check), FALSE);
    }
}