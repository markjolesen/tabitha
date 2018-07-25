/*
  smtp_client.c

  License CC0 PUBLIC DOMAIN

  To the extent possible under law, Mark J. Olesen has waived all copyright
  and related or neighboring rights to tabitha. This work is published from:
  United States.
*/
#if !defined(_WIN32)
#include <sys/select.h>
#else
#include <winsock2.h>
#endif
#include "smtp_client.h"
#include "error.h"
#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/err.h>

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

struct smtp_client_address
{
  gchar*                                m_email;
};

struct smtp_client_attachment
{
  gchar*                                m_path;
  gchar*                                m_mimetype;
  gchar*                                m_name;
};

enum
{
  prop_0,
  prop_username,
  prop_password,
  prop_server,
  prop_port,
  prop_from,
  prop_subject,
  prop_to,
  prop_body,
  prop_last
};

struct _SmtpClientPrivate
{
  gchar*                                m_username;
  gchar*                                m_password;
  gchar*                                m_server;
  gchar*                                m_port;
  gchar*                                m_from;
  gchar*                                m_subject;
  GArray*                               m_to;
  gchar*                                m_body;
  GArray*                               m_attachment;
  smtp_status_callback                  m_callback;
  gpointer                              m_user_data;
};

typedef struct _SmtpClientPrivate SmtpClientPrivate;

struct _SmtpClient
{
  GObject                               parent_instance;
  SmtpClientPrivate*                    m_priv;
};

G_DEFINE_TYPE_WITH_PRIVATE (SmtpClient, smtp_client, G_TYPE_OBJECT)

G_DEFINE_QUARK(smtp-client-error-domain_quark, smtp_client_error_domain)

static void
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

static void
smtp_client_address_add(
  GArray*const                          io_array,
  gchar const*                          i_email)
{
  struct smtp_client_address*           l_address;

  l_address= (struct smtp_client_address*)g_malloc0(sizeof(*l_address));
  (*l_address).m_email= g_strdup(i_email);
  g_array_append_val(io_array, (*l_address));
  g_free(l_address);

  return;
}

static void
smtp_client_address_clear(
  GArray*const                          io_array)
{
  struct smtp_client_address*           l_address;
  guint                                 l_slot;

  l_address= (struct smtp_client_address*)(*io_array).data;

  for (l_slot= 0; (*io_array).len > l_slot; l_slot++)
  {
    g_free((*l_address).m_email);
    memset(l_address, 0, sizeof(*l_address));
    l_address++;
  }

  (*io_array).len= 0;

  return;
}

static gchar**
smtp_client_address_get(
  GArray const*const                    io_array)
{
  struct smtp_client_address*           l_address;
  guint                                 l_slot;
  gchar**                               l_strv;

  l_strv= g_malloc0(sizeof(gpointer) * (1 + (*io_array).len));

  l_address= (struct smtp_client_address*)(*io_array).data;

  for (l_slot= 0; (*io_array).len > l_slot; l_slot++)
  {
    l_strv[l_slot]= g_strdup((*l_address).m_email);
    l_address++;
  }

  return l_strv;
}

static void
smtp_client_address_set(
  GArray*const                          io_array,
  gchar const**                         i_strv)
{
  struct smtp_client_address*           l_address;
  gsize                                 l_slot;
  gchar const*                          l_text;

  smtp_client_address_clear(io_array);

  l_address= g_malloc(sizeof(*l_address));
  l_slot= 0;

  while((l_text= i_strv[l_slot]))
  {
    (*l_address).m_email= g_strdup(l_text);
    g_array_append_val(io_array, (*l_address));
    l_slot++;
  }

  g_free(l_address);

  return;
}

static void
smtp_client_attachment_add(
  GArray*const                          io_array,
  gchar const*                          i_path,
  gchar const*                          i_mimetype,
  gchar const*                          i_name)
{
  struct smtp_client_attachment*        l_attachment;

  l_attachment= (struct smtp_client_attachment*)g_malloc0(sizeof(*l_attachment));
  (*l_attachment).m_path= g_strdup(i_path);
  (*l_attachment).m_mimetype= g_strdup(i_mimetype);
  (*l_attachment).m_name= g_strdup(i_name);
  g_array_append_val(io_array, (*l_attachment));
  g_free(l_attachment);

  return;
}

