#pragma once

#include <string>
#include <vector>
#include <gtk/gtk.h>

class UIHelpers {
public:
    static void set_text_view_content(GtkTextView* text_view, const char* content);
    static std::string get_text_view_content(GtkTextView* text_view);
    static void show_message_dialog(GtkWindow* parent, const char* message, GtkMessageType type);
    static void set_progress_bar_fraction(GtkProgressBar* progress_bar, double fraction);
    static void set_label_text(GtkLabel* label, const char* text);
};