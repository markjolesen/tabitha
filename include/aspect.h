/*
  aspect.h

  License CC0 PUBLIC DOMAIN

  To the extent possible under law, Mark J. Olesen has waived all copyright
  and related or neighboring rights to tabitha. This work is published from:
  United States.
*/
#if !defined(__aspect_h__)

#include <gtk/gtk.h>
#include <glib.h>

struct aspect
{
  gchar                                 m_dbname[63+1];
  gchar                                 m_hostname[63+1]; /* HOST_NAME_MAX or _POSIX_HOST_NAME_MAX */
  gchar                                 m_ipaddress[47+1]; /* INET6_ADDRSTRLEN  */
  gchar                                 m_port[7+1]; /* 1...65535 */
  gchar                                 m_username[31+1];
  gchar                                 m_password[31+1];
};

extern void
aspect_set_defaults(
  struct aspect*const                   o_aspect);

extern int
aspect_form(
  struct aspect*const                   io_object,
  GtkWindow*const                       io_parent,
  GtkBuilder*const                      io_builder);

#define __aspect_h__
#endif
/* 
vim:expandtab:softtabstop=2:tabstop=2:shiftwidth=2:nowrap:ruler 
*/
