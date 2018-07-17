/*
  smtp.c

  License CC0 PUBLIC DOMAIN

  To the extent possible under law, Mark J. Olesen has waived all copyright
  and related or neighboring rights to tabitha. This work is published from:
  United States.
*/
#include "smtp.h"
#include "error.h"
#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <sys/select.h>

enum
{
  smtp_size_boundary                    = (28+1),
  smtp_size_line                        = 998
};

enum authtype
{
  auth_unknown                          = 0,
  auth_plain                            = (1 << 0),
  auth_login                            = (1 << 1),
  auth_gssapi                           = (1 << 2),
  auth_digest_md5                       = (1 << 3),
  auth_md5                              = (1 << 4),
  auth_cram_md5                         = (1 << 5)
};

enum filetype
{
  file_unknown                          = 0,
  file_pdf
};

struct smtp_address
{
  gchar                                 m_address[smtp_size_address];
};

struct smtp_file
{
  enum filetype                         m_type;
  gchar*                                m_path;
};

G_DEFINE_QUARK(smtp-error-domain_quark, smtp_error_domain)

extern void
ssl_init_once()
{
  static gboolean                       l_been_here= FALSE;

  if (FALSE == l_been_here)
  {
    SSL_library_init();
    SSL_load_error_strings();
    ERR_load_BIO_strings();
    OpenSSL_add_all_algorithms();
    l_been_here= TRUE;
  }

  return;
}

static void
smtp_generate_boundary(
  gchar                                 o_boundary[smtp_size_boundary])
{
  static gchar const                    l_set[36]=
    {"1234567890abcdefghijklmnopqrstuvwxyz"};
  gint32                                l_index;
  GRand*                                l_rand;
  guint                                 l_slot;

  l_rand= g_rand_new();

  for (l_slot= 0; 12 > l_slot; l_slot++)
  {
    o_boundary[l_slot]= '0';
  }

  for (; (smtp_size_boundary - 1) > l_slot; l_slot++)
  {
    l_index= g_rand_int_range(l_rand, 0, (sizeof(l_set) - 1));
    o_boundary[l_slot]= l_set[l_index];
  }

  o_boundary[l_slot]= 0;

  g_rand_free(l_rand);

  return;
}

extern void
smtp_add_pdf_attachment(
  struct smtp*const                     io_smtp,
  gchar const*                          i_path)
{
  struct smtp_file*                     l_file;

  l_file= g_malloc0(sizeof(*l_file));
  (*l_file).m_type= file_pdf;
  (*l_file).m_path= g_strdup(i_path);
  g_array_append_val((*io_smtp).m_attachment, (*l_file));
  g_free(l_file);

  return;
}

extern void
smtp_add_to(
  struct smtp*const                     io_smtp,
  gchar const*                          i_to)
{
  struct smtp_address*                  l_to;

  l_to= g_malloc0(sizeof(*l_to));
  g_strlcpy((*l_to).m_address, i_to, smtp_size_address);
  g_array_append_val((*io_smtp).m_to, (*l_to));
  g_free(l_to);

  return;
}

extern void
smtp_assign(
  struct smtp*const                     o_smtp)
{

  memset(o_smtp, 0, sizeof(*o_smtp));
  (*o_smtp).m_to= g_array_new(FALSE, TRUE, sizeof(struct smtp_address));
  (*o_smtp).m_attachment= g_array_new(FALSE, TRUE, sizeof(struct smtp_file));

  return;
}

extern void
smtp_discharge(
  struct smtp*const                     io_smtp)
{
  struct smtp_file*                     l_file;
  guint                                 l_slot;

  if ((*io_smtp).m_to)
  {
    g_array_free((*io_smtp).m_to, TRUE);
  }

  if ((*io_smtp).m_attachment)
  {
    l_file= (struct smtp_file*)(*io_smtp).m_attachment->data;
    for (l_slot= 0; (*io_smtp).m_attachment->len > l_slot; l_slot++)
    {
      g_free((*l_file).m_path);
      l_file++;
    }
    g_array_free((*io_smtp).m_attachment, TRUE);
  }

  g_free((*io_smtp).m_body);
  memset(io_smtp, 0, sizeof(*io_smtp));

  return;
}

