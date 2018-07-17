/*
  sales_detail_form.c

  License CC0 PUBLIC DOMAIN

  To the extent possible under law, Mark J. Olesen has waived all copyright
  and related or neighboring rights to tabitha. This work is published from:
  United States.
*/
#include "sales.h"
#include "edit.h"
#include "error.h"
#include <gmodule.h>

struct property
{
  GtkEntry*                             m_product_id;
  GtkEntry*                             m_description;
  GtkSpinButton*                        m_quantity;
  GtkEntry*                             m_unit_price;
  gchar                                 m_current_id[size_product_id];
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

    (*o_property).m_product_id=
      GTK_ENTRY(gtk_builder_get_object(io_builder, "sales_detail_product_id"));

    if (0 == (*o_property).m_product_id)
    {
      break;
    }

    (*o_property).m_description=
      GTK_ENTRY(gtk_builder_get_object(io_builder, "sales_detail_description"));

    if (0 == (*o_property).m_description)
    {
      break;
    }

    (*o_property).m_quantity=
      GTK_SPIN_BUTTON(gtk_builder_get_object(io_builder, "sales_detail_quantity"));

    if (0 == (*o_property).m_quantity)
    {
      break;
    }

    (*o_property).m_unit_price=
      GTK_ENTRY(gtk_builder_get_object(io_builder, "sales_detail_unit_price"));

    if (0 == (*o_property).m_unit_price)
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
  struct sales_detail const*const       i_object)
{

  gtk_entry_set_text((*io_property).m_product_id, (*i_object).m_product_id);
  gtk_entry_set_text((*io_property).m_description, (*i_object).m_description);
  if ((*i_object).m_quantity[0])
  {
    gtk_entry_set_text(GTK_ENTRY((*io_property).m_quantity), (*i_object).m_quantity);
  }
  gtk_entry_set_text((*io_property).m_unit_price, (*i_object).m_unit_price);

  return;
}

static void
set_product(
  struct property*const                 io_property,
  struct product const*const            i_object)
{

  gtk_entry_set_text((*io_property).m_product_id, (*i_object).m_product_id);
  gtk_entry_set_text((*io_property).m_description, (*i_object).m_description);
  gtk_entry_set_text((*io_property).m_unit_price, (*i_object).m_unit_price);

  return;
}

static void
copy(
  struct sales_detail*const             io_object,
  struct property const*const           i_property)
{
  gchar const*                          l_text;

  memset(io_object, 0, sizeof(*io_object));

  l_text= gtk_entry_get_text((*i_property).m_product_id);
  g_strlcpy((*io_object).m_product_id, l_text, sizeof((*io_object).m_product_id));

  l_text= gtk_entry_get_text((*i_property).m_description);
  g_strlcpy((*io_object).m_description, l_text, sizeof((*io_object).m_description));

  l_text= gtk_entry_get_text(GTK_ENTRY((*i_property).m_quantity));
  g_strlcpy((*io_object).m_quantity, l_text, sizeof((*io_object).m_quantity));

  l_text= gtk_entry_get_text((*i_property).m_unit_price);
  g_strlcpy((*io_object).m_unit_price, l_text, sizeof((*io_object).m_unit_price));

  return;
}

G_MODULE_EXPORT gboolean
on_sales_detail_product_id_focus_out_event(
  G_GNUC_UNUSED GtkWidget*              io_widget,
  G_GNUC_UNUSED GdkEvent*               io_event,
  gpointer                              io_user_data)
{
  GtkDialog*                            l_dialog;
  GError*                               l_error;
  int                                   l_exists;
  int                                   l_exit;
  struct product*                       l_product;
  struct property*                      l_property;
  int                                   l_rc;
  struct session*                       l_session;
  gchar const*                          l_text;
  gboolean                              l_visible;

  l_product= (struct product*)g_malloc0(sizeof(*l_product));
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

    l_text= gtk_entry_get_text((*l_property).m_product_id);
    g_strlcpy((*l_product).m_product_id, l_text, sizeof((*l_product).m_product_id));

    l_rc= g_strcmp0((*l_product).m_product_id, (*l_property).m_current_id);

    if (0 == l_rc)
    {
      break;
    }

    l_exit= product_exists(&l_error, &l_exists, l_session, (*l_product).m_product_id);

    if (l_exit || FALSE == l_exists)
    {
      break;
    }

    l_exit= product_fetch(&l_error, l_product, l_session, (*l_product).m_product_id);

    if (l_exit)
    {
      break;
    }

    set_product(l_property, l_product);

  }while(0);

  g_free(l_product);

  if (l_error)
  {
    _error_display(GTK_WINDOW(l_dialog), l_error);
    g_clear_error(&l_error);
  }

  return GDK_EVENT_PROPAGATE;
}

