#include <gtk/gtk.h>
#include "../include/ui/MetadataRecoveryGUI.h"

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);

    MetadataRecoveryGUI gui;
    gui.create();
    gui.show();

    gtk_main();

    return 0;
}