/*
  smtp_client.h

  License CC0 PUBLIC DOMAIN

  To the extent possible under law, Mark J. Olesen has waived all copyright
  and related or neighboring rights to tabitha. This work is published from:
  United States.
*/
#if !defined(__smtp_client_h__)

#include <glib-object.h>

G_BEGIN_DECLS

#define SMTP_TYPE_CLIENT smtp_client_get_type()

G_DECLARE_FINAL_TYPE(SmtpClient, smtp_client, SMTP, CLIENT, GObject)

#define smtp_client_error_domain smtp_client_error_domain_quark()

enum smtp_client_error_code
{
  smtp_client_protocol_unsupported,
  smtp_client_server_did_not_respond,
  smtp_client_server_not_ready,
  smtp_client_unable_to_connect,
  smtp_client_unable_to_get_hostname,
  smtp_client_unable_to_read,
  smtp_client_unable_to_select_socket,
  smtp_client_unable_to_write
};

extern GQuark
smtp_error_domain_quark();

extern SmtpClient*
smtp_client_new();

#define smtp_client_new() g_object_new(SMTP_TYPE_CLIENT, 0)

extern void
smtp_client_add_attachment(
  SmtpClient*const                      io_smtp,
  gchar const*                          i_path,
  gchar const*                          i_mimetype);

extern void
smtp_client_add_to(
  SmtpClient*const                      io_smtp,
  gchar const*                          i_email);

extern int
smtp_client_send(
  GError**                              o_error,
  SmtpClient const*const                i_smtp);

extern void
smtp_client_set_body(
  SmtpClient*const                      io_smtp,
  gchar const*                          i_message);

extern void
smtp_client_set_from(
  SmtpClient*const                      io_smtp,
  gchar const*                          i_email);

extern void
smtp_client_set_server(
  SmtpClient*const                      io_smtp,
  gchar const*                          i_username,
  gchar const*                          i_password,
  gchar const*                          i_server,
  gchar const*                          i_port);

extern void
smtp_client_set_subject(
  SmtpClient*const                      io_smtp,
  gchar const*                          i_subject);

G_END_DECLS

#define __smtp_client_h__
#endif
/* 
vim:expandtab:softtabstop=2:tabstop=2:shiftwidth=2:nowrap:ruler 
*/