G_MODULE_EXPORT void
on_sales_detail_product_id_index_button_clicked(
  G_GNUC_UNUSED GtkButton*const         io_button,
  gpointer                              io_user_data)
{
  GtkBuilder*                           l_builder;
  GtkDialog*                            l_dialog;
  GError*                               l_error;
  int                                   l_exit;
  struct product*                       l_product;
  struct property*                      l_property;
  struct session*                       l_session;

  l_product= (struct product*)g_malloc0(sizeof(*l_product));
  l_error= 0;

  l_dialog= GTK_DIALOG(io_user_data);
  l_session= (struct session*)g_object_get_data(G_OBJECT(l_dialog), "session");
  l_property= (struct property*)g_object_get_data(G_OBJECT(l_dialog), "property");
  l_builder= GTK_BUILDER(g_object_get_data(G_OBJECT(l_dialog), "builder"));

  do 
  {

    l_exit= product_index_form((*l_product).m_product_id, l_session, GTK_WINDOW(l_dialog), l_builder);

    if (l_exit)
    {
      break;
    }

    l_exit= product_fetch(&l_error, l_product, l_session, (*l_product).m_product_id);

    if (l_exit)
    {
      break;
    }

    set_product(l_property, l_product);

  }while(0);

  g_free(l_product);

  if (l_error)
  {
    _error_display(GTK_WINDOW(l_dialog), l_error);
    g_clear_error(&l_error);
  }

  return;
}

extern int
sales_detail_form(
  struct sales_detail*const             io_object,
  struct session*const                  io_session,
  GtkWindow*const                       io_parent,
  GtkBuilder*const                      io_builder)
{
  GtkDialog*                            l_dialog;
  GError*                               l_error;
  int                                   l_exit;
  gboolean                              l_exists;
  struct property*                      l_property;

  l_dialog= 0;
  l_error= 0;
  l_exit= -1;
  l_property= (struct property*)g_malloc0(sizeof(*l_property));

  do
  {

    l_dialog= GTK_DIALOG(gtk_builder_get_object(io_builder, "dialog_sales_detail"));

    if (0 == l_dialog)
    {
      l_error= g_error_new(
        domain_general,
        error_generic,
        "Unable to find dialog: '%s'",
        "dialog_sales_detail");
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
        "dialog_sales_detail");
      break;
    }

    set(l_property, io_object);

    g_object_set_data(G_OBJECT(l_dialog), "session", io_session);
    g_object_set_data(G_OBJECT(l_dialog), "property", l_property);
    g_object_set_data(G_OBJECT(l_dialog), "builder", io_builder);

    gtk_widget_show_all(GTK_WIDGET(l_dialog));
    gtk_window_set_modal(GTK_WINDOW(l_dialog), 1);

    do
    {

      l_exit= gtk_dialog_run(l_dialog);

      if (GTK_RESPONSE_OK != l_exit)
      {
        break;
      }

      copy(io_object, l_property);

      if (0 == (*io_object).m_product_id[0])
      {
        l_exit= 0;
        break;
      }

      l_exit= product_exists(&l_error, &l_exists, io_session, (*io_object).m_product_id);

      if ((0 == l_exit) && l_exists)
      {
        break;
      }

      l_error= g_error_new(
        domain_sales_detail,
        error_sales_detail_form_product_does_not_exist,
        "Product ID '%s' does not exist",
        (*io_object).m_product_id);   

      _error_display(io_parent, l_error);
      g_clear_error(&l_error);

    }while(1);
  
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
