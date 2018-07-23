/*
  email_form.c

  License CC0 PUBLIC DOMAIN

  To the extent possible under law, Mark J. Olesen has waived all copyright
  and related or neighboring rights to tabitha. This work is published from:
  United States.
*/
#include "email.h"
#include "error.h"

struct property
{
  GtkEntry*                             m_from;
  GtkEntry*                             m_to;
  GtkEntry*                             m_subject;
  GtkTextView*                          m_message;
  GtkEntry*                             m_server;
  GtkEntry*                             m_username;
  GtkEntry*                             m_password;
  GtkSpinButton*                        m_port;
};

static int
bind(
  struct property*const                 o_property,
  GtkBuilder*const                      io_builder)
{
  int                                   l_exit;

  memset(o_property, 0, sizeof(*o_property));
  l_exit= -1;

  do
  {

    (*o_property).m_from=
      GTK_ENTRY(gtk_builder_get_object(io_builder, "email_from"));

    if (0 == (*o_property).m_from)
    {
      break;
    }

    (*o_property).m_to=
      GTK_ENTRY(gtk_builder_get_object(io_builder, "email_to"));

    if (0 == (*o_property).m_to)
    {
      break;
    }

    (*o_property).m_subject=
      GTK_ENTRY(gtk_builder_get_object(io_builder, "email_subject"));

    if (0 == (*o_property).m_subject)
    {
      break;
    }

    (*o_property).m_message=
      GTK_TEXT_VIEW(gtk_builder_get_object(io_builder, "email_textview"));

    if (0 == (*o_property).m_message)
    {
      break;
    }


    (*o_property).m_server=
      GTK_ENTRY(gtk_builder_get_object(io_builder, "email_server"));

    if (0 == (*o_property).m_server)
    {
      break;
    }

    (*o_property).m_username=
      GTK_ENTRY(gtk_builder_get_object(io_builder, "email_email"));

    if (0 == (*o_property).m_username)
    {
      break;
    }

    (*o_property).m_password=
      GTK_ENTRY(gtk_builder_get_object(io_builder, "email_password"));

    if (0 == (*o_property).m_password)
    {
      break;
    }

    (*o_property).m_port=
      GTK_SPIN_BUTTON(gtk_builder_get_object(io_builder, "email_port"));

    if (0 == (*o_property).m_port)
    {
      break;
    }

    l_exit= 0;

  }while(0);

  return l_exit;
}

static void
set(
  struct property*const                 io_property,
  struct email const*const              i_object)
{
  GtkTextBuffer*                        l_buffer;
  gchar const*                          l_text;

  l_text= ((*i_object).m_from ? (*i_object).m_from : "");
  gtk_entry_set_text((*io_property).m_from, l_text);

  l_text= ((*i_object).m_to ? (*i_object).m_to : "");
  gtk_entry_set_text((*io_property).m_to, l_text);

  l_text= ((*i_object).m_subject ? (*i_object).m_subject : "");
  gtk_entry_set_text((*io_property).m_subject, l_text);

  l_text= ((*i_object).m_message ? (*i_object).m_message : "");
  l_buffer= gtk_text_view_get_buffer((*io_property).m_message);
  gtk_text_buffer_set_text(l_buffer, l_text, -1);

  l_text= ((*i_object).m_server ? (*i_object).m_server : "");
  gtk_entry_set_text((*io_property).m_server, (*i_object).m_server);

  l_text= ((*i_object).m_username ? (*i_object).m_username : "");
  gtk_entry_set_text((*io_property).m_username, (*i_object).m_username);

  l_text= ((*i_object).m_password ? (*i_object).m_password : "");
  gtk_entry_set_text((*io_property).m_password, (*i_object).m_password);

  l_text= ((*i_object).m_port ? (*i_object).m_port : "");
  gtk_entry_set_text(GTK_ENTRY((*io_property).m_port), (*i_object).m_port);

  return;
}

static void
copy(
  struct email*const                    io_object,
  struct property const*const           i_property)
{
  GtkTextBuffer*                        l_buffer;
  GtkTextIter                           l_end;
  GtkTextIter                           l_start;
  gchar const*                          l_text;

  g_free((*io_object).m_from);
  l_text= gtk_entry_get_text((*i_property).m_from);
  (*io_object).m_from= g_strdup(l_text);

  g_free((*io_object).m_to);
  l_text= gtk_entry_get_text((*i_property).m_to);
  (*io_object).m_to= g_strdup(l_text);

  g_free((*io_object).m_subject);
  l_text= gtk_entry_get_text((*i_property).m_subject);
  (*io_object).m_subject= g_strdup(l_text);

  g_free((*io_object).m_message);
  l_buffer= gtk_text_view_get_buffer((*i_property).m_message);
  gtk_text_buffer_get_start_iter(l_buffer, &l_start);
  gtk_text_buffer_get_end_iter(l_buffer, &l_end);
  (*io_object).m_message= gtk_text_buffer_get_text(l_buffer, &l_start, &l_end, FALSE);

  g_free((*io_object).m_server);
  l_text= gtk_entry_get_text((*i_property).m_server);
  (*io_object).m_server= g_strdup(l_text);

  g_free((*io_object).m_username);
  l_text= gtk_entry_get_text((*i_property).m_username);
  (*io_object).m_username= g_strdup(l_text);

  g_free((*io_object).m_password);
  l_text= gtk_entry_get_text((*i_property).m_password);
  (*io_object).m_password= g_strdup(l_text);

  g_free((*io_object).m_port);
  l_text= gtk_entry_get_text(GTK_ENTRY((*i_property).m_port));
  (*io_object).m_port= g_strdup(l_text);

  return;
}

extern int
email_form(
  struct email*const                    io_object,
  struct session*const                  io_session,
  GtkWindow*const                       io_parent,
  GtkBuilder*const                      io_builder)
{
  GtkDialog*                            l_dialog;
  GError*                               l_error;
  int                                   l_exit;
  struct property*                      l_property;

  l_dialog= 0;
  l_error= 0;
  l_exit= -1;
  l_property= (struct property*)g_malloc0(sizeof(*l_property));

  do
  {

    l_dialog= GTK_DIALOG(gtk_builder_get_object(io_builder, "dialog_email"));

    if (0 == l_dialog)
    {
      l_error= g_error_new(
        domain_general,
        error_generic,
        "Unable to find dialog: '%s'",
        "dialog_email");
      break;
    }

    gtk_window_set_transient_for(GTK_WINDOW(l_dialog), io_parent);

    l_exit= bind(l_property, io_builder);

    if (l_exit)
    {
      l_error= g_error_new(
        domain_general,
        error_generic,
        "Unable to load dialog: '%s'",
        "dialog_email");
      break;
    }

    set(l_property, io_object);

    g_object_set_data(G_OBJECT(l_dialog), "builder", io_builder);
    g_object_set_data(G_OBJECT(l_dialog), "session", io_session);
    g_object_set_data(G_OBJECT(l_dialog), "property", l_property);

    gtk_widget_show_all(GTK_WIDGET(l_dialog));
    gtk_window_set_modal(GTK_WINDOW(l_dialog), 1);
    l_exit= gtk_dialog_run(l_dialog);

    if (GTK_RESPONSE_OK != l_exit)
    {
      break;
    }

    copy(io_object, l_property);
    l_exit= 0;

  }while(0);

  g_free(l_property);

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
