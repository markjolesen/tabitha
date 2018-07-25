/*
  sales_email.c

  License CC0 PUBLIC DOMAIN

  To the extent possible under law, Mark J. Olesen has waived all copyright
  and related or neighboring rights to tabitha. This work is published from:
  United States.
*/
#include "sales.h"
#include "error.h"
#include "smtp_client.h"

struct sales_email_thread_data
{
  GtkWindow*                            m_parent;
  GtkTextBuffer*                        m_textbuffer;
  GtkButton*                            m_cancel_button;
  struct session*                       m_session;
  struct email const*                   m_email;
  gchar const*                          m_sales_id;
  gchar*                                m_basename;
  gchar*                                m_path;
  gboolean                              m_cancel;
};

static int
sales_email_validate(
  GtkWindow*const                       io_parent,
  struct email const*const              i_email)
{
  GError*                               l_error;
  int                                   l_exit;

  l_error= 0;
  l_exit= -1;

  do
  {

    if (0 == (*i_email).m_from || 0 == (*i_email).m_from[0])
    {
      l_error= g_error_new(
        domain_sales,
        error_sales_missing_from,
        "Missing or invalid 'from' address");
      break;
    }

    if (0 == (*i_email).m_to || 0 == (*i_email).m_to[0])
    {
      l_error= g_error_new(
        domain_sales,
        error_sales_missing_to,
        "Missing or invalid 'to' address");
      break;
    }

    if (0 == (*i_email).m_subject || 0 == (*i_email).m_subject[0])
    {
      l_error= g_error_new(
        domain_sales,
        error_sales_missing_subject,
        "Missing or invalid 'subject'");
      break;
    }

    if (0 == (*i_email).m_server || 0 == (*i_email).m_server[0])
    {
      l_error= g_error_new(
        domain_sales,
        error_sales_missing_subject,
        "Missing or invalid 'server'");
      break;
    }

    if (0 == (*i_email).m_username || 0 == (*i_email).m_username[0])
    {
      l_error= g_error_new(
        domain_sales,
        error_sales_missing_username,
        "Missing 'username'");
      break;
    }

    if (0 == (*i_email).m_password || 0 == (*i_email).m_password[0])
    {
      l_error= g_error_new(
        domain_sales,
        error_sales_missing_password,
        "Missing 'password'");
      break;
    }

    if (0 == (*i_email).m_port || 0 == (*i_email).m_port[0])
    {
      l_error= g_error_new(
        domain_sales,
        error_sales_missing_port,
        "Missing or invalid 'port'");
      break;
    }

    l_exit= 0;

  }while(0);

  if (l_error)
  {
    _error_display(io_parent, l_error);
    g_clear_error(&l_error);
  }

  return l_exit;
}

static gboolean
sales_email_status_callback(
  gpointer                              io_user_data,
  G_GNUC_UNUSED SmtpClient const*const  i_smtp,
  gchar const*                          i_message)
{
  struct sales_email_thread_data*       l_data;

  l_data= (struct sales_email_thread_data*)io_user_data;
  gtk_text_buffer_insert_at_cursor((*l_data).m_textbuffer, i_message, -1);

  return (*l_data).m_cancel; 
}

static gpointer
sales_email_run(
  gpointer                              i_data)
{
  struct sales_email_thread_data*       l_data;
  GError*                               l_error;
  int                                   l_exit;
  gchar*                                l_head;
  SmtpClient*                           l_smtp;
  gchar*                                l_tail;
  GString*                              l_buffer;

  l_buffer= g_string_sized_new(1024);
  l_data= (struct sales_email_thread_data*)i_data;
  l_error= 0;
  l_exit= 0;
  l_smtp= 0;

  do
  {

    l_exit= sales_print(
      (*l_data).m_session,
      (*l_data).m_parent,
      (*l_data).m_sales_id,
      (*l_data).m_path);

    if (l_exit)
    {
      break;
    }

    l_smtp= smtp_client_new();

    smtp_client_set_status_callback(l_smtp, sales_email_status_callback, l_data);

    smtp_client_set_server(
      l_smtp,
      (*l_data).m_email->m_username,
      (*l_data).m_email->m_password,
      (*l_data).m_email->m_server,
      (*l_data).m_email->m_port);

    smtp_client_set_from(l_smtp, (*l_data).m_email->m_from);
    smtp_client_set_subject(l_smtp, (*l_data).m_email->m_subject);
    smtp_client_set_body(l_smtp, (*l_data).m_email->m_message);
    smtp_client_add_attachment(
      l_smtp,
      (*l_data).m_path,
      "application/pdf",
      (*l_data).m_basename);


    g_string_assign(l_buffer, (*l_data).m_email->m_to);
    l_head= (*l_buffer).str;

    do
    {

      if (0 == l_head || 0 == l_head[0])
      {
        break;
      }

      l_tail= strchr(l_head, ',');

      if (l_tail)
      {
        *l_tail++= 0;
      }

      g_strstrip(l_head);
      smtp_client_add_to(l_smtp, l_head);
      l_head= l_tail;

    }while(1);

    l_exit= smtp_client_send(&l_error, l_smtp);

  }while(0);

  gtk_button_set_label((*l_data).m_cancel_button, "gtk-close");

  if (l_error)
  {
    g_string_printf(
      l_buffer,
      "[%d.%d] %s",
      (*l_error).domain,
      (*l_error).code,
      (*l_error).message);
    gtk_text_buffer_insert_at_cursor((*l_data).m_textbuffer, (*l_buffer).str, (*l_buffer).len);
    g_clear_error(&l_error);
  }

  g_string_free(l_buffer, TRUE);
  g_object_unref(l_smtp);

  g_thread_exit((gpointer)(ptrdiff_t)l_exit);

  return (gpointer)(ptrdiff_t)l_exit;
}

