/*
  company_form.c

  License CC0 PUBLIC DOMAIN

  To the extent possible under law, Mark J. Olesen has waived all copyright
  and related or neighboring rights to tabitha. This work is published from:
  United States.
*/
#include "company.h"
#include "error.h"

struct property
{
  GtkEntry*                             m_company_name;
  GtkEntry*                             m_street1;
  GtkEntry*                             m_street2;
  GtkEntry*                             m_street3;
  GtkEntry*                             m_city;
  GtkEntry*                             m_state;
  GtkEntry*                             m_zipcode;
  GtkEntry*                             m_phone;
  GtkEntry*                             m_cellphone;
  GtkEntry*                             m_fax;
  GtkEntry*                             m_email;
  GtkEntry*                             m_website;
  GtkEntry*                             m_smtp_server;
  GtkEntry*                             m_smtp_username;
  GtkEntry*                             m_smtp_password;
  GtkSpinButton*                        m_smtp_port;
};

static void
bind(
  struct property*const                 o_property,
  GtkBuilder*const                      io_builder)
{

  memset(o_property, 0, sizeof(*o_property));

  do
  {

    (*o_property).m_company_name=
      GTK_ENTRY(gtk_builder_get_object(io_builder, "company_company_name"));

    (*o_property).m_street1=
      GTK_ENTRY(gtk_builder_get_object(io_builder, "company_street1"));

    (*o_property).m_street2=
      GTK_ENTRY(gtk_builder_get_object(io_builder, "company_street2"));

    (*o_property).m_street3=
      GTK_ENTRY(gtk_builder_get_object(io_builder, "company_street3"));

    (*o_property).m_city=
      GTK_ENTRY(gtk_builder_get_object(io_builder, "company_city"));

    (*o_property).m_state=
      GTK_ENTRY(gtk_builder_get_object(io_builder, "company_state"));

    (*o_property).m_zipcode=
      GTK_ENTRY(gtk_builder_get_object(io_builder, "company_zipcode"));

    (*o_property).m_phone=
      GTK_ENTRY(gtk_builder_get_object(io_builder, "company_phone"));

    (*o_property).m_cellphone=
      GTK_ENTRY(gtk_builder_get_object(io_builder, "company_cellphone"));

    (*o_property).m_fax=
      GTK_ENTRY(gtk_builder_get_object(io_builder, "company_fax"));

    (*o_property).m_email=
      GTK_ENTRY(gtk_builder_get_object(io_builder, "company_email"));

    (*o_property).m_website=
      GTK_ENTRY(gtk_builder_get_object(io_builder, "company_website"));

    (*o_property).m_smtp_server=
      GTK_ENTRY(gtk_builder_get_object(io_builder, "smtp_server"));

    (*o_property).m_smtp_username=
      GTK_ENTRY(gtk_builder_get_object(io_builder, "smtp_email"));

    (*o_property).m_smtp_password=
      GTK_ENTRY(gtk_builder_get_object(io_builder, "smtp_password"));

    (*o_property).m_smtp_port=
      GTK_SPIN_BUTTON(gtk_builder_get_object(io_builder, "smtp_port"));

  }while(0);

  return;
}

static void
set(
  struct property*const                 io_property,
  struct company const*const            i_object)
{

  gtk_entry_set_text((*io_property).m_company_name, (*i_object).m_company_name);
  gtk_entry_set_text((*io_property).m_street1, (*i_object).m_street1);
  gtk_entry_set_text((*io_property).m_street2, (*i_object).m_street2);
  gtk_entry_set_text((*io_property).m_street3, (*i_object).m_street3);
  gtk_entry_set_text((*io_property).m_city, (*i_object).m_city);
  gtk_entry_set_text((*io_property).m_state, (*i_object).m_state);
  gtk_entry_set_text((*io_property).m_zipcode, (*i_object).m_zipcode);
  gtk_entry_set_text((*io_property).m_phone, (*i_object).m_phone);
  gtk_entry_set_text((*io_property).m_cellphone, (*i_object).m_cellphone);
  gtk_entry_set_text((*io_property).m_fax, (*i_object).m_fax);
  gtk_entry_set_text((*io_property).m_email, (*i_object).m_email);
  gtk_entry_set_text((*io_property).m_website, (*i_object).m_website);
  gtk_entry_set_text((*io_property).m_smtp_server, (*i_object).m_smtp_server);
  gtk_entry_set_text((*io_property).m_smtp_username, (*i_object).m_smtp_username);
  gtk_entry_set_text((*io_property).m_smtp_password, (*i_object).m_smtp_password);
  gtk_entry_set_text(GTK_ENTRY((*io_property).m_smtp_port), (*i_object).m_smtp_port);

  return;
}

static void
fetch_and_set(
  struct property*const                 io_property,
  struct session*const                  io_session,
  GtkWindow*const                       io_parent)
{
  GError*                               l_error;
  int                                   l_exit;
  struct company                        l_company;

  memset(&l_company, 0, sizeof(l_company));
  l_error= 0;

  l_exit= company_fetch(&l_error, &l_company, io_session);

  if (0 == l_exit)
  {
    set(io_property, &l_company);
  }

  if (l_error)
  {
    _error_display(io_parent, l_error);
    g_clear_error(&l_error);
  }

  return;
}

