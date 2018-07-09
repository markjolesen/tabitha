/*
  aspect_form.c

  License CC0 PUBLIC DOMAIN

  To the extent possible under law, Mark J. Olesen has waived all copyright
  and related or neighboring rights to tabitha. This work is published from:
  United States.
*/
#include "aspect.h"
#include "error.h"

struct property
{
  GtkEntry*                             m_hostname;
  GtkEntry*                             m_ipaddress;
  GtkEntry*                             m_port;
  GtkEntry*                             m_username;
  GtkEntry*                             m_password;
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

    (*o_property).m_hostname=
      GTK_ENTRY(gtk_builder_get_object(io_builder, "aspect_hostname"));

    if (0 == (*o_property).m_hostname)
    {
      break;
    }

    (*o_property).m_ipaddress=
      GTK_ENTRY(gtk_builder_get_object(io_builder, "aspect_ip_address"));

    if (0 == (*o_property).m_ipaddress)
    {
      break;
    }

    (*o_property).m_port=
      GTK_ENTRY(gtk_builder_get_object(io_builder, "aspect_port"));

    if (0 == (*o_property).m_port)
    {
      break;
    }

    (*o_property).m_username=
      GTK_ENTRY(gtk_builder_get_object(io_builder, "aspect_username"));

    if (0 == (*o_property).m_username)
    {
      break;
    }

    (*o_property).m_password=
      GTK_ENTRY(gtk_builder_get_object(io_builder, "aspect_password"));

    if (0 == (*o_property).m_password)
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
  struct aspect const*const             i_object)
{

  gtk_entry_set_text((*io_property).m_hostname, (*i_object).m_hostname);
  gtk_entry_set_text((*io_property).m_ipaddress, (*i_object).m_ipaddress);
  gtk_entry_set_text((*io_property).m_port, (*i_object).m_port);
  gtk_entry_set_text((*io_property).m_username, (*i_object).m_username);
  gtk_entry_set_text((*io_property).m_password, (*i_object).m_password);

  return;
}

static void
copy(
  struct aspect*const                   io_object,
  struct property const*const           i_property)
{
  gchar const*                          l_text;

  memset(io_object, 0, sizeof(*io_object));

  l_text= gtk_entry_get_text((*i_property).m_hostname);
  g_strlcpy((*io_object).m_hostname, l_text, sizeof((*io_object).m_hostname));

  l_text= gtk_entry_get_text((*i_property).m_ipaddress);
  g_strlcpy((*io_object).m_ipaddress, l_text, sizeof((*io_object).m_ipaddress));

  l_text= gtk_entry_get_text((*i_property).m_port);
  g_strlcpy((*io_object).m_port, l_text, sizeof((*io_object).m_port));

  l_text= gtk_entry_get_text((*i_property).m_username);
  g_strlcpy((*io_object).m_username, l_text, sizeof((*io_object).m_username));

  l_text= gtk_entry_get_text((*i_property).m_password);
  g_strlcpy((*io_object).m_password, l_text, sizeof((*io_object).m_password));

  return;
}

extern int
aspect_form(
  struct aspect*const                   io_object,
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

    l_dialog= GTK_DIALOG(gtk_builder_get_object(io_builder, "dialog_aspect"));

    if (0 == l_dialog)
    {
      l_error= g_error_new(
        domain_general,
        error_generic,
        "Unable to find dialog: '%s'",
        "dialog_aspect");
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
        "dialog_aspect");
      break;
    }

    set(l_property, io_object);

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