extern void
smtp_set_body(
  struct smtp*const                     io_smtp,
  gchar const*                          i_body)
{

  g_free((*io_smtp).m_body);
  (*io_smtp).m_body= g_strdup(i_body);

  return;
}

extern void
smtp_set_from(
  struct smtp*const                     io_smtp,
  gchar const*                          i_from)
{

  g_strlcpy((*io_smtp).m_from, i_from, sizeof((*io_smtp).m_from));

  return;
}

extern void
smtp_set_server(
  struct smtp*const                     io_smtp,
  gchar const*                          i_username,
  gchar const*                          i_password,
  gchar const*                          i_server,
  gchar const*                          i_port)
{

  g_strlcpy((*io_smtp).m_username, i_username, sizeof((*io_smtp).m_username));
  g_strlcpy((*io_smtp).m_password, i_password, sizeof((*io_smtp).m_password));
  g_strlcpy((*io_smtp).m_server, i_server, sizeof((*io_smtp).m_server));
  g_strlcpy((*io_smtp).m_port, i_port, sizeof((*io_smtp).m_port));

  return;
}

extern void
smtp_set_subject(
  struct smtp*const                     io_smtp,
  gchar const*                          i_subject)
{

  g_strlcpy((*io_smtp).m_subject, i_subject, sizeof((*io_smtp).m_subject));

  return;
}

static int
smtp_write(
  GError**                              o_error,
  BIO*const                             io_bio,
  gchar const*                          i_buffer,
  guint const                           i_buffer_size)
{
  guint                                 l_bytes_left;
  GError*                               l_error;
  int                                   l_exit;
  gchar const*                          l_text;
  int                                   l_rc;
  gchar const*                          l_tail;

  l_bytes_left= i_buffer_size;
  l_error= 0;
  l_exit= 0;
  l_tail= i_buffer;

for (guint slot= 0; slot < i_buffer_size; slot++) printf("%c", i_buffer[slot]);

  do
  {

    if (0 == l_bytes_left)
    {
      break;
    }

    l_rc= BIO_write(io_bio, i_buffer, l_bytes_left);

    if (1 <= l_rc)
    {
      l_bytes_left-= l_rc;
      l_tail+= l_rc;
      /*l_attempts= 0;*/
    }
    else
    {
      /*l_attempts++;*/

      l_rc= BIO_should_retry(io_bio);

      if (FALSE == l_rc /*|| 10 <= l_attempts*/)
      {
        l_rc= ERR_get_error();

        if (l_rc)
        {
          l_text= ERR_reason_error_string(l_rc);
        }
        else
        {
          l_text= "timeout";
        }

        l_error= g_error_new(
          smtp_error_domain,
          error_smtp_unable_to_write,
          "Unable to write to SMTP server: %s",
          l_text);

        l_exit= -1;
        break;
      }
    }

  }while(1);

  if (l_error)
  {
    g_propagate_error(o_error, l_error);
  }

  return l_exit;
}

int
smtp_write_string(
  GError**                              o_error,
  BIO*const                             io_bio,
  GString const*                        i_string);

#define smtp_write_string(e,b,s) smtp_write((e), (b), (s)->str, (s)->len)

#if 0
static void
smtp_flush_input(
  BIO*const                             io_bio)
{
  gchar                                 l_char;
  gint                                  l_fd;
  int                                   l_rc;
  fd_set                                l_set;
  struct timeval                        l_timeout;

  BIO_get_fd(io_bio, &l_fd);

  do
  {

    if (FALSE == BIO_pending(io_bio))
    {
      FD_ZERO(&l_set);
      FD_SET(l_fd, &l_set);

      l_timeout.tv_sec= 3;
      l_timeout.tv_usec= 0;

      l_rc= select(l_fd+1, &l_set, 0, 0, &l_timeout);

      if (-1 == l_rc)
      {
        break;
      }

      if (0 == l_rc)
      {
        break;
      }
    }

    do
    {

      BIO_read(io_bio, &l_char, 1);

printf("%c", l_char);

      if ('\n' == l_char)
      {
        break;
      }

    }while(1);

  }while(1);

  return;
}
#endif

