/*
  glade.h

  License CC0 PUBLIC DOMAIN

  To the extent possible under law, Mark J. Olesen has waived all copyright
  and related or neighboring rights to tabitha. This work is published from:
  United States.
*/
#if !defined(__glade_h__)

#include <gtk/gtk.h>
#include <glib.h>

extern GtkBuilder*                      g_builder;
extern gchar*                           g_builder_basedir;
extern gchar const*                     g_glade_filename;
extern gchar*                           g_glade_path;
extern gchar*                           g_module_basedir;
extern gchar*                           g_module_filename;

extern gint
glade_init(
  GError**                              o_error,
  int                                   argc,
  char*                                 argv[]);

extern void
glade_deinit();

#define __glade_h__
#endif
/* 
vim:expandtab:softtabstop=2:tabstop=2:shiftwidth=2:nowrap:ruler 
*/
