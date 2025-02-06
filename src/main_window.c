
#include <gtk/gtk.h>


static void
app_activate (GApplication *app, gpointer *user_data)
{
    const char *WIN_TITLE = "Northstar Advantage Emulator";
    const int WIN_WIDTH   = 400;
    const int WIN_HEIGHT  = 300;

    GtkWidget *win = NULL;

    win = gtk_application_window_new (GTK_APPLICATION (app));
    gtk_window_set_title (GTK_WINDOW (win), WIN_TITLE);
    gtk_window_set_default_size (GTK_WINDOW (win), WIN_WIDTH, WIN_HEIGHT);

    gtk_window_present (GTK_WINDOW (win));
}


int
main (int argc, char **argv)
{
    const char *APPLICATION_ID = "org.sageetcher.NorthstarAdvantageEmulator";

    int status = 0;
    GtkApplication *app = NULL;

    app = gtk_application_new (APPLICATION_ID, G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
    status = g_application_run (G_APPLICATION (app), argc, argv); 

    g_object_unref (app);
    return status;
}





