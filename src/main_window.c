
#include <gtk/gtk.h>


static void
app_activate (GApplication *app, gpointer *user_data)
{
    g_print ("GtkApplication is activated.\n");
}


int
main (int argc, char **argv)
{
    int status = 0;
    GtkApplication *app = NULL;

    app = gtk_application_new ("org.sageetcher.NorthStarAdvantageEmulator", 
                               G_APPLICATION_DEFAULT_FLAGS);

    g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);
    status = g_application_run (G_APPLICATION (app), argc, argv); 

    g_object_unref (app);
    return status;
}





