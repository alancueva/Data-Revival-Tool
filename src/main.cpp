#include <gtk/gtk.h>
#include <iostream>
#include "../include/ui/MetadataRecoveryGUI.h"
using namespace std;

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
    catch (const exception &e)
    {
        cerr << "Error crítico: " << e.what() << endl;
        return 1;
    }

    return 0;
}