/*
  receipt_form.c

  License CC0 PUBLIC DOMAIN

  To the extent possible under law, Mark J. Olesen has waived all copyright
  and related or neighboring rights to tabitha. This work is published from:
  United States.
*/
#include "receipt.h"
#include "sales.h"
#include "calendar.h"
#include "error.h"
#include <gmodule.h>

struct property
{
  GtkEntry*                             m_receipt_id;
  GtkEntry*                             m_sales_id;
  GtkButton*                            m_received_date_button;
  GtkEntry*                             m_received_date;
  GtkEntry*                             m_amount;
  GtkEntry*                             m_reference;
  struct calendar                       m_cal_received_date;
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

    (*o_property).m_receipt_id=
      GTK_ENTRY(gtk_builder_get_object(io_builder, "receipt_receipt_id"));

    if (0 == (*o_property).m_receipt_id)
    {
      break;
    }

    (*o_property).m_sales_id=
      GTK_ENTRY(gtk_builder_get_object(io_builder, "receipt_sales_id"));

    if (0 == (*o_property).m_sales_id)
    {
      break;
    }

    (*o_property).m_received_date_button=
      GTK_BUTTON(gtk_builder_get_object(io_builder, "receipt_received_date_button"));

    if (0 == (*o_property).m_received_date_button)
    {
      break;
    }

    (*o_property).m_received_date=
      GTK_ENTRY(gtk_builder_get_object(io_builder, "receipt_received_date"));

    if (0 == (*o_property).m_received_date)
    {
      break;
    }

    (*o_property).m_amount=
      GTK_ENTRY(gtk_builder_get_object(io_builder, "receipt_amount"));

    if (0 == (*o_property).m_amount)
    {
      break;
    }

    (*o_property).m_reference=
      GTK_ENTRY(gtk_builder_get_object(io_builder, "receipt_reference"));

    if (0 == (*o_property).m_reference)
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
  struct receipt const*const            i_object)
{

  gtk_entry_set_text((*io_property).m_receipt_id, (*i_object).m_receipt_id);
  gtk_entry_set_text((*io_property).m_sales_id, (*i_object).m_sales_id);
  gtk_entry_set_text((*io_property).m_received_date, (*i_object).m_received_date);
  gtk_entry_set_text((*io_property).m_amount, (*i_object).m_amount);
  gtk_entry_set_text((*io_property).m_reference, (*i_object).m_reference);

  return;
}

static void
copy(
  struct receipt*const                  io_object,
  struct property const*const           i_property)
{
  gchar const*                          l_text;

  memset(io_object, 0, sizeof(*io_object));

  l_text= gtk_entry_get_text((*i_property).m_receipt_id);
  g_strlcpy((*io_object).m_receipt_id, l_text, sizeof((*io_object).m_receipt_id));

  l_text= gtk_entry_get_text((*i_property).m_sales_id);
  g_strlcpy((*io_object).m_sales_id, l_text, sizeof((*io_object).m_sales_id));

  l_text= gtk_entry_get_text((*i_property).m_received_date);
  g_strlcpy((*io_object).m_received_date, l_text, sizeof((*io_object).m_received_date));

  l_text= gtk_entry_get_text((*i_property).m_amount);
  g_strlcpy((*io_object).m_amount, l_text, sizeof((*io_object).m_amount));

  l_text= gtk_entry_get_text((*i_property).m_reference);
  g_strlcpy((*io_object).m_reference, l_text, sizeof((*io_object).m_reference));

  return;
}

static void
set_defaults(
  struct property*const                 io_property)
{
  gchar*                                l_text;

  gtk_entry_set_text((*io_property).m_receipt_id, "");
  gtk_entry_set_text((*io_property).m_sales_id, "");

  calendar_set_now(&(*io_property).m_cal_received_date); 
  l_text= calendar_format(&(*io_property).m_cal_received_date);
  gtk_entry_set_text((*io_property).m_received_date, l_text);
  g_free(l_text);