static int
smtp_read_line(
  GError**                              o_error,
  BIO*const                             io_bio,
  GString*const                         o_buffer)
{
  gchar                                 l_char;
  GError*                               l_error;
  int                                   l_exit;
  gint                                  l_fd;
  gchar const*                          l_text;
  int                                   l_rc;
  fd_set                                l_set;
  struct timeval                        l_timeout;

  g_string_set_size(o_buffer, 0);
  memset((*o_buffer).str, 0, (*o_buffer).allocated_len);

  l_error= 0;
  l_exit= 0;

  BIO_get_fd(io_bio, &l_fd);

  FD_ZERO(&l_set);
  FD_SET(l_fd, &l_set);

  l_timeout.tv_sec= 3;
  l_timeout.tv_usec= 0;

  do
  {

    if (FALSE == BIO_pending(io_bio))
    {
      l_rc= select((1+l_fd), &l_set, 0, 0, &l_timeout);

      if (-1 == l_rc)
      {
        l_error= g_error_new(
          smtp_error_domain,
          error_smtp_unable_to_select_socket,
          "Unable to select socket");
        l_exit= -1;
        break;
      }

      if (0 == l_rc)
      {
        break;
      }
    }

    do
    {

      l_rc= BIO_read(io_bio, &l_char, 1);

      if (1 == l_rc)
      {
printf("%c", l_char);
        if ('\r' != l_char)
        {
          if ('\n' != l_char)
          {
            g_string_append_c(o_buffer, l_char);
          }
          else
          {
            break;
          }
        }
      }
      else
      {
        l_rc= BIO_should_retry(io_bio);

        if (FALSE == l_rc)
        {
          l_rc= ERR_get_error();

          if (l_rc)
          {
            l_text= ERR_reason_error_string(l_rc);
          }
          else
          {
            l_text= "timeout";
          }

          l_error= g_error_new(
            smtp_error_domain,
            error_smtp_unable_to_read,
            "Unable to read from SMTP server: %s",
            l_text);

          l_exit= -1;
          break;
        }
      }

    }while(1);

  }while(0);

  if (l_error)
  {
    g_propagate_error(o_error, l_error);
  }

  return l_exit;
}

static int
smtp_response_read(
  GError**                              o_error,
  BIO*const                             io_bio,
  GString*const                         o_buffer,
  guint const                           i_expected_code)
{
  guint                                 l_code;
  GError*                               l_error;
  int                                   l_exit;

  l_error= 0;
  l_exit= 0;

  do
  {

    l_exit= smtp_read_line(&l_error, io_bio, o_buffer);

    if (l_exit)
    {
      break;
    }

    if (0 == (*o_buffer).len)
    {
      l_error= g_error_new(
        smtp_error_domain,
        error_smtp_server_did_not_respond,
        "Unable to read from SMTP server");
      l_exit= -1;
      break;
    }

    l_code= strtoul((*o_buffer).str, 0, 10);

    if (i_expected_code != l_code)
    {
      l_error= g_error_new(
        smtp_error_domain,
        error_smtp_server_not_ready,
        "Server not ready: %u",
        l_code);

      l_exit= -1;
      break;
    }

  }while(0);

  if (l_error)
  {
    g_propagate_error(o_error, l_error);
  }

  return l_exit;
}

static int
smtp_connect(
  GError**                              o_error,
  BIO*const                             io_bio)
{
  GError*                               l_error;
  int                                   l_exit;
  gchar const*                          l_text;
  int                                   l_rc;

  l_error= 0;
  l_exit= 0;

  do
  {

    l_rc= BIO_do_connect(io_bio);

    if (1 == l_rc)
    {
      break;
    }

    l_rc= BIO_should_retry(io_bio);

    if (FALSE == l_rc)
    {
      l_rc= ERR_get_error();

      if (l_rc)
      {
        l_text= ERR_reason_error_string(l_rc);
      }
      else
      {
        l_text= "timeout";
      }

      l_error= g_error_new(
        smtp_error_domain,
        error_smtp_unable_to_connect,
        "Unable to connect to SMTP server: %s",
        l_text);

      l_exit= -1;
      break;
    }

  }while(1);

  if (l_error)
  {
    g_propagate_error(o_error, l_error);
  }

  return l_exit;
}

