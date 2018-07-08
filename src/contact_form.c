/*
  contact_form.c

  License CC0 PUBLIC DOMAIN

  To the extent possible under law, Mark J. Olesen has waived all copyright
  and related or neighboring rights to tabitha. This work is published from:
  United States.
*/
#include "contact.h"
#include "error.h"
#include <gmodule.h>

struct property
{
  GtkEntry*                             m_contact_id;
  GtkComboBoxText*                      m_contact_type;
  GtkEntry*                             m_company_name;
  GtkEntry*                             m_first_name;
  GtkEntry*                             m_last_name;
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
  GtkTextView*                          m_notes;
  gchar                                 m_current_id[size_contact_id];
};

static void
bind(
  struct property*const                 o_property,
  GtkBuilder*const                      io_builder)
{

  memset(o_property, 0, sizeof(*o_property));

  do
  {

    (*o_property).m_contact_id=
      GTK_ENTRY(gtk_builder_get_object(io_builder, "contact_contact_id"));

    (*o_property).m_contact_type=
      GTK_COMBO_BOX_TEXT(gtk_builder_get_object(io_builder, "contact_contact_type"));

    (*o_property).m_company_name=
      GTK_ENTRY(gtk_builder_get_object(io_builder, "contact_company_name"));

    (*o_property).m_first_name=
      GTK_ENTRY(gtk_builder_get_object(io_builder, "contact_first_name"));

    (*o_property).m_last_name=
      GTK_ENTRY(gtk_builder_get_object(io_builder, "contact_last_name"));

    (*o_property).m_street1=
      GTK_ENTRY(gtk_builder_get_object(io_builder, "contact_street1"));

    (*o_property).m_street2=
      GTK_ENTRY(gtk_builder_get_object(io_builder, "contact_street2"));

    (*o_property).m_street3=
      GTK_ENTRY(gtk_builder_get_object(io_builder, "contact_street3"));

    (*o_property).m_city=
      GTK_ENTRY(gtk_builder_get_object(io_builder, "contact_city"));

    (*o_property).m_state=
      GTK_ENTRY(gtk_builder_get_object(io_builder, "contact_state"));

    (*o_property).m_zipcode=
      GTK_ENTRY(gtk_builder_get_object(io_builder, "contact_zipcode"));

    (*o_property).m_phone=
      GTK_ENTRY(gtk_builder_get_object(io_builder, "contact_phone"));

    (*o_property).m_cellphone=
      GTK_ENTRY(gtk_builder_get_object(io_builder, "contact_cellphone"));

    (*o_property).m_fax=
      GTK_ENTRY(gtk_builder_get_object(io_builder, "contact_fax"));

    (*o_property).m_email=
      GTK_ENTRY(gtk_builder_get_object(io_builder, "contact_email"));

    (*o_property).m_website=
      GTK_ENTRY(gtk_builder_get_object(io_builder, "contact_website"));

    (*o_property).m_notes=
      GTK_TEXT_VIEW(gtk_builder_get_object(io_builder, "contact_notes"));

  }while(0);

  return;
}

static void
set(
  struct property*const                 io_property,
  struct contact const*const            i_object)
{

  gtk_entry_set_text((*io_property).m_contact_id, (*i_object).m_contact_id);
  gtk_combo_box_set_active(GTK_COMBO_BOX((*io_property).m_contact_type), (*i_object).m_contact_type);
  gtk_entry_set_text((*io_property).m_company_name, (*i_object).m_company_name);
  gtk_entry_set_text((*io_property).m_first_name, (*i_object).m_first_name);
  gtk_entry_set_text((*io_property).m_last_name, (*i_object).m_last_name);
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

/*
  if ((*i_object).m_notes)
  {
    l_text= (*i_object).m_notes;
  }
  else
  {
    l_text= "";
  }

  gtk_entry_set_text((*io_property).m_notes, l_text);
*/

  return;
}

static void
copy(
  struct contact*const                  io_object,
  struct property const*const           i_property)
{
  GtkTextBuffer*                        l_buffer;
  GtkTextIter                           l_end;
  gint                                  l_rc;
  GtkTextIter                           l_start;
  gchar const*                          l_text;

  l_text= gtk_entry_get_text((*i_property).m_contact_id);
  g_strlcpy((*io_object).m_contact_id, l_text, sizeof((*io_object).m_contact_id));

  l_rc= gtk_combo_box_get_active(GTK_COMBO_BOX((*i_property).m_contact_type));
  (*io_object).m_contact_type= (enum contacttype)l_rc;

  l_text= gtk_entry_get_text((*i_property).m_company_name);
  g_strlcpy((*io_object).m_company_name, l_text, sizeof((*io_object).m_company_name));

  l_text= gtk_entry_get_text((*i_property).m_first_name);
  g_strlcpy((*io_object).m_first_name, l_text, sizeof((*io_object).m_first_name));

  l_text= gtk_entry_get_text((*i_property).m_last_name);
  g_strlcpy((*io_object).m_last_name, l_text, sizeof((*io_object).m_last_name));

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

  g_free((*io_object).m_notes);
  l_buffer= gtk_text_view_get_buffer((*i_property).m_notes);
  gtk_text_buffer_get_start_iter(l_buffer, &l_start);
  gtk_text_buffer_get_end_iter(l_buffer, &l_end);
  (*io_object).m_notes= gtk_text_buffer_get_text(l_buffer, &l_start, &l_end, FALSE);

  contact_trim(io_object);

  return;
}

