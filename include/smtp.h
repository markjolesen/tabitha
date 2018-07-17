/*
  smtp.h

  License CC0 PUBLIC DOMAIN

  To the extent possible under law, Mark J. Olesen has waived all copyright
  and related or neighboring rights to tabitha. This work is published from:
  United States.
*/
#if !defined(__smtp_h__)

#include <glib.h>

enum
{
  smtp_size_address                     = (41+1),
  smtp_size_port                        = (5+1),
  smtp_size_subject                     = (128+1),
};

#define smtp_error_domain smtp_error_domain_quark()

enum smtp_error_code
{
  error_smtp_protocol_unsupported,
  error_smtp_server_did_not_respond,
  error_smtp_server_not_ready,
  error_smtp_unable_to_connect,
  error_smtp_unable_to_get_hostname,
  error_smtp_unable_to_read,
  error_smtp_unable_to_select_socket,
  error_smtp_unable_to_write
};

struct smtp
{
  gchar                                 m_username[smtp_size_address];
  gchar                                 m_password[smtp_size_address];
  gchar                                 m_server[smtp_size_address];
  gchar                                 m_port[smtp_size_port];
  gchar                                 m_from[smtp_size_address];
  gchar                                 m_subject[smtp_size_subject];
  GArray*                               m_to;
  gchar*                                m_body;
  GArray*                               m_attachment;
};

extern GQuark
smtp_error_domain_quark();

extern void
smtp_add_pdf_attachment(
  struct smtp*const                     io_smtp,
  gchar const*                          i_path);

extern void
smtp_add_to(
  struct smtp*const                     io_smtp,
  gchar const*                          i_to);

extern void
smtp_assign(
  struct smtp*const                     o_smtp);

extern void
smtp_discharge(
  struct smtp*const                     io_smtp);

extern void
smtp_set_body(
  struct smtp*const                     io_smtp,
  gchar const*                          i_body);

extern void
smtp_set_from(
  struct smtp*const                     io_smtp,
  gchar const*                          i_from);

extern void
smtp_set_server(
  struct smtp*const                     io_smtp,
  gchar const*                          i_username,
  gchar const*                          i_password,
  gchar const*                          i_server,
  gchar const*                          i_port);

extern int
smtp_send(
  GError**                              o_error,
  struct smtp const*const               i_smtp);

extern void
smtp_set_subject(
  struct smtp*const                     io_smtp,
  gchar const*                          i_subject);

#define __smtp_h__
#endif
/* 
vim:expandtab:softtabstop=2:tabstop=2:shiftwidth=2:nowrap:ruler 
*/