static enum authtype
smtp_authtype_lookup(
  gchar const*                          i_auth)
{
  enum authtype                         l_auth;
  int                                   l_rc;

  l_auth= auth_unknown;

  do
  {

    l_rc= strcmp("PLAIN", i_auth);

    if (0 == l_rc)
    {
      l_auth= auth_plain;
      break;
    }

    l_rc= strcmp("LOGIN", i_auth);

    if (0 == l_rc)
    {
      l_auth= auth_login;
      break;
    }

    l_rc= strcmp("GSSAPI", i_auth);

    if (0 == l_rc)
    {
      l_auth= auth_gssapi;
      break;
    }

    l_rc= strcmp("DIGEST-MD5", i_auth);

    if (0 == l_rc)
    {
      l_auth= auth_digest_md5;
      break;
    }

    l_rc= strcmp("MD5", i_auth);

    if (0 == l_rc)
    {
      l_auth= auth_md5;
      break;
    }

    l_rc= strcmp("CRAM-MD5", i_auth);

    if (0 == l_rc)
    {
      l_auth= auth_cram_md5;
      break;
    }

  }while(0);

  return l_auth;
}

static int
smtp_response_ehlo(
  GError**                              o_error,
  enum authtype*const                   o_auth,
  BIO*const                             io_bio,
  GString*const                         o_buffer)
{
  GError*                               l_error;
  int                                   l_exit;
  gchar*                                l_text;
  gchar*                                l_head;
  gchar*                                l_tail;

  (*o_auth)= 0;
  l_error= 0;
  l_exit= 0;

  do
  {

    l_exit= smtp_response_read(&l_error, io_bio, o_buffer, 250);

    if (l_exit)
    {
      break;
    }

    l_text= strstr((*o_buffer).str, "AUTH");

    if (l_text)
    {
      for (l_tail= l_text; (*l_tail && (' ' != *l_tail)); l_tail++);
      if (' ' == *l_tail)
      {
        l_tail++;
      }
      do
      {
        if (0 == *l_tail)
        {
          break;
        }
        for (l_head= l_tail; (*l_tail && (' ' != *l_tail)); l_tail++);
        if (' ' == *l_tail)
        {
          *l_tail= 0;
          l_tail++;
        }
        (*o_auth)|= smtp_authtype_lookup(l_head);
      }while(1);
    }

    if ('-' != (*o_buffer).str[3])
    {
      break;
    }

  }while(1);

  if (l_error)
  {
    g_propagate_error(o_error, l_error);
  }

  return l_exit;
}

static int
smtp_auth_login(
  GError**                              o_error,
  BIO*const                             io_bio,
  GString*const                         o_buffer,
  gchar const*                          i_username,
  gchar const*                          i_password)
{
  GError*                               l_error;
  int                                   l_exit;
  gchar*                                l_text;

  l_error= 0;
  l_exit= 0;

  do
  {

    l_exit= smtp_write(&l_error, io_bio, "AUTH LOGIN\r\n", 12);

    if (l_exit)
    {
      break;
    }

    l_exit= smtp_response_read(&l_error, io_bio, o_buffer, 334);

    if (l_exit)
    {
      break;
    }

    l_text= g_base64_encode((guchar*)i_username, strlen(i_username));
    l_exit= smtp_write(&l_error, io_bio, l_text, strlen(l_text));
    g_free(l_text);

    if (l_exit)
    {
      break;
    }

    l_exit= smtp_write(&l_error, io_bio, "\r\n", 2);

    if (l_exit)
    {
      break;
    }

    l_exit= smtp_response_read(&l_error, io_bio, o_buffer, 334);

    if (l_exit)
    {
      break;
    }

    l_text= g_base64_encode((guchar*)i_password, strlen(i_password));
    l_exit= smtp_write(&l_error, io_bio, l_text, strlen(l_text));
    g_free(l_text);

    if (l_exit)
    {
      break;
    }

    l_exit= smtp_write(&l_error, io_bio, "\r\n", 2);

    if (l_exit)
    {
      break;
    }

    l_exit= smtp_response_read(&l_error, io_bio, o_buffer, 235);

  }while(0);

  if (l_error)
  {
    g_propagate_error(o_error, l_error);
  }

  return l_exit;
}