static void
smtp_client_attachment_clear(
  GArray*const                          io_attachment)
{
  struct smtp_client_attachment*        l_attachment;
  guint                                 l_slot;

  l_attachment= (struct smtp_client_attachment*)(*io_attachment).data;

  for (l_slot= 0; (*io_attachment).len > l_slot; l_slot++)
  {
    g_free((*l_attachment).m_path);
    g_free((*l_attachment).m_mimetype);
    g_free((*l_attachment).m_name);
    memset(l_attachment, 0, sizeof(*l_attachment));
    l_attachment++;
  }

  (*io_attachment).len= 0;

  return;
}

static void
smtp_client_finalize (
  GObject*const                         io_gobject)
{
  SmtpClientPrivate*                    l_priv;

  l_priv= smtp_client_get_instance_private(SMTP_CLIENT(io_gobject));

  g_free((*l_priv).m_username);
  g_free((*l_priv).m_password);
  g_free((*l_priv).m_server);
  g_free((*l_priv).m_port);
  g_free((*l_priv).m_from);
  g_free((*l_priv).m_subject);
  g_free((*l_priv).m_body);

  if ((*l_priv).m_to)
  {
    smtp_client_address_clear((*l_priv).m_to);
    g_array_free((*l_priv).m_to, TRUE);
  }

  if ((*l_priv).m_attachment)
  {
    smtp_client_attachment_clear((*l_priv).m_attachment);
    g_array_free((*l_priv).m_attachment, TRUE);
  }

  G_OBJECT_CLASS(smtp_client_parent_class)->finalize(io_gobject);

  return;
}

static void
smtp_client_init(
  SmtpClient*const                      io_self)
{
  SmtpClientPrivate*                    l_priv;

  l_priv= smtp_client_get_instance_private(io_self);

  (*l_priv).m_to= g_array_new(FALSE, TRUE, sizeof(struct smtp_client_address));
  (*l_priv).m_attachment= g_array_new(FALSE, TRUE, sizeof(struct smtp_client_attachment));

  return;
}