static void
copy(
  struct company*const                   io_object,
  struct property const*const            i_property)
{
  gchar const*                          l_text;

  l_text= gtk_entry_get_text((*i_property).m_company_name);
  g_strlcpy((*io_object).m_company_name, l_text, sizeof((*io_object).m_company_name));

  l_text= gtk_entry_get_text((*i_property).m_street1);
  g_strlcpy((*io_object).m_street1, l_text, sizeof((*io_object).m_street1));

  l_text= gtk_entry_get_text((*i_property).m_street2);
  g_strlcpy((*io_object).m_street2, l_text, sizeof((*io_object).m_street2));

  l_text= gtk_entry_get_text((*i_property).m_street3);
  g_strlcpy((*io_object).m_street3, l_text, sizeof((*io_object).m_street3));

  l_text= gtk_entry_get_text((*i_property).m_city);
  g_strlcpy((*io_object).m_city, l_text, sizeof((*io_object).m_city));

  l_text= gtk_entry_get_text((*i_property).m_state);
  g_strlcpy((*io_object).m_state, l_text, sizeof((*io_object).m_state));

  l_text= gtk_entry_get_text((*i_property).m_zipcode);
  g_strlcpy((*io_object).m_zipcode, l_text, sizeof((*io_object).m_zipcode));

  l_text= gtk_entry_get_text((*i_property).m_phone);
  g_strlcpy((*io_object).m_phone, l_text, sizeof((*io_object).m_phone));

  l_text= gtk_entry_get_text((*i_property).m_cellphone);
  g_strlcpy((*io_object).m_cellphone, l_text, sizeof((*io_object).m_cellphone));

  l_text= gtk_entry_get_text((*i_property).m_fax);
  g_strlcpy((*io_object).m_fax, l_text, sizeof((*io_object).m_fax));

  l_text= gtk_entry_get_text((*i_property).m_email);
  g_strlcpy((*io_object).m_email, l_text, sizeof((*io_object).m_email));

  l_text= gtk_entry_get_text((*i_property).m_website);
  g_strlcpy((*io_object).m_website, l_text, sizeof((*io_object).m_website));

  l_text= gtk_entry_get_text((*i_property).m_smtp_server);
  g_strlcpy((*io_object).m_smtp_server, l_text, sizeof((*io_object).m_smtp_server));

  l_text= gtk_entry_get_text((*i_property).m_smtp_username);
  g_strlcpy((*io_object).m_smtp_username, l_text, sizeof((*io_object).m_smtp_username));

  l_text= gtk_entry_get_text((*i_property).m_smtp_password);
  g_strlcpy((*io_object).m_smtp_password, l_text, sizeof((*io_object).m_smtp_password));

  l_text= gtk_entry_get_text(GTK_ENTRY((*i_property).m_smtp_port));
  g_strlcpy((*io_object).m_smtp_port, l_text, sizeof((*io_object).m_smtp_port));

  company_trim(io_object);

  return;
}

G_MODULE_EXPORT void
on_company_save_button_clicked(
  GtkButton*                            io_button,
  gpointer                              io_user_data)
{
  struct company                        l_company;
  GtkDialog*                            l_dialog;
  GError*                               l_error;
  struct property*                      l_property;
  struct session*                       l_session;

  l_error= 0;
  l_dialog= GTK_DIALOG(GTK_WIDGET(io_user_data));
  l_session= (struct session*)g_object_get_data(G_OBJECT(l_dialog), "session");
  l_property= (struct property*)g_object_get_data(G_OBJECT(l_dialog), "property");

  memset(&l_company, 0, sizeof(l_company));
  copy(&l_company, l_property);

  company_update(&l_error, l_session, &l_company);

  if (l_error)
  {
    _error_display(GTK_WINDOW(l_dialog), l_error);
    g_clear_error(&l_error);
  }

  return;
}

extern int
company_form(
  struct session*const                  io_session,
  GtkWindow*const                       io_parent,
  GtkBuilder*const                      io_builder)
{
  GtkDialog*                            l_dialog;
  GError*                               l_error;
  int                                   l_exit;
  struct property                       l_property;

  l_dialog= 0;
  l_error= 0;
  l_exit= -1;

  do
  {

    l_dialog= GTK_DIALOG(gtk_builder_get_object(io_builder, "dialog_company"));

    if (0 == l_dialog)
    {
      l_error= g_error_new(
        domain_general,
        error_generic,
        "Unable to find dialog: '%s'",
        "dialog_company");
      break;
    }

    gtk_window_set_transient_for(GTK_WINDOW(l_dialog), io_parent);

    bind(&l_property, io_builder);
    fetch_and_set(&l_property, io_session, GTK_WINDOW(l_dialog));

    g_object_set_data(G_OBJECT(l_dialog), "builder", io_builder);
    g_object_set_data(G_OBJECT(l_dialog), "session", io_session);
    g_object_set_data(G_OBJECT(l_dialog), "property", &l_property);

    gtk_widget_show_all(GTK_WIDGET(l_dialog));
    gtk_window_set_modal(GTK_WINDOW(l_dialog), 1);

    gtk_dialog_run(l_dialog);
    l_exit= 0;

  }while(0);

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