static int
smtp_hello(
  GError**                              o_error,
  BIO*const                             io_bio,
  GString*const                         o_buffer,
  gchar const*                          i_username,
  gchar const*                          i_password)
{
  enum authtype                         l_auth;
  GError*                               l_error;
  int                                   l_exit;
  gchar                                 l_hostname[127+1];
  int                                   l_rc;

  l_error= 0;
  l_exit= 0;

  do
  {

    l_rc= gethostname(l_hostname, sizeof(l_hostname));

    if (l_rc)
    {
      l_error= g_error_new(
        smtp_error_domain,
        error_smtp_unable_to_get_hostname,
        "Unable to get hostname");
      l_exit= -1;
      break;
    }

    l_hostname[sizeof(l_hostname)-1]= 0;

    g_string_printf(o_buffer, "EHLO %s\r\n", l_hostname);
    l_exit= smtp_write_string(&l_error, io_bio, o_buffer);

    if (l_exit)
    {
      break;
    }

    l_exit= smtp_response_ehlo(&l_error, &l_auth, io_bio, o_buffer);

    if (l_exit)
    {
      break;
    }

    if (auth_login & l_auth)
    {
      l_exit= smtp_auth_login(&l_error, io_bio, o_buffer, i_username, i_password);
    }
    else
    {
        l_error= g_error_new(
          smtp_error_domain,
          error_smtp_protocol_unsupported,
          "Unsupported authentication protocol");

        l_exit= -1;
        break;
    }

  }while(0);

  if (l_error)
  {
    g_propagate_error(o_error, l_error);
  }

  return l_exit;
}

static int
smtp_mail_from(
  GError**                              o_error,
  BIO*const                             io_bio,
  GString*const                         o_buffer,
  struct smtp const*const               i_smtp)
{
  GError*                               l_error;
  int                                   l_exit;

  l_error= 0;
  l_exit= 0;

  do
  {

    g_string_printf(o_buffer, "MAIL FROM:<%s>\r\n", (*i_smtp).m_from);
    l_exit= smtp_write_string(&l_error, io_bio, o_buffer);

    if (l_exit)
    {
      break;
    }

    l_exit= smtp_response_read(&l_error, io_bio, o_buffer, 250);

  }while(0);

  if (l_error)
  {
    g_propagate_error(o_error, l_error);
  }

  return l_exit;
}

static int
smtp_rcpt_to(
  GError**                              o_error,
  BIO*const                             io_bio,
  GString*const                         o_buffer,
  struct smtp const*const               i_smtp)
{
  GError*                               l_error;
  int                                   l_exit;
  guint                                 l_slot;
  struct smtp_address const*            l_to;

  l_error= 0;
  l_exit= 0;

  l_to= (struct smtp_address*)(*i_smtp).m_to->data;

  for (l_slot= 0; (*i_smtp).m_to->len > l_slot; l_slot++)
  {

    g_string_printf(o_buffer, "RCPT TO:<%s>\r\n", (*l_to).m_address);
    l_exit= smtp_write_string(&l_error, io_bio, o_buffer);

    if (l_exit)
    {
      break;
    }

    l_exit= smtp_response_read(&l_error, io_bio, o_buffer, 250);

    if (l_exit)
    {
      break;
    }

    l_to++;

  }

  if (l_error)
  {
    g_propagate_error(o_error, l_error);
  }

  return l_exit;
}

static int
smtp_headers(
  GError**                              o_error,
  BIO*const                             io_bio,
  GString*const                         o_buffer,
  struct smtp const*const               i_smtp)
{
  GError*                               l_error;
  int                                   l_exit;
  GDateTime*                            l_date;
  guint                                 l_slot;
  gchar*                                l_text;
  struct smtp_address const*            l_to;

  l_error= 0;
  l_exit= 0;

