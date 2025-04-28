#include "../../include/ui/MetadataRecoveryGUI.h"
#include "../../include/ui/UIHelpers.h"
#include <thread>
#include <chrono>

MetadataRecoveryGUI::MetadataRecoveryGUI()
{

    window = nullptr;
    file_entry = nullptr;
    text_view = nullptr;
    progress_bar = nullptr;
    status_label = nullptr;
    file_info_label = nullptr;
    scan_combo = nullptr;
    basic_scan_button = nullptr;
    recover_button = nullptr;
    deep_scan_button = nullptr;
}

MetadataRecoveryGUI::~MetadataRecoveryGUI()
{

    gtk_widget_destroy(window);
    gtk_main_quit();
}

/// Configurar la interfaz de usuario
// Sección de selección de archivo
void MetadataRecoveryGUI::create()
{
    
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Recuperador de Metadatos v1.0 - Open Source");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Layout principal
    GtkWidget *main_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(window), main_vbox);

    // Configurar las diferentes secciones de la interfaz
    setup_file_selection(main_vbox);
    setup_analysis_options(main_vbox);
    setup_results_area(main_vbox);

    UIHelpers::set_text_view_content(GTK_TEXT_VIEW(text_view),
                                    "Bienvenido al Software de Recuperación de Metadatos\n\n"
                                    "Este programa permite analizar archivos y recuperar metadatos que han sido sobrescritos o eliminados.\n\n"
                                    "Instrucciones:\n"
                                    "1. Haga clic en 'Explorar...' para seleccionar un archivo.\n"
                                    "2. Seleccione el tipo de análisis deseado.\n"
                                    "3. Use los botones para realizar diferentes tipos de escaneo.\n\n"
                                    "Compatible con imágenes (JPG, PNG, TIFF...) y documentos (PDF, DOC, DOCX...).\n\n"
                                    "Software libre y de código abierto (GNU GPL v3).");
}

void MetadataRecoveryGUI::show()
{
    gtk_widget_show_all(window);
}
