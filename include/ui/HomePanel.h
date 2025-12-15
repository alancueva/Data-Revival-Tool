#ifndef HOMEPANEL_H
#define HOMEPANEL_H

#include <gtk/gtk.h>
#include <string>

class HomePanel
{
private:
    GtkWidget *panel;

public:
    HomePanel();
    ~HomePanel();
    GtkWidget *get_panel();
};

#endif