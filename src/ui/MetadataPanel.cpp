
#include "../../include/ui/metadataPanel.h"
#include "../../include/metadata/FileMetadata.h"
#include "../../include/core/MetadataRecoveryEngine.h"
#include <string>
#include <iostream>
using namespace std;

MetadataPanel::MetadataPanel() : file_frame(nullptr),
                                 file_grid(nullptr), 
                                 file_button(nullptr),
                                 file_label(nullptr),
                                 file_entry(nullptr),
                                 file_chooser(nullptr),
                                 image_filter(nullptr),
                                 all_filter(nullptr),
                                 doc_filter(nullptr),
                                 panel(nullptr),
                                 text_view(nullptr),
                                 engine(new MetadataRecoveryEngine())
{
    panel = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_margin_start(panel, 20);
    gtk_widget_set_margin_end(panel, 20);
    gtk_widget_set_margin_top(panel, 20);
    gtk_widget_set_margin_bottom(panel, 20);

    GtkWidget *title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title), "<span size='x-large' weight='bold'>Búsqueda de Metadatos</span>");
    gtk_box_pack_start(GTK_BOX(panel), title, FALSE, FALSE, 10);

    // Crear secciones del panel
    create_file_selection_section();
    create_results_section();
}


MetadataPanel::~MetadataPanel()
{
    delete engine;
    gtk_widget_destroy(panel);
}

void MetadataPanel::create_file_selection_section()
{
    // Grupo de selección de archivo
    GtkWidget *file_frame = gtk_frame_new("Seleccionar archivo");
    gtk_box_pack_start(GTK_BOX(panel), file_frame, FALSE, FALSE, 10);

    GtkWidget *file_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_container_add(GTK_CONTAINER(file_frame), file_box);
    gtk_widget_set_margin_start(file_box, 10);
    gtk_widget_set_margin_end(file_box, 10);
    gtk_widget_set_margin_top(file_box, 10);
    gtk_widget_set_margin_bottom(file_box, 10);

    file_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(file_entry), "Ruta del archivo...");
    gtk_editable_set_editable(GTK_EDITABLE(file_entry), FALSE);
    gtk_widget_set_sensitive(file_entry, FALSE); // Deshabilita completamente el entry
    gtk_widget_set_can_focus(file_entry, FALSE); // No permite focus
    gtk_box_pack_start(GTK_BOX(file_box), file_entry, TRUE, TRUE, 0);

    // Botón para examinar archivos
    GtkWidget *browse_button = gtk_button_new_with_label("Examinar");
    gtk_box_pack_start(GTK_BOX(file_box), browse_button, FALSE, FALSE, 0);
    g_signal_connect(browse_button, "clicked", G_CALLBACK(on_browse_button_clicked), this);

    // Contenedor horizontal para los botones de analizar y eliminar
    GtkWidget *button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

    // Botón de analizar
    GtkWidget *analyze_button = gtk_button_new_with_label("Analizar Metadatos");
    gtk_widget_set_margin_top(analyze_button, 10);
    gtk_box_pack_start(GTK_BOX(button_box), analyze_button, FALSE, FALSE, 0);
    g_signal_connect(analyze_button, "clicked", G_CALLBACK(on_analyze_button_clicked), this);

    // Botón de eliminar
    GtkWidget *clear_button = gtk_button_new_with_label("Eliminar");
    gtk_widget_set_margin_top(clear_button, 10);
    gtk_box_pack_start(GTK_BOX(button_box), clear_button, FALSE, FALSE, 0);

    // Conectar señal para limpiar file_entry y text_view
    g_signal_connect_swapped(clear_button, "clicked", G_CALLBACK(+[](MetadataPanel* self){
        gtk_entry_set_text(GTK_ENTRY(self->file_entry), "");
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(self->text_view));
        gtk_text_buffer_set_text(buffer, "", -1);
    }), this);

    // Agregar el contenedor de botones al panel
    gtk_box_pack_start(GTK_BOX(panel), button_box, FALSE, FALSE, 0);

    // Habilitar/deshabilitar el botón según el contenido de file_entry
    g_signal_connect(file_entry, "changed", G_CALLBACK(+[](GtkEditable *editable, gpointer user_data) {
        GtkWidget *button = GTK_WIDGET(user_data);
        const gchar *text = gtk_entry_get_text(GTK_ENTRY(editable));
        gtk_widget_set_sensitive(button, (text && *text));
    }), analyze_button );

    // g_signal_connect(file_entry, "changed", G_CALLBACK(+[](GtkEditable *editable, gpointer user_data) {
    //     GtkWidget *button = GTK_WIDGET(user_data);
    //     const gchar *text = gtk_entry_get_text(GTK_ENTRY(editable));
    //     gtk_widget_set_sensitive(button, (text && *text));
    // }), clear_button );

    // Inicialmente deshabilitado si no hay texto
    const gchar *init_text = gtk_entry_get_text(GTK_ENTRY(file_entry));
    gtk_widget_set_sensitive(analyze_button, (init_text && *init_text));
}

void MetadataPanel::create_results_section()
{
    
    GtkWidget *results_frame = gtk_frame_new("Resultados ");
    gtk_box_pack_start(GTK_BOX(panel), results_frame, TRUE, TRUE, 10);

    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(results_frame), scroll);

    text_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_WORD);
    gtk_container_add(GTK_CONTAINER(scroll), text_view);
}

/**
 * @brief Obtiene el panel de metadatos
 * @return GtkWidget*
 */
GtkWidget *MetadataPanel::get_panel()
{
    return panel;
}


void MetadataPanel::on_browse_button_clicked(GtkWidget *widget, gpointer data)
{
    MetadataPanel *panel = static_cast<MetadataPanel *>(data);

    GtkWidget *dialog = gtk_file_chooser_dialog_new("Seleccionar archivo",
                                                    NULL,
                                                    GTK_FILE_CHOOSER_ACTION_OPEN,
                                                    "_Cancelar", GTK_RESPONSE_CANCEL,
                                                    "_Abrir", GTK_RESPONSE_ACCEPT,
                                                    NULL);

    gint result = gtk_dialog_run(GTK_DIALOG(dialog));
    if (result == GTK_RESPONSE_ACCEPT)
    {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        gtk_entry_set_text(GTK_ENTRY(panel->file_entry), filename);
        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}

void MetadataPanel::on_analyze_button_clicked(GtkWidget *widget, gpointer data)
{
    MetadataPanel *panel = static_cast<MetadataPanel *>(data);
    const gchar* filepath = gtk_entry_get_text(GTK_ENTRY(panel->file_entry));
    string tipo =  panel->engine->recoverMetadata(filepath);
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(panel->text_view));
    gtk_text_buffer_set_text(buffer, "", -1); // Limpiar el buffer antes de agregar nuevo texto
    // Limpiar el buffer antes de agregar nuevo texto
    

    if (strlen(filepath) > 0)
    {
        gchar *metadata = g_strdup_printf(
            "%s: %s",
            tipo.c_str(),
            "");

        gtk_text_buffer_set_text(buffer, metadata, -1);
        g_free(metadata);
    }
    else
    {
        gtk_text_buffer_set_text(buffer, "Por favor, seleccione un archivo primero.", -1);
    }
}