extern int
sales_email(
  struct session*const                  io_session,
  GtkWindow*const                       io_parent,
  GtkBuilder*const                      io_builder,
  struct email const*const              i_email,
  gchar const                           i_sales_id[size_pg_big_int],
  enum salestype const                  i_sales_type)
{
  struct sales_email_thread_data        l_data;
  GtkDialog*                            l_dialog;
  GtkTextIter                           l_end;
  GError*                               l_error;
  int                                   l_exit;
  GFile*                                l_file;
  GFileIOStream*                        l_iostream;
  GtkTextIter                           l_start;
  GtkTextView*                          l_textview;
  GThread*                              l_thread;

  memset(&l_data, 0, sizeof(l_data));
  l_dialog= 0;
  l_error= 0;
  l_exit= 0;
  l_file= 0;
  l_thread= 0;

  do
  {

    l_exit= sales_email_validate(io_parent, i_email);

    if (l_exit)
    {
      break;
    }

    l_dialog= GTK_DIALOG(gtk_builder_get_object(io_builder, "dialog_status"));

    if (0 == l_dialog)
    {
      l_error= g_error_new(
        domain_general,
        error_generic,
        "Unable to find dialog: '%s'",
        "dialog_status");
      l_exit= -1;
      break;
    }

    gtk_window_set_transient_for(GTK_WINDOW(l_dialog), io_parent);

    l_textview= GTK_TEXT_VIEW(gtk_builder_get_object(io_builder, "status_textview"));
    l_data.m_cancel_button= GTK_BUTTON(gtk_builder_get_object(io_builder, "status_cancel_button"));

    if (0 == l_textview || 0 == l_data.m_cancel_button)
    {
      l_error= g_error_new(
        domain_general,
        error_generic,
        "Unable to load dialog: '%s'",
        "dialog_status");
      l_exit= -1;
      break;
    }

    gtk_button_set_label(l_data.m_cancel_button, "gtk-cancel");
    l_data.m_parent= io_parent;
    l_data.m_textbuffer= gtk_text_view_get_buffer(l_textview);
    gtk_text_buffer_get_start_iter(l_data.m_textbuffer, &l_start);
    gtk_text_buffer_get_end_iter(l_data.m_textbuffer, &l_end);
    gtk_text_buffer_delete(l_data.m_textbuffer, &l_start, &l_end);
    l_data.m_session= io_session;
    l_data.m_email= i_email;
    l_data.m_sales_id= i_sales_id;
    l_file= g_file_new_tmp(0, &l_iostream, &l_error);

    if (0 == l_file)
    {
      l_exit= -1;
      break;
    }

    l_data.m_path= g_file_get_path(l_file);
    l_data.m_basename= (gchar*)g_malloc0(127+1);
    g_snprintf(
      l_data.m_basename,
      128,
      "%s_%s.pdf",
      sales_type_as_string(i_sales_type),
      i_sales_id); 

    l_thread= g_thread_new("sales_email", sales_email_run, &l_data);

    gtk_widget_show_all(GTK_WIDGET(l_dialog));
    gtk_window_set_modal(GTK_WINDOW(l_dialog), 1);

    gtk_dialog_run(l_dialog);

    l_data.m_cancel= TRUE;

    l_exit= (int)(ptrdiff_t)g_thread_join(l_thread);

  }while(0);

  g_free(l_data.m_basename);
  g_free(l_data.m_path);

  if (l_file)
  {
    g_file_delete(l_file, 0, 0);
    g_object_unref(l_file);
  }

  if (l_thread)
  {
    g_thread_unref(l_thread);
  }

  if (l_dialog)
  {
    gtk_widget_hide(GTK_WIDGET(l_dialog));
  }

  if (l_error)
  {
    _error_display(io_parent, l_error);
    g_clear_error(&l_error);
  }

  return l_exit;
}
/* 
vim:expandtab:softtabstop=2:tabstop=2:shiftwidth=2:nowrap:ruler 
*/
