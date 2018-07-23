/*
  email.h

  License CC0 PUBLIC DOMAIN

  To the extent possible under law, Mark J. Olesen has waived all copyright
  and related or neighboring rights to tabitha. This work is published from:
  United States.
*/
#if !defined(__email_h__)

#include "session.h"

struct email
{
  gchar*                                m_from;
  gchar*                                m_to;
  gchar*                                m_subject;
  gchar*                                m_message;
  gchar*                                m_server;
  gchar*                                m_username;
  gchar*                                m_password;
  gchar*                                m_port;
};

extern void
email_assign(
  struct email*const                    o_email);

extern void
email_discharge(
  struct email*const                    io_email);

extern int
email_form(
  struct email*const                    io_object,
  struct session*const                  io_session,
  GtkWindow*const                       io_parent,
  GtkBuilder*const                      io_builder);

extern int
email_load(
  GError**                              o_error,
  struct email*const                    io_email,
  struct session*const                  io_session,
  gchar const                           i_sales_id[size_pg_big_int]);

extern void
email_trim(
  struct email*const                    io_email);

#define __email_h__
#endif
/* 
vim:expandtab:softtabstop=2:tabstop=2:shiftwidth=2:nowrap:ruler 
*/
