#include "../../include/ui/HomePanel.h"
#include <iostream>
using namespace std;

HomePanel::HomePanel() : panel(nullptr)
{
    panel = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_margin_start(panel, 20);
    gtk_widget_set_margin_end(panel, 20);
    gtk_widget_set_margin_top(panel, 20);
    gtk_widget_set_margin_bottom(panel, 20);

    GtkWidget *title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title), "<span size='x-large' weight='bold'>Inicio</span>");
    gtk_box_pack_start(GTK_BOX(panel), title, FALSE, FALSE, 10);

    // cargar imagen
    GError *error = nullptr;
    gchar *current_dir = g_get_current_dir();
    gchar *image_path = g_build_filename(current_dir, "resources", "icon", "icons8-mantenimiento-100.png", nullptr);
    g_free(current_dir);

    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(image_path, &error);
    g_free(image_path);

    if (error) {
        g_warning("Error al cargar la imagen de marca de agua: %s", error->message);
        g_error_free(error);
    } else {
        int original_width = gdk_pixbuf_get_width(pixbuf);
        int original_height = gdk_pixbuf_get_height(pixbuf);
        GdkPixbuf *scaled_pixbuf = gdk_pixbuf_scale_simple(pixbuf,
                                                           original_width * 2,
                                                           original_height * 2,
                                                           GDK_INTERP_BILINEAR);
        g_object_unref(pixbuf);

        GtkWidget *image = gtk_image_new_from_pixbuf(scaled_pixbuf);
        g_object_unref(scaled_pixbuf);
        gtk_box_pack_start(GTK_BOX(panel), image, TRUE, TRUE, 0);
    }


    
}

HomePanel::~HomePanel()
{

}


/**
 * Obtiene el panel de recuperación.
 */
GtkWidget *HomePanel::get_panel()
{
    return panel;
}