  gtk_entry_set_text((*io_property).m_amount, "");
  gtk_entry_set_text((*io_property).m_reference, "");

  return;
}

G_MODULE_EXPORT void
on_receipt_received_date_button_clicked(
  GtkButton*const                       io_button,
  gpointer                              io_user_data)
{
  GtkAllocation                         l_alloc;
  GtkBuilder*                           l_builder;
  GtkDialog*                            l_dialog;
  int                                   l_exit;
  gint                                  l_pos_x;
  gint                                  l_pos_y;
  struct property*                      l_property;
  gchar*                                l_text;

  l_text= 0;
  l_dialog= GTK_DIALOG(io_user_data);
  l_property= (struct property*)g_object_get_data(G_OBJECT(l_dialog), "property");
  l_builder= GTK_BUILDER(g_object_get_data(G_OBJECT(l_dialog), "builder"));

  gtk_window_get_position(GTK_WINDOW(l_dialog), &l_pos_x, &l_pos_y);
  gtk_widget_get_allocation(GTK_WIDGET((*l_property).m_received_date_button), &l_alloc);
  l_pos_x+= l_alloc.x;
  l_pos_y+= l_alloc.y;
  l_exit= calendar_form(&(*l_property).m_cal_received_date, GTK_WINDOW(l_dialog), l_builder, l_pos_x, l_pos_y);

  if (0 == l_exit)
  {
    l_text= calendar_format(&(*l_property).m_cal_received_date);
    gtk_entry_set_text((*l_property).m_received_date, l_text);
    g_free(l_text);
  }

  return;
}

G_MODULE_EXPORT void
on_receipt_sales_index_button_clicked(
  GtkButton*const                       io_button,
  gpointer                              io_user_data)
{
  GtkBuilder*                           l_builder;
  GtkDialog*                            l_dialog;
  GError*                               l_error;
  int                                   l_exit;
  struct property*                      l_property;
  gchar                                 l_sales_id[size_pg_big_int];
  struct session*                       l_session;

  memset(l_sales_id, 0, sizeof(l_sales_id));
  l_error= 0;

  l_dialog= GTK_DIALOG(GTK_WIDGET(io_user_data));
  l_session= (struct session*)g_object_get_data(G_OBJECT(l_dialog), "session");
  l_builder= GTK_BUILDER(g_object_get_data(G_OBJECT(l_dialog), "builder"));
  l_property= (struct property*)g_object_get_data(G_OBJECT(l_dialog), "property");

  do
  {

    l_exit= sales_index_form(l_sales_id, l_session, GTK_WINDOW(l_dialog), l_builder);

    if (l_exit)
    {
      break;
    }

    gtk_entry_set_text((*l_property).m_sales_id, l_sales_id);

  }while(0);

  return;
}

G_MODULE_EXPORT void
on_receipt_save_button_clicked(
  GtkButton*const                       io_button,
  gpointer                              io_user_data)
{
  GtkDialog*                            l_dialog;
  GError*                               l_error;
  int                                   l_exit;
  struct property*                      l_property;
  struct receipt*                       l_receipt;
  struct session*                       l_session;

  l_receipt= (struct receipt*)g_malloc0(sizeof(*l_receipt));
  l_error= 0;
  l_dialog= GTK_DIALOG(GTK_WIDGET(io_user_data));
  l_session= (struct session*)g_object_get_data(G_OBJECT(l_dialog), "session");
  l_property= (struct property*)g_object_get_data(G_OBJECT(l_dialog), "property");

  copy(l_receipt, l_property);
  receipt_trim(l_receipt);

  do
  {

    if (0 == (*l_receipt).m_sales_id[0])
    {
      l_error= g_error_new(
        domain_receipt,
        error_receipt_missing_id,
        "'Sales ID' is empty");
      break;
    }

    if (0 == (*l_receipt).m_received_date[0])
    {
      l_error= g_error_new(
        domain_receipt,
        error_receipt_missing_amount,
        "'Amount' is empty");
      break;
    }

    if (0 == (*l_receipt).m_amount[0])
    {
      l_error= g_error_new(
        domain_receipt,
        error_receipt_missing_amount,
        "'Amount' is empty");
      break;
    }

    if (0 == (*l_receipt).m_receipt_id[0])
    {
      l_exit= receipt_insert(&l_error, (*l_receipt).m_receipt_id, l_session, l_receipt);
      if (0 == l_exit)
      {
        gtk_entry_set_text((*l_property).m_receipt_id, (*l_receipt).m_receipt_id);
      }
    }
    else
    {
      l_exit= receipt_update(&l_error, l_session, l_receipt);
    }

  }while(0);

  g_free(l_receipt);

  if (l_error)
  {
    _error_display(GTK_WINDOW(l_dialog), l_error);
    g_clear_error(&l_error);
  }

  return;
}

