/*
  main.c

  License CC0 PUBLIC DOMAIN

  To the extent possible under law, Mark J. Olesen has waived all copyright
  and related or neighboring rights to tabitha. This work is published from:
  United States.
*/
#include "glade.h"
#include "error.h"
#include "aspect.h"
#include "session.h"

GtkApplicationWindow*                   g_window_main= 0;
struct aspect                           g_aspect;
struct session                          g_session;

G_MODULE_EXPORT void
on_window_main_destroy(
  G_GNUC_UNUSED GtkWidget*              io_object,
  G_GNUC_UNUSED gpointer                io_user_data)
{
  gtk_main_quit();
}

static int 
connect(
  struct session*const                  io_session,
  struct aspect*const                   io_aspect,
  GtkWindow*                            io_parent,
  GtkBuilder*                           io_builder)
{
  int                                   l_exit;
  GError*                               l_error;

  l_error= 0;

  do
  {

    l_exit= aspect_form(io_aspect, io_parent, io_builder);

    if (l_exit)
    {
      break;
    }

    l_exit= session_connect(&l_error, io_session, io_aspect);

    if (0 == l_exit)
    {
      break;
    }

    _error_display(io_parent, l_error);
    g_clear_error(&l_error);

  }while(1);

  return l_exit;
}

int main(
  int                                   argc,
  char*                                 argv[])
{
  int                                   l_exit;
  GError*                               l_error;
  GtkSettings*                          l_settings;

  l_exit= -1;
  l_error= 0;

  aspect_set_defaults(&g_aspect);
  session_assign(&g_session);

  do
  {

    gtk_init(&argc, &argv); /* on error aborts */

    l_settings= gtk_settings_get_default();
    g_object_set(l_settings, "gtk-button-images", TRUE, 0);
    g_object_unref(l_settings);

    l_exit= glade_init(&l_error, argc, argv);

    if (l_exit)
    {
      break;
    }

    g_window_main= GTK_APPLICATION_WINDOW(gtk_builder_get_object(g_builder, "window_main"));
    g_object_set_data(G_OBJECT(g_window_main), "builder", g_builder);
    g_object_set_data(G_OBJECT(g_window_main), "session", &g_session);

    gtk_window_set_icon_from_file(g_window_main, "tabitha-48x48.png", 0);

    gtk_widget_show(GTK_WIDGET(g_window_main));

    l_exit= connect(&g_session, &g_aspect, GTK_WINDOW(g_window_main), g_builder);

    if (l_exit)
    {
      break;
    }

    gtk_main();

  }while(0);

  if (l_error)
  {
    _error_display(GTK_WINDOW(g_window_main), l_error);
    g_clear_error(&l_error);
  }

  session_discharge(&g_session);
  glade_deinit();

  return l_exit;
}
/* 
vim:expandtab:softtabstop=2:tabstop=2:shiftwidth=2:nowrap:ruler 
*/