  do
  {

    g_string_printf(o_buffer, "User-Agent: tabitha\r\n");
    l_exit= smtp_write_string(&l_error, io_bio, o_buffer);

    if (l_exit)
    {
      break;
    }

    l_date= g_date_time_new_now_local();
    l_text= g_date_time_format(l_date, "Date: %d %b %Y %H:%M %z\r\n");
    g_date_time_unref(l_date);

    l_exit= smtp_write(&l_error, io_bio, l_text, strlen(l_text));
    g_free(l_text);

    if (l_exit)
    {
      break;
    }

    g_string_printf(o_buffer, "Subject: %s\r\n", (*i_smtp).m_subject);
    l_exit= smtp_write_string(&l_error, io_bio, o_buffer);

    if (l_exit)
    {
      break;
    }

    g_string_printf(o_buffer, "From: <%s>\r\n", (*i_smtp).m_from);
    l_exit= smtp_write_string(&l_error, io_bio, o_buffer);

    if (l_exit)
    {
      break;
    }

    l_exit= smtp_write(&l_error, io_bio, "To: ", 4); 

    if (l_exit)
    {
      break;
    }

    l_to= (struct smtp_address*)(*i_smtp).m_to->data;

    for (l_slot= 0; (*i_smtp).m_to->len > l_slot; l_slot++)
    {

      if (l_slot)
      {
        l_exit= smtp_write(&l_error, io_bio, ",", 1);

        if (l_exit)
        {
          break;
        }
      }

      g_string_printf(o_buffer, "<%s>", (*l_to).m_address);
      l_exit= smtp_write_string(&l_error, io_bio, o_buffer);

      if (l_exit)
      {
        break;
      }

      l_to++;

    }

    l_exit= smtp_write(&l_error, io_bio, "\r\n", 2); 

    if (l_exit)
    {
      break;
    }

  }while(0);

  if (l_error)
  {
    g_propagate_error(o_error, l_error);
  }

  return l_exit;
}

static int
smtp_attachment(
  GError**                              o_error,
  BIO*const                             io_bio,
  GString*const                         o_buffer,
  struct smtp const*const               i_smtp,
  gchar const                           i_boundary[smtp_size_boundary])
{
  gchar*                                l_basename;
  gchar*                                l_blob;
  gboolean                              l_bool;
  GError*                               l_error;
  int                                   l_exit;
  struct smtp_file*                     l_file;
  GFile*                                l_fileobj;
  gsize                                 l_len;
  guint                                 l_slot;
  gchar*                                l_text;
  gchar const*                          l_type;

  l_blob= 0;
  l_error= 0;
  l_exit= 0;
  l_text= 0;

  l_file= (struct smtp_file*)(*i_smtp).m_attachment->data;

  for (l_slot= 0; (*i_smtp).m_attachment->len > l_slot; l_slot++)
  {
    l_bool= g_file_get_contents((*l_file).m_path, &l_blob, &l_len, &l_error);

    if (FALSE == l_bool)
    {
      l_exit= -1;
      break;
    }

    l_text= g_base64_encode((guchar*)l_blob, l_len);
    g_free(l_blob);
    l_blob= 0;

    l_fileobj= g_file_new_for_path((*l_file).m_path);
    l_basename= g_file_get_basename(l_fileobj);
    g_object_unref(l_fileobj);

    switch((*l_file).m_type)
    {
      default:
        l_type= "application/pdf";
    }

    g_string_printf(
      o_buffer,
      "--%s\r\n"
      "Content-Type: %s; name=\"%s\"\r\n"
      "Content-Disposition: attachment; filename=\"%s\"\r\n"
      "Content-Transfer-Encoding: base64\r\n\r\n",
      i_boundary,
      l_type,
      l_basename,
      l_basename);

    g_free(l_basename);

    l_exit= smtp_write_string(&l_error, io_bio, o_buffer);

    if (l_exit)
    {
      break;
    }

    l_exit= smtp_write(&l_error, io_bio, l_text, strlen(l_text));

    if (l_exit)
    {
      break;
    }

    l_exit= smtp_write(&l_error, io_bio, "\r\n", 2);

    if (l_exit)
    {
      break;
    }

    l_file++;
  }

  g_free(l_text);
  g_free(l_blob);

  if (l_error)
  {
    g_propagate_error(o_error, l_error);
  }

  return l_exit;
}

extern int
smtp_send(
  GError**                              o_error,
  struct smtp const*const               i_smtp)
{
  gchar                                 l_alt[smtp_size_boundary];
  BIO*                                  l_bio;
  GString*                              l_buffer;
  gboolean                              l_connected;
  SSL_CTX*                              l_ctx;
  GError*                               l_error;
  int                                   l_exit;
  gchar                                 l_multi[smtp_size_boundary];
  SSL*                                  l_ssl;

