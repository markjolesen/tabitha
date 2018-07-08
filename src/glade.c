/*
  glade.c

  License CC0 PUBLIC DOMAIN

  To the extent possible under law, Mark J. Olesen has waived all copyright
  and related or neighboring rights to tabitha. This work is published from:
  United States.
*/
#include "glade.h"
#include "error.h"

GtkBuilder*                             g_builder= 0;
gchar*                                  g_builder_basedir= 0;
gchar const*                            g_glade_filename= "tabitha.glade";
gchar*                                  g_glade_path= 0;
gchar*                                  g_module_basedir= 0;
gchar*                                  g_module_filename= 0;

gint
glade_init(
  GError**                              o_error,
  int                                   argc,
  char*                                 argv[])
{
  char const*const*                     l_env;
  GError*                               l_error;
  gint                                  l_exit;
  gint                                  l_rc;
  size_t                                l_slot;

  l_error= 0;
  l_exit= 0;
  g_module_filename= g_strdup(argv[0]);
  g_module_basedir= g_path_get_dirname(g_module_filename);

  do
  {

    g_glade_path= g_build_filename(
      g_module_basedir,
      g_glade_filename,
      (char*)0);

    l_rc= g_file_test(g_glade_path, G_FILE_TEST_IS_REGULAR);

    if (l_rc)
    {
      break;
    }

    g_free(g_glade_path);
    g_glade_path= 0;

    l_env= g_get_system_data_dirs();
    l_slot= 0;

    do
    {

      if (0 == l_env[l_slot])
      {
        l_exit= -1;
        break;
      }

      g_glade_path= g_build_filename(
        l_env[l_slot],
        "tabitha",
        "ui",
        g_glade_filename,
        (char*)0);

      l_rc= g_file_test(g_glade_path, G_FILE_TEST_IS_REGULAR);

      if (l_rc)
      {
        break;
      }

      g_free(g_glade_path);
      g_glade_path= 0;

      l_slot++;

    }while(1);

  }while(0);

  if (0 == l_exit)
  {
    g_builder= gtk_builder_new();
    l_rc= gtk_builder_add_from_file(g_builder, g_glade_path, &l_error);
    if (0 == l_rc)
    {
      l_exit= -1;
    }
    gtk_builder_connect_signals(g_builder, 0);
  }
  else if (0 == l_error)
  {
      l_error= g_error_new(
        domain_glade,
        error_glade_file_not_found,
        "Unable to find glade file: '%s'",
        g_glade_filename);
  }

  if (l_error)
  {
    g_propagate_error(o_error, l_error);
  }

  return l_exit;
}

void
glade_deinit()
{

  if (g_builder)
  {
    g_object_unref(g_builder);
  }

  g_free(g_module_filename);
  g_free(g_module_basedir);
  g_free(g_builder_basedir);
  g_free(g_glade_path);

  return;
}
/* 
vim:expandtab:softtabstop=2:tabstop=2:shiftwidth=2:nowrap:ruler
*/