static void
smtp_client_set_property(
  GObject*const                         io_object,
  guint const                           i_prop_id,
  GValue const*const                    i_value,
  GParamSpec*const                      io_pspec)
{
  gchar const**                         l_strv;
  SmtpClient*                           l_self;

  l_self= SMTP_CLIENT(io_object);

  switch(i_prop_id)
  {
    case prop_username:
      g_free((*l_self).m_priv->m_username);
      (*l_self).m_priv->m_username= g_value_dup_string(i_value);
      break;
    case prop_password:
      g_free((*l_self).m_priv->m_password);
      (*l_self).m_priv->m_password= g_value_dup_string(i_value);
      break;
    case prop_server:
      g_free((*l_self).m_priv->m_server);
      (*l_self).m_priv->m_server= g_value_dup_string(i_value);
      break;
    case prop_port:
      g_free((*l_self).m_priv->m_port);
      (*l_self).m_priv->m_port= g_value_dup_string(i_value);
      break;
    case prop_from:
      g_free((*l_self).m_priv->m_from);
      (*l_self).m_priv->m_from= g_value_dup_string(i_value);
      break;
    case prop_subject:
      g_free((*l_self).m_priv->m_subject);
      (*l_self).m_priv->m_subject= g_value_dup_string(i_value);
      break;
    case prop_to:
      l_strv= (gchar const**)g_value_get_boxed(i_value);
      smtp_client_address_set((*l_self).m_priv->m_to, l_strv);
      break;
    case prop_body:
      g_free((*l_self).m_priv->m_body);
      (*l_self).m_priv->m_body= g_value_dup_string(i_value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(io_object, i_prop_id, io_pspec);
      break;
  }

  return;
}

static void
smtp_client_get_property (
  GObject*const                         io_object,
  guint const                           i_prop_id,
  GValue*const                          o_value,
  GParamSpec*const                      i_pspec)
{
  SmtpClient*                           l_self;
  gchar**                               l_strv;

  l_self= SMTP_CLIENT(io_object);

  switch(i_prop_id)
  {
    case prop_username:
      g_value_set_string(o_value, (*l_self).m_priv->m_username);
      break;
    case prop_password:
      g_value_set_string(o_value, (*l_self).m_priv->m_password);
      break;
    case prop_server:
      g_value_set_string(o_value, (*l_self).m_priv->m_server);
      break;
    case prop_port:
      g_value_set_string(o_value, (*l_self).m_priv->m_port);
      break;
    case prop_from:
      g_value_set_string(o_value, (*l_self).m_priv->m_from);
      break;
    case prop_subject:
      g_value_set_string(o_value, (*l_self).m_priv->m_subject);
      break;
    case prop_to:
      l_strv= smtp_client_address_get((*l_self).m_priv->m_to);
      g_value_take_boxed(o_value, l_strv);
      break;
    case prop_body:
      g_value_set_string(o_value, (*l_self).m_priv->m_body);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(io_object, i_prop_id, i_pspec);
      break;
  }

  return;
}

static void
smtp_client_class_init(
  SmtpClientClass*const                 io_klass)
{
  GObjectClass*                         l_object_class;

  l_object_class= G_OBJECT_CLASS(io_klass);

  (*l_object_class).finalize= smtp_client_finalize;
  (*l_object_class).set_property= smtp_client_set_property;
  (*l_object_class).get_property= smtp_client_get_property;

  g_object_class_install_property(
    l_object_class,
    prop_username,
    g_param_spec_string(
      "username",
      "Username",
      "User login name",
      0,
      G_PARAM_READWRITE));
    
  g_object_class_install_property(
    l_object_class,
    prop_password,
    g_param_spec_string(
      "password",
      "Password",
      "User password",
      0,
      G_PARAM_READWRITE));

  g_object_class_install_property(
    l_object_class,
    prop_server,
    g_param_spec_string(
      "server",
      "server",
      "SMTP server",
      0,
      G_PARAM_READWRITE));

  g_object_class_install_property(
    l_object_class,
    prop_port,
    g_param_spec_string(
      "port",
      "Port",
      "Server port number",
      "465",
      G_PARAM_READWRITE));

  g_object_class_install_property(
    l_object_class,
    prop_from,
    g_param_spec_string(
      "from",
      "From",
      "From e-mail address",
      0,
      G_PARAM_READWRITE));

  g_object_class_install_property(
    l_object_class,
    prop_subject,
    g_param_spec_string(
      "subject",
      "Subject",
      "Subject line",
      0,
      G_PARAM_READWRITE));

  g_object_class_install_property(
    l_object_class,
    prop_to,
    g_param_spec_boxed(
      "to",
      "To",
      "To address list",
      G_TYPE_STRV,
      G_PARAM_READWRITE));

  g_object_class_install_property(
    l_object_class,
    prop_body,
    g_param_spec_string(
      "body",
      "Body",
      "Message body",
      0,
      G_PARAM_READWRITE));

  return;
}

extern void
smtp_client_add_attachment(
  SmtpClient*const                      io_smtp,
  gchar const*                          i_path,
  gchar const*                          i_mimetype,
  gchar const*                          i_name)
{
  SmtpClientPrivate*                    l_priv;

  l_priv= smtp_client_get_instance_private(io_smtp);
  smtp_client_attachment_add((*l_priv).m_attachment, i_path, i_mimetype, i_name);

  return;
}

extern void
smtp_client_add_to(
  SmtpClient*const                      io_smtp,
  gchar const*                          i_email)
{
  SmtpClientPrivate*                    l_priv;

  l_priv= smtp_client_get_instance_private(io_smtp);
  smtp_client_address_add((*l_priv).m_to, i_email);

  return;
}

extern void
smtp_client_set_body(
  SmtpClient*const                      io_smtp,
  gchar const*                          i_message)
{
  SmtpClientPrivate*                    l_priv;

  l_priv= smtp_client_get_instance_private(io_smtp);
  g_free((*l_priv).m_body);
  (*l_priv).m_body= g_strdup(i_message);

  return;
}

extern void
smtp_client_set_from(
  SmtpClient*const                      io_smtp,
  gchar const*                          i_email)
{
  SmtpClientPrivate*                    l_priv;

  l_priv= smtp_client_get_instance_private(io_smtp);
  g_free((*l_priv).m_from);
  (*l_priv).m_from= g_strdup(i_email);

  return;
}

extern void
smtp_client_set_server(
  SmtpClient*const                      io_smtp,
  gchar const*                          i_username,
  gchar const*                          i_password,
  gchar const*                          i_server,
  gchar const*                          i_port)
{
  SmtpClientPrivate*                    l_priv;

  l_priv= smtp_client_get_instance_private(io_smtp);
  g_free((*l_priv).m_username);
  g_free((*l_priv).m_password);
  g_free((*l_priv).m_server);
  g_free((*l_priv).m_port);
  (*l_priv).m_username= g_strdup(i_username);
  (*l_priv).m_password= g_strdup(i_password);
  (*l_priv).m_server= g_strdup(i_server);
  (*l_priv).m_port= g_strdup(i_port);

  return;
}

extern void
smtp_client_set_status_callback(
  SmtpClient*const                      io_smtp,
  smtp_status_callback                  i_callback,
  gpointer                              io_user_data)
{
  SmtpClientPrivate*                    l_priv;

  l_priv= smtp_client_get_instance_private(io_smtp);
  (*l_priv).m_callback= i_callback;
  (*l_priv).m_user_data= io_user_data;

  return;
}

extern void
smtp_client_set_subject(
  SmtpClient*const                      io_smtp,
  gchar const*                          i_subject)
{
  SmtpClientPrivate*                    l_priv;

  l_priv= smtp_client_get_instance_private(io_smtp);
  g_free((*l_priv).m_subject);
  (*l_priv).m_subject= g_strdup(i_subject);

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
          l_error= g_error_new(
            smtp_client_error_domain,
            smtp_client_unable_to_write,
            "Unable to write to SMTP server: %s",
            l_text);
          l_exit= -1;
          break;
        }
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
          smtp_client_error_domain,
          smtp_client_unable_to_select_socket,
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
            l_error= g_error_new(
              smtp_client_error_domain,
              smtp_client_unable_to_read,
              "Unable to read from SMTP server: %s",
              l_text);
            l_exit= -1;
            break;
          }
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
        smtp_client_error_domain,
        smtp_client_server_did_not_respond,
        "Unable to read from SMTP server");
      l_exit= -1;
      break;
    }

    l_code= strtoul((*o_buffer).str, 0, 10);

    if (i_expected_code != l_code)
    {
      l_error= g_error_new(
        smtp_client_error_domain,
        smtp_client_server_not_ready,
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
        l_error= g_error_new(
          smtp_client_error_domain,
          smtp_client_unable_to_connect,
          "Unable to connect to SMTP server: %s",
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
        smtp_client_error_domain,
        smtp_client_unable_to_get_hostname,
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
          smtp_client_error_domain,
          smtp_client_protocol_unsupported,
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
  gchar const*                          i_email)
{
  GError*                               l_error;
  int                                   l_exit;

  l_error= 0;
  l_exit= 0;

  do
  {

    g_string_printf(o_buffer, "MAIL FROM:<%s>\r\n", i_email);
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
  GArray const*const                    i_array)
{
  struct smtp_client_address const*     l_address;
  GError*                               l_error;
  int                                   l_exit;
  guint                                 l_slot;

  l_error= 0;
  l_exit= 0;

  l_address= (struct smtp_client_address*)(*i_array).data;

  for (l_slot= 0; (*i_array).len > l_slot; l_slot++)
  {

    g_string_printf(o_buffer, "RCPT TO:<%s>\r\n", (*l_address).m_email);
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

    l_address++;

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
  SmtpClientPrivate const*const         i_smtp)
{
  GError*                               l_error;
  int                                   l_exit;
  GDateTime*                            l_date;
  guint                                 l_slot;
  gchar*                                l_text;
  struct smtp_client_address const*     l_to;

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

    l_to= (struct smtp_client_address*)(*i_smtp).m_to->data;

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

      g_string_printf(o_buffer, "<%s>", (*l_to).m_email);
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
  GArray const*const                    i_array,
  gchar const                           i_boundary[smtp_size_boundary])
{
  gchar*                                l_basename;
  gchar*                                l_blob;
  gboolean                              l_bool;
  GError*                               l_error;
  int                                   l_exit;
  struct smtp_client_attachment*        l_attachment;
  GFile*                                l_file;
  gsize                                 l_len;
  guint                                 l_slot;
  gchar*                                l_text;

  l_basename= 0;
  l_blob= 0;
  l_error= 0;
  l_exit= 0;
  l_text= 0;

  l_attachment= (struct smtp_client_attachment*)(*i_array).data;

  for (l_slot= 0; (*i_array).len > l_slot; l_slot++)
  {
    l_bool= g_file_get_contents((*l_attachment).m_path, &l_blob, &l_len, &l_error);

    if (FALSE == l_bool)
    {
      l_exit= -1;
      break;
    }

    l_text= g_base64_encode((guchar*)l_blob, l_len);
    g_free(l_blob);
    l_blob= 0;

    if (0 == (*l_attachment).m_name || 0 == (*l_attachment).m_name[0])
    {
      l_file= g_file_new_for_path((*l_attachment).m_path);
      l_basename= g_file_get_basename(l_file);
      g_object_unref(l_file);
    }
    else
    {
      l_basename= g_strdup((*l_attachment).m_name);
    }

    g_string_printf(
      o_buffer,
      "--%s\r\n"
      "Content-Type: %s;\r\n"
      "Content-Disposition: attachment; filename=\"%s\"\r\n"
      "Content-Transfer-Encoding: base64\r\n\r\n",
      i_boundary,
      (*l_attachment).m_mimetype,
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

    l_attachment++;
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
smtp_client_send(
  GError**                              o_error,
  SmtpClient const*const                i_smtp)
{
  gchar                                 l_alt[smtp_size_boundary];
  BIO*                                  l_bio;
  GString*                              l_buffer;
  gboolean                              l_cancel;
  gboolean                              l_connected;
  SSL_CTX*                              l_ctx;
  GError*                               l_error;
  int                                   l_exit;
  gchar                                 l_multi[smtp_size_boundary];
  SmtpClientPrivate const*              l_priv;
  SSL*                                  l_ssl;

  smtp_generate_boundary(l_alt);
  l_bio= 0;
  l_buffer= g_string_sized_new(smtp_size_line);
  l_cancel= FALSE;
  l_connected= FALSE;
  l_ctx= 0;
  l_error= 0;
  l_exit= 0;
  smtp_generate_boundary(l_multi);
  l_priv= smtp_client_get_instance_private((SmtpClient*)i_smtp);
  l_ssl= 0;

  ssl_init_once();

  do
  {

    if ((*l_priv).m_callback)
    {
      g_string_printf(l_buffer, "Connecting to %s port %s\n", (*l_priv).m_server, (*l_priv).m_port);
      l_cancel= (*l_priv).m_callback((*l_priv).m_user_data, i_smtp, (*l_buffer).str);
      if (l_cancel)
      {
        break;
      }
    }

    l_ctx= SSL_CTX_new(SSLv23_client_method());
    l_bio= BIO_new_ssl_connect(l_ctx);
    BIO_set_conn_hostname(l_bio, (*l_priv).m_server);
    BIO_set_conn_port(l_bio, (*l_priv).m_port);
#if !defined(_WIN32)
    BIO_set_conn_ip_family(l_bio, BIO_FAMILY_IPV4);
#endif
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

    if ((*l_priv).m_callback)
    {
      g_string_printf(l_buffer, "Server negotiation\n");
      l_cancel= (*l_priv).m_callback((*l_priv).m_user_data, i_smtp, (*l_buffer).str);
      if (l_cancel)
      {
        break;
      }
    }

    l_exit= smtp_hello(
      &l_error,
      l_bio,
      l_buffer,
      (*l_priv).m_username,
      (*l_priv).m_password);

    if (l_exit)
    {
      break;
    }

    l_exit= smtp_mail_from(&l_error, l_bio, l_buffer, (*l_priv).m_from);

    if (l_exit)
    {
      break;
    }

    l_exit= smtp_rcpt_to(&l_error, l_bio, l_buffer, (*l_priv).m_to);

    if (l_exit)
    {
      break;
    }

    if ((*l_priv).m_callback)
    {
      g_string_printf(l_buffer, "Sending data\n");
      l_cancel= (*l_priv).m_callback((*l_priv).m_user_data, i_smtp, (*l_buffer).str);
      if (l_cancel)
      {
        break;
      }
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

    l_exit= smtp_headers(&l_error, l_bio, l_buffer, l_priv);

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

    if ((*l_priv).m_body)
    {

      l_exit= smtp_write(&l_error, l_bio, (*l_priv).m_body, strlen((*l_priv).m_body));

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

    if ((*l_priv).m_attachment)
    {

      if ((*l_priv).m_callback)
      {
        g_string_printf(l_buffer, "Sending attachment's\n");
        l_cancel= (*l_priv).m_callback((*l_priv).m_user_data, i_smtp, (*l_buffer).str);
        if (l_cancel)
        {
          break;
        }
      }

      l_exit= smtp_attachment(&l_error, l_bio, l_buffer, (*l_priv).m_attachment, l_multi);

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

  if ((*l_priv).m_callback)
  {
    g_string_printf(l_buffer, "Ending session\n");
    (*l_priv).m_callback((*l_priv).m_user_data, i_smtp, (*l_buffer).str);
  }

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