  smtp_generate_boundary(l_alt);
  l_bio= 0;
  l_buffer= g_string_sized_new(smtp_size_line);
  l_connected= FALSE;
  l_ctx= 0;
  l_error= 0;
  l_exit= 0;
  smtp_generate_boundary(l_multi);
  l_ssl= 0;

  ssl_init_once();

  do
  {

    l_ctx= SSL_CTX_new(SSLv23_client_method());
    l_bio= BIO_new_ssl_connect(l_ctx);
    BIO_set_conn_hostname(l_bio, (*i_smtp).m_server);
    BIO_set_conn_port(l_bio, (*i_smtp).m_port);
    BIO_set_conn_ip_family(l_bio, BIO_FAMILY_IPV4);
    BIO_set_nbio(l_bio, 1);
    BIO_get_ssl(l_bio, &l_ssl);
    SSL_set_mode(l_ssl, SSL_MODE_AUTO_RETRY);

    l_exit= smtp_connect(&l_error, l_bio);

    if (l_exit)
    {
      l_exit= -1;
      break;
    }

    l_connected= TRUE;

    l_exit= smtp_response_read(&l_error, l_bio, l_buffer, 220);

    if (l_exit)
    {
      break;
    }

    l_exit= smtp_hello(
      &l_error,
      l_bio,
      l_buffer,
      (*i_smtp).m_username,
      (*i_smtp).m_password);

    if (l_exit)
    {
      break;
    }

    l_exit= smtp_mail_from(&l_error, l_bio, l_buffer, i_smtp);

    if (l_exit)
    {
      break;
    }

    l_exit= smtp_rcpt_to(&l_error, l_bio, l_buffer, i_smtp);

    if (l_exit)
    {
      break;
    }

    l_exit= smtp_write(&l_error, l_bio, "DATA\r\n", 6);

    if (l_exit)
    {
      break;
    }

    l_exit= smtp_response_read(&l_error, l_bio, l_buffer, 354);

    if (l_exit)
    {
      break;
    }

    l_exit= smtp_headers(&l_error, l_bio, l_buffer, i_smtp);

    if (l_exit)
    {
      break;
    }

    g_string_printf(
      l_buffer, 
      "MIME-Version: 1.0\r\n"
      "Content-type: multipart/mixed; boundary=\"%s\"\r\n\r\n"
      "--%s\r\n"
      "Content-type: multipart/alternative; boundary=\"%s\"\r\n\r\n"
      "--%s\r\n"
      "Content-Type: text/plain; charset=\"UTF-8\"\r\n\r\n",
      l_multi,
      l_multi,
      l_alt,
      l_alt);

    l_exit= smtp_write_string(&l_error, l_bio, l_buffer);

    if (l_exit)
    {
      break;
    }

    if ((*i_smtp).m_body)
    {

      l_exit= smtp_write(&l_error, l_bio, (*i_smtp).m_body, strlen((*i_smtp).m_body));

      if (l_exit)
      {
        break;
      }
    }

    g_string_printf(l_buffer, "\r\n--%s--\r\n", l_alt);
    l_exit= smtp_write_string(&l_error, l_bio, l_buffer);

    if (l_exit)
    {
      break;
    }

    if ((*i_smtp).m_attachment)
    {
      l_exit= smtp_attachment(&l_error, l_bio, l_buffer, i_smtp, l_multi);

      if (l_exit)
      {
        break;
      }
    }

    g_string_printf(l_buffer, "--%s--\r\n", l_multi);
    l_exit= smtp_write_string(&l_error, l_bio, l_buffer);

    if (l_exit)
    {
      break;
    }

    l_exit= smtp_write(&l_error, l_bio, "\r\n.\r\n", 5);

    if (l_exit)
    {
      break;
    }

  }while(0);

  if (l_connected)
  {
    BIO_write(l_bio, "QUIT\r\n", 6);
  }

  g_string_free(l_buffer, TRUE);
  BIO_free_all(l_bio);
  SSL_CTX_free(l_ctx);  

  if (l_error)
  {
    g_propagate_error(o_error, l_error);
  }

  return l_exit;
}
/* 
vim:expandtab:softtabstop=2:tabstop=2:shiftwidth=2:nowrap:ruler 
*/