G_MODULE_EXPORT void
on_contact_index_button_clicked(
  GtkButton*                            io_button,
  gpointer                              io_user_data)
{
  GtkBuilder*                           l_builder;
  struct contact                        l_contact;
  GtkDialog*                            l_dialog;
  GError*                               l_error;
  int                                   l_exit;
  struct property*                      l_property;
  struct session*                       l_session;
  gchar                                 l_contact_id[size_contact_id];

  memset(&l_contact, 0, sizeof(l_contact));
  memset(l_contact_id, 0, size_contact_id);
  l_error= 0;

  l_dialog= GTK_DIALOG(GTK_WIDGET(io_user_data));
  l_builder= GTK_BUILDER(g_object_get_data(G_OBJECT(l_dialog), "builder"));
  l_session= (struct session*)g_object_get_data(G_OBJECT(l_dialog), "session");
  l_property= (struct property*)g_object_get_data(G_OBJECT(l_dialog), "property");

  do
  {

    l_exit= contact_index_form(l_contact_id, l_session, GTK_WINDOW(l_dialog), l_builder);

    if (l_exit)
    {
      break;
    }

    l_exit= contact_fetch(&l_error, &l_contact, l_session, l_contact_id);    

    if (l_exit)
    {
      break;
    }

    set(l_property, &l_contact);

  }while(0);

  if (l_error)
  {
    _error_display(GTK_WINDOW(l_dialog), l_error);
    g_clear_error(&l_error);
  }

  return;
}

G_MODULE_EXPORT void
on_contact_new_button_clicked(
  GtkButton*                            io_button,
  gpointer                              io_user_data)
{
  struct contact                        l_contact;
  GtkDialog*                            l_dialog;
  struct property*                      l_property;

  l_dialog= GTK_DIALOG(GTK_WIDGET(io_user_data));
  l_property= (struct property*)g_object_get_data(G_OBJECT(l_dialog), "property");

  memset(&l_contact, 0, sizeof(l_contact));
  set(l_property, &l_contact);

  return;
}

G_MODULE_EXPORT void
on_contact_save_button_clicked(
  GtkButton*                            io_button,
  gpointer                              io_user_data)
{
  struct contact                        l_contact;
  GtkDialog*                            l_dialog;
  GError*                               l_error;
  struct property*                      l_property;
  struct session*                       l_session;

  l_error= 0;
  l_dialog= GTK_DIALOG(GTK_WIDGET(io_user_data));
  l_session= (struct session*)g_object_get_data(G_OBJECT(l_dialog), "session");
  l_property= (struct property*)g_object_get_data(G_OBJECT(l_dialog), "property");

  memset(&l_contact, 0, sizeof(l_contact));
  copy(&l_contact, l_property);

  do
  {

    if (0 == l_contact.m_contact_id[0])
    {
      l_error= g_error_new(
        domain_contact,
        error_contact_missing_id,
        "'Contact ID' is empty");
      break;
    }

    contact_save(&l_error, l_session, &l_contact);

  }while(0);

  if (l_error)
  {
    _error_display(GTK_WINDOW(l_dialog), l_error);
    g_clear_error(&l_error);
  }

  return;
}

G_MODULE_EXPORT gboolean
on_contact_contact_id_focus_out_event(
  GtkWidget*                            io_widget,
  GdkEvent*                             io_event,
  gpointer                              io_user_data)
{
  struct contact                        l_contact;
  GtkDialog*                            l_dialog;
  GError*                               l_error;
  int                                   l_exists;
  int                                   l_exit;
  struct property*                      l_property;
  int                                   l_rc;
  struct session*                       l_session;
  gchar const*                          l_text;
  gboolean                              l_visible;

  memset(&l_contact, 0, sizeof(l_contact));
  l_error= 0;
  l_dialog= GTK_DIALOG(GTK_WIDGET(io_user_data));

  do
  {

    l_visible= gtk_widget_get_visible(GTK_WIDGET(l_dialog));

    if (FALSE == l_visible)
    {
      break;
    }

    l_session= (struct session*)g_object_get_data(G_OBJECT(l_dialog), "session");
    l_property= (struct property*)g_object_get_data(G_OBJECT(l_dialog), "property");

    l_text= gtk_entry_get_text((*l_property).m_contact_id);
    g_strlcpy(l_contact.m_contact_id, l_text, sizeof(l_contact.m_contact_id));

    l_rc= g_strcmp0(l_contact.m_contact_id, (*l_property).m_current_id);

    if (0 == l_rc)
    {
      break;
    }

    l_exit= contact_exists(&l_error, &l_exists, l_session, l_contact.m_contact_id);

    if (l_exit)
    {
      break;
    }

    if (l_exists)
    {
      l_exit= contact_fetch(&l_error, &l_contact, l_session, l_contact.m_contact_id);
    }

    set(l_property, &l_contact);

  }while(0);

  if (l_error)
  {
    _error_display(GTK_WINDOW(l_dialog), l_error);
    g_clear_error(&l_error);
  }

  return GDK_EVENT_PROPAGATE;
}

extern int
contact_form(
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

    l_dialog= GTK_DIALOG(gtk_builder_get_object(io_builder, "dialog_contact"));

    if (0 == l_dialog)
    {
      l_error= g_error_new(
        domain_general,
        error_generic,
        "Unable to find dialog: '%s'",
        "dialog_contact");
      break;
    }

    gtk_window_set_transient_for(GTK_WINDOW(l_dialog), io_parent);

    bind(&l_property, io_builder);

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