G_MODULE_EXPORT void
on_receipt_new_button_clicked(
  GtkButton*const                       io_button,
  gpointer                              io_user_data)
{
  GtkDialog*                            l_dialog;
  struct property*                      l_property;

  l_dialog= GTK_DIALOG(io_user_data);
  l_property= (struct property*)g_object_get_data(G_OBJECT(l_dialog), "property");
  set_defaults(l_property);

  return;
}

G_MODULE_EXPORT void
on_receipt_index_button_clicked(
  GtkButton*const                       io_button,
  gpointer                              io_user_data)
{
  GtkBuilder*                           l_builder;
  GtkDialog*                            l_dialog;
  GError*                               l_error;
  int                                   l_exit;
  struct property*                      l_property;
  struct receipt*                       l_receipt;
  gchar                                 l_receipt_id[size_pg_big_int];
  struct session*                       l_session;

  l_receipt= (struct receipt*)g_malloc0(sizeof(*l_receipt));
  memset(l_receipt_id, 0, size_pg_big_int);
  l_error= 0;

  l_dialog= GTK_DIALOG(GTK_WIDGET(io_user_data));
  l_builder= GTK_BUILDER(g_object_get_data(G_OBJECT(l_dialog), "builder"));
  l_session= (struct session*)g_object_get_data(G_OBJECT(l_dialog), "session");
  l_property= (struct property*)g_object_get_data(G_OBJECT(l_dialog), "property");

  do
  {

    l_exit= receipt_index_form(l_receipt_id, l_session, GTK_WINDOW(l_dialog), l_builder);

    if (l_exit)
    {
      break;
    }

    l_exit= receipt_fetch(&l_error, l_receipt, l_session, l_receipt_id);    

    if (l_exit)
    {
      break;
    }

    set(l_property, l_receipt);

  }while(0);

  g_free(l_receipt);

  if (l_error)
  {
    _error_display(GTK_WINDOW(l_dialog), l_error);
    g_clear_error(&l_error);
  }

  return;
}

extern int
receipt_form(
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

    l_dialog= GTK_DIALOG(gtk_builder_get_object(io_builder, "dialog_receipt"));

    if (0 == l_dialog)
    {
      l_error= g_error_new(
        domain_general,
        error_generic,
        "Unable to find dialog: '%s'",
        "dialog_receipt");
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
        "dialog_receipt");
      break;
    }

    set_defaults(l_property);

    g_object_set_data(G_OBJECT(l_dialog), "builder", io_builder);
    g_object_set_data(G_OBJECT(l_dialog), "session", io_session);
    g_object_set_data(G_OBJECT(l_dialog), "property", &l_property);

    gtk_widget_show_all(GTK_WIDGET(l_dialog));
    gtk_window_set_modal(GTK_WINDOW(l_dialog), 1);

    gtk_dialog_run(l_dialog);
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
