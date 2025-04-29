#include <gtk/gtk.h>
#include <iostream>
#include "../include/ui/MetadataRecoveryGUI.h"

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);
        
    try
    {
        MetadataRecoveryGUI gui;
        gui.crear();
        gui.mostrar();
        gtk_main();
    
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error crítico: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}