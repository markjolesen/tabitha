/*
  sales_form.c

  License CC0 PUBLIC DOMAIN

  To the extent possible under law, Mark J. Olesen has waived all copyright
  and related or neighboring rights to tabitha. This work is published from:
  United States.
*/
#include "sales.h"
#include "edit.h"
#include "calendar.h"
#include "error.h"
#include <gmodule.h>

enum
{
  store_col_line_number_hidden          = 0,
  store_col_line_number,
  store_col_product_id,
  store_col_description,
  store_col_quantity,
  store_col_unit_price
};

struct property
{
  GtkEntry*                             m_sales_id;
  GtkButton*                            m_sales_date_button;
  GtkEntry*                             m_sales_date;
  GtkComboBoxText*                      m_sales_type;
  GtkEntry*                             m_billing_id;
  GtkTextView*                          m_service_contact;
  GtkEntry*                             m_start_date;
  GtkButton*                            m_start_date_button;
  GtkEntry*                             m_completed_date;
  GtkButton*                            m_completed_date_button;
  GtkEntry*                             m_due_date;
  GtkButton*                            m_due_date_button;
  GtkEntry*                             m_customer_po;
  GtkEntry*                             m_discount;
  GtkTextView*                          m_notes;
  GtkTextView*                          m_notes_internal;
  GtkTreeView*                          m_treeview;
  GtkListStore*                         m_liststore;
  struct calendar                       m_cal_sales_date;
  struct calendar                       m_cal_start_date;
  struct calendar                       m_cal_completed_date;
  struct calendar                       m_cal_due_date;
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

    (*o_property).m_sales_id=
      GTK_ENTRY(gtk_builder_get_object(io_builder, "sales_sales_id"));

    if (0 == (*o_property).m_sales_id)
    {
      break;
    }

    (*o_property).m_sales_type=
      GTK_COMBO_BOX_TEXT(gtk_builder_get_object(io_builder, "sales_sales_type"));

    if (0 == (*o_property).m_sales_type)
    {
      break;
    }

    (*o_property).m_sales_date_button=
      GTK_BUTTON(gtk_builder_get_object(io_builder, "sales_sales_date_button"));

    if (0 == (*o_property).m_sales_date_button)
    {
      break;
    }

    (*o_property).m_billing_id=
      GTK_ENTRY(gtk_builder_get_object(io_builder, "sales_billing_id"));

    if (0 == (*o_property).m_billing_id)
    {
      break;
    }

    (*o_property).m_service_contact=
      GTK_TEXT_VIEW(gtk_builder_get_object(io_builder, "sales_service_contact"));

    if (0 == (*o_property).m_service_contact)
    {
      break;
    }

    (*o_property).m_sales_date=
      GTK_ENTRY(gtk_builder_get_object(io_builder, "sales_sales_date"));

    if (0 == (*o_property).m_sales_date)
    {
      break;
    }

    (*o_property).m_start_date=
      GTK_ENTRY(gtk_builder_get_object(io_builder, "sales_start_date"));

    if (0 == (*o_property).m_start_date)
    {
      break;
    }

    (*o_property).m_start_date_button=
      GTK_BUTTON(gtk_builder_get_object(io_builder, "sales_start_date_button"));

    if (0 == (*o_property).m_start_date_button)
    {
      break;
    }

    (*o_property).m_completed_date=
      GTK_ENTRY(gtk_builder_get_object(io_builder, "sales_completed_date"));

    if (0 == (*o_property).m_completed_date)
    {
      break;
    }

    (*o_property).m_completed_date_button=
      GTK_BUTTON(gtk_builder_get_object(io_builder, "sales_completed_date_button"));

    if (0 == (*o_property).m_completed_date_button)
    {
      break;
    }

    (*o_property).m_due_date=
      GTK_ENTRY(gtk_builder_get_object(io_builder, "sales_due_date"));

    if (0 == (*o_property).m_due_date)
    {
      break;
    }

    (*o_property).m_due_date_button=
      GTK_BUTTON(gtk_builder_get_object(io_builder, "sales_due_date_button"));

    if (0 == (*o_property).m_due_date_button)
    {
      break;
    }

    (*o_property).m_customer_po=
      GTK_ENTRY(gtk_builder_get_object(io_builder, "sales_customer_po"));

    if (0 == (*o_property).m_customer_po)
    {
      break;
    }

    (*o_property).m_discount=
      GTK_ENTRY(gtk_builder_get_object(io_builder, "sales_discount"));

    if (0 == (*o_property).m_discount)
    {
      break;
    }

    (*o_property).m_notes=
      GTK_TEXT_VIEW(gtk_builder_get_object(io_builder, "sales_notes"));

    if (0 == (*o_property).m_notes)
    {
      break;
    }

    (*o_property).m_notes_internal=
      GTK_TEXT_VIEW(gtk_builder_get_object(io_builder, "sales_notes_internal"));

    if (0 == (*o_property).m_notes_internal)
    {
      break;
    }

    (*o_property).m_treeview=
      GTK_TREE_VIEW(gtk_builder_get_object(io_builder, "sales_detail_treeview"));

    if (0 == (*o_property).m_treeview)
    {
      break;
    }

    (*o_property).m_liststore=
      GTK_LIST_STORE(gtk_builder_get_object(io_builder, "sales_detail_liststore"));

    if (0 == (*o_property).m_liststore)
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
  struct sales const*const              i_object)
{
  GtkTextBuffer*                        l_buffer;

  gtk_entry_set_text((*io_property).m_sales_id, (*i_object).m_sales_id);
  gtk_combo_box_set_active(GTK_COMBO_BOX((*io_property).m_sales_type), (*i_object).m_sales_type);
  gtk_entry_set_text((*io_property).m_sales_date, (*i_object).m_sales_date);
  gtk_entry_set_text((*io_property).m_billing_id, (*i_object).m_billing_id);
  l_buffer= gtk_text_view_get_buffer((*io_property).m_service_contact);
  gtk_text_buffer_set_text(l_buffer, (*i_object).m_service_contact, -1);
  gtk_entry_set_text((*io_property).m_start_date, (*i_object).m_start_date);
  gtk_entry_set_text((*io_property).m_completed_date, (*i_object).m_completed_date);
  gtk_entry_set_text((*io_property).m_due_date, (*i_object).m_due_date);
  gtk_entry_set_text((*io_property).m_customer_po, (*i_object).m_customer_po);
  gtk_entry_set_text((*io_property).m_discount, (*i_object).m_discount);
  l_buffer= gtk_text_view_get_buffer((*io_property).m_notes);
  gtk_text_buffer_set_text(l_buffer, (*i_object).m_notes, -1);
  l_buffer= gtk_text_view_get_buffer((*io_property).m_notes_internal);
  gtk_text_buffer_set_text(l_buffer, (*i_object).m_notes_internal, -1);

  return;
}

static void
copy(
  struct sales*const                    io_object,
  struct property const*const           i_property)
{
  GtkTextBuffer*                        l_buffer;
  GtkTextIter                           l_end;
  GtkTextIter                           l_start;
  gchar const*                          l_text;

  memset(io_object, 0, sizeof(*io_object));

  l_text= gtk_entry_get_text((*i_property).m_sales_id);
  g_strlcpy((*io_object).m_sales_id, l_text, sizeof((*io_object).m_sales_id));

  (*io_object).m_sales_type= gtk_combo_box_get_active(GTK_COMBO_BOX((*i_property).m_sales_type));

  l_text= gtk_entry_get_text((*i_property).m_sales_date);
  g_strlcpy((*io_object).m_sales_date, l_text, sizeof((*io_object).m_sales_date));

  l_text= gtk_entry_get_text((*i_property).m_billing_id);
  g_strlcpy((*io_object).m_billing_id, l_text, sizeof((*io_object).m_billing_id));

  g_free((*io_object).m_service_contact);
  l_buffer= gtk_text_view_get_buffer((*i_property).m_service_contact);
  gtk_text_buffer_get_start_iter(l_buffer, &l_start);
  gtk_text_buffer_get_end_iter(l_buffer, &l_end);
  (*io_object).m_service_contact= gtk_text_buffer_get_text(l_buffer, &l_start, &l_end, FALSE);

  l_text= gtk_entry_get_text((*i_property).m_start_date);
  g_strlcpy((*io_object).m_start_date, l_text, sizeof((*io_object).m_start_date));

  l_text= gtk_entry_get_text((*i_property).m_completed_date);
  g_strlcpy((*io_object).m_completed_date, l_text, sizeof((*io_object).m_completed_date));

  l_text= gtk_entry_get_text((*i_property).m_due_date);
  g_strlcpy((*io_object).m_due_date, l_text, sizeof((*io_object).m_due_date));

  l_text= gtk_entry_get_text((*i_property).m_customer_po);
  g_strlcpy((*io_object).m_customer_po, l_text, sizeof((*io_object).m_customer_po));

  l_text= gtk_entry_get_text((*i_property).m_discount);
  g_strlcpy((*io_object).m_discount, l_text, sizeof((*io_object).m_discount));

  g_free((*io_object).m_notes);
  l_buffer= gtk_text_view_get_buffer((*i_property).m_notes);
  gtk_text_buffer_get_start_iter(l_buffer, &l_start);
  gtk_text_buffer_get_end_iter(l_buffer, &l_end);
  (*io_object).m_notes= gtk_text_buffer_get_text(l_buffer, &l_start, &l_end, FALSE);

  g_free((*io_object).m_notes_internal);
  l_buffer= gtk_text_view_get_buffer((*i_property).m_notes_internal);
  gtk_text_buffer_get_start_iter(l_buffer, &l_start);
  gtk_text_buffer_get_end_iter(l_buffer, &l_end);
  (*io_object).m_notes_internal= gtk_text_buffer_get_text(l_buffer, &l_start, &l_end, FALSE);

  sales_trim(io_object);

  return;
}

static void
set_defaults(
  struct property*const                 io_property)
{
  GtkTextBuffer*                        l_buffer;
  GtkTextIter                           l_end;
  GtkTextIter                           l_start;
  gchar*                                l_text;

  gtk_entry_set_text((*io_property).m_sales_id, "");

  calendar_set_now(&(*io_property).m_cal_sales_date); 
  l_text= calendar_format(&(*io_property).m_cal_sales_date);
  gtk_entry_set_text((*io_property).m_sales_date, l_text);
  g_free(l_text);

  gtk_combo_box_set_active(GTK_COMBO_BOX((*io_property).m_sales_type), 0);
  gtk_entry_set_text((*io_property).m_billing_id, "");

  l_buffer= gtk_text_view_get_buffer((*io_property).m_service_contact);
  gtk_text_buffer_get_start_iter(l_buffer, &l_start);
  gtk_text_buffer_get_end_iter(l_buffer, &l_end);
  gtk_text_buffer_delete(l_buffer, &l_start, &l_end);

  gtk_entry_set_text((*io_property).m_start_date, "");
  gtk_entry_set_text((*io_property).m_completed_date, "");
  gtk_entry_set_text((*io_property).m_due_date, "");
  gtk_entry_set_text((*io_property).m_customer_po, "");
  gtk_entry_set_text((*io_property).m_discount, "");

  l_buffer= gtk_text_view_get_buffer((*io_property).m_notes);
  gtk_text_buffer_get_start_iter(l_buffer, &l_start);
  gtk_text_buffer_get_end_iter(l_buffer, &l_end);
  gtk_text_buffer_delete(l_buffer, &l_start, &l_end);

  l_buffer= gtk_text_view_get_buffer((*io_property).m_notes_internal);
  gtk_text_buffer_get_start_iter(l_buffer, &l_start);
  gtk_text_buffer_get_end_iter(l_buffer, &l_end);
  gtk_text_buffer_delete(l_buffer, &l_start, &l_end);

  gtk_list_store_clear((*io_property).m_liststore);

  memset(&(*io_property).m_cal_start_date, 0, sizeof((*io_property).m_cal_start_date));
  memset(&(*io_property).m_cal_completed_date, 0, sizeof((*io_property).m_cal_completed_date));
  memset(&(*io_property).m_cal_due_date, 0, sizeof((*io_property).m_cal_due_date));

  return;
}

static int
save(
  GtkDialog*const                       io_dialog)
{
  gboolean                              l_bool;
  gchar*                                l_description;
  struct sales_detail*                  l_detail;
  GError*                               l_error;
  int                                   l_exit;
  GtkTreeIter                           l_iter;
  GtkTreeModel*                         l_model;
  gchar*                                l_product_id;
  struct property*                      l_property;
  gchar*                                l_quantity;
  struct sales*                         l_sales;
  struct session*                       l_session;
  gchar*                                l_unit_price;
  gchar*                                l_line_number_hidden;
  gchar*                                l_line_number;

  l_detail= (struct sales_detail*)g_malloc0(sizeof(*l_detail));
  l_sales= (struct sales*)g_malloc0(sizeof(*l_sales));
  l_description= 0;
  l_product_id= 0;
  l_quantity= 0;
  l_unit_price= 0;
  l_line_number_hidden= 0;
  l_line_number= 0;

  l_error= 0;
  l_property= (struct property*)g_object_get_data(G_OBJECT(io_dialog), "property");
  l_session= (struct session*)g_object_get_data(G_OBJECT(io_dialog), "session");

  copy(l_sales, l_property);
  sales_trim(l_sales);

  do
  {

    if (0 == (*l_sales).m_sales_id[0])
    {
      l_exit= sales_insert(&l_error, (*l_sales).m_sales_id, l_session, l_sales);
      if (0 == l_exit)
      {
        gtk_entry_set_text((*l_property).m_sales_id, (*l_sales).m_sales_id);
      }
    }
    else
    {
      l_exit= sales_update(&l_error, l_session, l_sales);
    }

    if (l_exit)
    {
      break;
    }

    l_model= gtk_tree_view_get_model((*l_property).m_treeview);

    if (0 == l_model)
    {
      break;
    }

    l_bool= gtk_tree_model_get_iter_first(l_model, &l_iter);

    do
    {

      if (FALSE == l_bool)
      {
        break;
      }

      gtk_tree_model_get(
        l_model,
        &l_iter,
        store_col_line_number_hidden, &l_line_number_hidden,
        store_col_line_number, &l_line_number,
        store_col_product_id, &l_product_id,
        store_col_description, &l_description,
        store_col_quantity, &l_quantity,
        store_col_unit_price, &l_unit_price,
        -1);

      g_strlcpy((*l_detail).m_line_number, l_line_number, sizeof((*l_detail).m_line_number));
      g_strlcpy((*l_detail).m_product_id, l_product_id, sizeof((*l_detail).m_product_id));
      g_strlcpy((*l_detail).m_description, l_description, sizeof((*l_detail).m_description));
      g_strlcpy((*l_detail).m_quantity, l_quantity, sizeof((*l_detail).m_quantity));
      g_strlcpy((*l_detail).m_unit_price, l_unit_price, sizeof((*l_detail).m_unit_price));
      sales_detail_trim(l_detail);

      if (0 == l_line_number_hidden[0])
      {
        l_exit= sales_detail_insert(&l_error, l_session, (*l_sales).m_sales_id, l_detail);
      }
      else
      {
        l_exit= sales_detail_update(&l_error, l_session, (*l_sales).m_sales_id, l_line_number_hidden, l_detail);
      }

      gtk_list_store_set(
        (*l_property).m_liststore,
        &l_iter,
        store_col_line_number_hidden, l_line_number,
        -1);

      if (l_error)
      {
        _error_display(GTK_WINDOW(io_dialog), l_error);
        g_clear_error(&l_error);
      }

      l_bool= gtk_tree_model_iter_next(l_model, &l_iter);

    }while(1);

  }while(0);

  g_free(l_detail);
  g_free(l_sales);
  g_free(l_description);
  g_free(l_product_id);
  g_free(l_quantity);
  g_free(l_unit_price);
  g_free(l_line_number_hidden);
  g_free(l_line_number);

  if (l_error)
  {
    _error_display(GTK_WINDOW(io_dialog), l_error);
    g_clear_error(&l_error);
  }

  return l_exit;
}

G_MODULE_EXPORT void
on_sales_print_button_clicked(
  G_GNUC_UNUSED GtkToolButton*const     io_button,
  gpointer                              io_user_data)
{
  GtkDialog*                            l_dialog;
  int                                   l_exit;
  struct property*                      l_property;
  gchar                                 l_sales_id[size_pg_big_int];
  struct session*                       l_session;
  gchar const*                          l_text;

  memset(l_sales_id, 0, sizeof(l_sales_id));

  l_dialog= GTK_DIALOG(GTK_WIDGET(io_user_data));
  l_session= (struct session*)g_object_get_data(G_OBJECT(l_dialog), "session");
  l_property= (struct property*)g_object_get_data(G_OBJECT(l_dialog), "property");

  do
  {

    l_exit= save(l_dialog);

    if (l_exit)
    {
      break;
    }

    l_text= gtk_entry_get_text((*l_property).m_sales_id);
    g_strlcpy(l_sales_id, l_text, sizeof(l_sales_id));
    sales_print(l_session, GTK_WINDOW(l_dialog), l_sales_id);

  }while(0);

  return;
}

G_MODULE_EXPORT void
on_sales_new_button_clicked(
  G_GNUC_UNUSED GtkButton*const         io_button,
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
on_sales_index_button_clicked(
  G_GNUC_UNUSED GtkButton*const         io_button,
  gpointer                              io_user_data)
{
  GtkBuilder*                           l_builder;
  struct cursor                         l_cursor;
  struct sales_detail*                  l_detail;
  GtkDialog*                            l_dialog;
  gboolean                              l_eof;
  GError*                               l_error;
  int                                   l_exit;
  GtkTreeIter                           l_iter;
  struct property*                      l_property;
  struct sales*                         l_sales;
  gchar                                 l_sales_id[size_pg_big_int];
  struct session*                       l_session;

  l_detail= (struct sales_detail*)g_malloc0(sizeof(*l_detail));
  l_sales= (struct sales*)g_malloc0(sizeof(*l_sales));
  cursor_assign(&l_cursor);
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

    set_defaults(l_property);

    l_exit= sales_fetch(&l_error, l_sales, l_session, l_sales_id);    

    if (l_exit)
    {
      break;
    }

    set(l_property, l_sales);

    l_exit= sales_detail_fetch_query(
      &l_error,
      &l_cursor,
      l_session,
      (*l_sales).m_sales_id);

    if (l_exit)
    {
      break;
    }

    do
    {

      l_eof= sales_detail_fetch_next(l_detail, &l_cursor);

      if (l_eof)
      {
        break;
      }

      gtk_list_store_append((*l_property).m_liststore, &l_iter);

      gtk_list_store_set(
        (*l_property).m_liststore,
        &l_iter,
        store_col_line_number_hidden, (*l_detail).m_line_number,
        store_col_line_number, (*l_detail).m_line_number,
        store_col_product_id, (*l_detail).m_product_id,
        store_col_description, (*l_detail).m_description,
        store_col_quantity, (*l_detail).m_quantity,
        store_col_unit_price, (*l_detail).m_unit_price,
        -1);

    }while(1);

  }while(0);

  g_free(l_detail);
  g_free(l_sales);
  cursor_discharge(&l_cursor);

  if (l_error)
  {
    _error_display(GTK_WINDOW(l_dialog), l_error);
    g_clear_error(&l_error);
  }

  return;
}

G_MODULE_EXPORT void
on_sales_billing_id_index_button_clicked(
  G_GNUC_UNUSED GtkButton*const         io_button,
  gpointer                              io_user_data)
{
  GtkBuilder*                           l_builder;
  gchar                                 l_contact_id[size_contact_id];
  GtkDialog*                            l_dialog;
  int                                   l_exit;
  struct property*                      l_property;
  struct session*                       l_session;

  memset(l_contact_id, 0, size_contact_id);

  l_dialog= GTK_DIALOG(io_user_data);
  l_property= (struct property*)g_object_get_data(G_OBJECT(l_dialog), "property");
  l_session= (struct session*)g_object_get_data(G_OBJECT(l_dialog), "session");
  l_builder= GTK_BUILDER(g_object_get_data(G_OBJECT(l_dialog), "builder"));
  
  l_exit= contact_index_form(l_contact_id, l_session, GTK_WINDOW(l_dialog), l_builder);

  if (0 == l_exit)
  {
    gtk_entry_set_text((*l_property).m_billing_id, l_contact_id);
  }

  return;
}

G_MODULE_EXPORT void
on_sales_service_contact_index_button_clicked(
  G_GNUC_UNUSED GtkButton*const         io_button,
  gpointer                              io_user_data)
{
  GtkBuilder*                           l_builder;
  GtkTextBuffer*                        l_buffer;
  struct contact*                       l_contact;
  gchar                                 l_contact_id[size_contact_id];
  GtkDialog*                            l_dialog;
  GError*                               l_error;
  int                                   l_exit;
  gchar*                                l_text;
  struct property*                      l_property;
  struct session*                       l_session;

  l_contact= (struct contact*)g_malloc0(sizeof(*l_contact));
  memset(l_contact_id, 0, size_contact_id);

  l_error= 0;
  l_dialog= GTK_DIALOG(io_user_data);
  l_property= (struct property*)g_object_get_data(G_OBJECT(l_dialog), "property");
  l_session= (struct session*)g_object_get_data(G_OBJECT(l_dialog), "session");
  l_builder= GTK_BUILDER(g_object_get_data(G_OBJECT(l_dialog), "builder"));

  do
  {

    l_exit= contact_index_form(l_contact_id, l_session, GTK_WINDOW(l_dialog), l_builder);

    if (l_exit)
    {
      break;
    }

    l_exit= contact_fetch(&l_error, l_contact, l_session, l_contact_id);

    if (l_exit)
    {
      break;
    }

    l_text= g_malloc0(1024);

    g_snprintf(
      l_text,
      1024,
      "%s%s"
      "%s %s\n" 
      "%s%s"
      "%s%s"
      "%s%s"
      "%s%s %s %s\n",
      (*l_contact).m_company_name, ((*l_contact).m_company_name[0] ? "\n" : ""),
      (*l_contact).m_first_name, (*l_contact).m_last_name,
      (*l_contact).m_street1, ((*l_contact).m_street1[0] ? "\n" : ""),
      (*l_contact).m_street2, ((*l_contact).m_street2[0] ? "\n" : ""),
      (*l_contact).m_street3, ((*l_contact).m_street3[0] ? "\n" : ""),
      (*l_contact).m_city, ((*l_contact).m_city[0] ? "," : ""),
      (*l_contact).m_state, (*l_contact).m_zipcode);

      l_buffer= gtk_text_view_get_buffer((*l_property).m_service_contact);
      gtk_text_buffer_set_text(l_buffer, l_text, -1);
      g_free(l_text);

  }while(0);

  g_free(l_contact);

  if (l_error)
  {
    _error_display(GTK_WINDOW(l_dialog), l_error);
    g_clear_error(&l_error);
  }

  return;
}

G_MODULE_EXPORT void
on_sales_service_contact_edit_button_clicked(
  G_GNUC_UNUSED GtkButton*const         io_button,
  gpointer                              io_user_data)
{
  GtkBuilder*                           l_builder;
  GtkDialog*                            l_dialog;
  struct property*                      l_property;

  l_dialog= GTK_DIALOG(io_user_data);
  l_property= (struct property*)g_object_get_data(G_OBJECT(l_dialog), "property");
  l_builder= GTK_BUILDER(g_object_get_data(G_OBJECT(l_dialog), "builder"));
  edit_form((*l_property).m_service_contact, GTK_WINDOW(l_dialog), l_builder);

  return;
}

G_MODULE_EXPORT void
on_sales_notes_edit_button_clicked(
  G_GNUC_UNUSED GtkButton*const         io_button,
  gpointer                              io_user_data)
{
  GtkBuilder*                           l_builder;
  GtkDialog*                            l_dialog;
  struct property*                      l_property;

  l_dialog= GTK_DIALOG(io_user_data);
  l_property= (struct property*)g_object_get_data(G_OBJECT(l_dialog), "property");
  l_builder= GTK_BUILDER(g_object_get_data(G_OBJECT(l_dialog), "builder"));
  edit_form((*l_property).m_notes, GTK_WINDOW(l_dialog), l_builder);

  return;
}

G_MODULE_EXPORT void
on_sales_notes_internal_edit_button_clicked(
  G_GNUC_UNUSED GtkButton*              io_button,
  gpointer                              io_user_data)
{
  GtkBuilder*                           l_builder;
  GtkDialog*                            l_dialog;
  struct property*                      l_property;

  l_dialog= GTK_DIALOG(io_user_data);
  l_property= (struct property*)g_object_get_data(G_OBJECT(l_dialog), "property");
  l_builder= GTK_BUILDER(g_object_get_data(G_OBJECT(l_dialog), "builder"));
  edit_form((*l_property).m_notes_internal, GTK_WINDOW(l_dialog), l_builder);

  return;
}

G_MODULE_EXPORT void
on_sales_sales_date_button_clicked(
  G_GNUC_UNUSED GtkButton*const         io_button,
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
  gtk_widget_get_allocation(GTK_WIDGET((*l_property).m_sales_date_button), &l_alloc);
  l_pos_x+= l_alloc.x;
  l_pos_y+= l_alloc.y;
  l_exit= calendar_form(&(*l_property).m_cal_sales_date, GTK_WINDOW(l_dialog), l_builder, l_pos_x, l_pos_y);

  if (0 == l_exit)
  {
    l_text= calendar_format(&(*l_property).m_cal_sales_date);
    gtk_entry_set_text((*l_property).m_sales_date, l_text);
    g_free(l_text);
  }

  return;
}

G_MODULE_EXPORT void
on_sales_due_date_button_clicked(
  G_GNUC_UNUSED GtkButton*const         io_button,
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
  gtk_widget_get_allocation(GTK_WIDGET((*l_property).m_due_date_button), &l_alloc);
  l_pos_x+= l_alloc.x;
  l_pos_y+= l_alloc.y;
  l_exit= calendar_form(&(*l_property).m_cal_due_date, GTK_WINDOW(l_dialog), l_builder, l_pos_x, l_pos_y);

  if (0 == l_exit)
  {
    l_text= calendar_format(&(*l_property).m_cal_due_date);
    gtk_entry_set_text((*l_property).m_due_date, l_text);
    g_free(l_text);
  }

  return;
}

G_MODULE_EXPORT void
on_sales_start_date_button_clicked(
  G_GNUC_UNUSED GtkButton*const         io_button,
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
  gtk_widget_get_allocation(GTK_WIDGET((*l_property).m_start_date_button), &l_alloc);
  l_pos_x+= l_alloc.x;
  l_pos_y+= l_alloc.y;
  l_exit= calendar_form(&(*l_property).m_cal_start_date, GTK_WINDOW(l_dialog), l_builder, l_pos_x, l_pos_y);

  if (0 == l_exit)
  {
    l_text= calendar_format(&(*l_property).m_cal_start_date);
    gtk_entry_set_text((*l_property).m_start_date, l_text);
    g_free(l_text);
  }

  return;
}

G_MODULE_EXPORT void
on_sales_completed_date_button_clicked(
  G_GNUC_UNUSED GtkButton*const         io_button,
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
  gtk_widget_get_allocation(GTK_WIDGET((*l_property).m_completed_date_button), &l_alloc);
  l_pos_x+= l_alloc.x;
  l_pos_y+= l_alloc.y + l_alloc.height;
  l_exit= calendar_form(&(*l_property).m_cal_completed_date, GTK_WINDOW(l_dialog), l_builder, l_pos_x, l_pos_y);

  if (0 == l_exit)
  {
    l_text= calendar_format(&(*l_property).m_cal_completed_date);
    gtk_entry_set_text((*l_property).m_completed_date, l_text);
    g_free(l_text);
  }

  return;
}

G_MODULE_EXPORT void
on_sales_detail_add_button_clicked(
  G_GNUC_UNUSED GtkButton*const         io_button,
  gpointer                              io_user_data)
{
  GtkBuilder*                           l_builder;
  struct sales_detail*                  l_detail;
  GtkDialog*                            l_dialog;
  int                                   l_exit;
  GtkTreeIter                           l_iter;
  guint                                 l_lines;
  struct property*                      l_property;
  struct session*                       l_session;

  l_detail= (struct sales_detail*)g_malloc0(sizeof(*l_detail));

  l_dialog= GTK_DIALOG(io_user_data);
  l_property= (struct property*)g_object_get_data(G_OBJECT(l_dialog), "property");
  l_session= (struct session*)g_object_get_data(G_OBJECT(l_dialog), "session");
  l_builder= GTK_BUILDER(g_object_get_data(G_OBJECT(l_dialog), "builder"));

  do
  {

    l_exit= sales_detail_form(l_detail, l_session, GTK_WINDOW(l_dialog), l_builder);

    if (l_exit)
    {
      break;
    }

    l_lines= gtk_tree_model_iter_n_children(GTK_TREE_MODEL((*l_property).m_liststore), 0);

    g_snprintf((*l_detail).m_line_number, sizeof((*l_detail).m_line_number), "%u", l_lines);

    gtk_list_store_append((*l_property).m_liststore, &l_iter);
    gtk_list_store_set(
      (*l_property).m_liststore,
      &l_iter,
      store_col_line_number_hidden, "",
      store_col_line_number, (*l_detail).m_line_number,
      store_col_product_id, (*l_detail).m_product_id,
      store_col_description, (*l_detail).m_description,
      store_col_quantity, (*l_detail).m_quantity,
      store_col_unit_price, (*l_detail).m_unit_price,
      -1);

  }while(0);

  g_free(l_detail);

  return;
}

G_MODULE_EXPORT void
on_sales_save_button_clicked(
  G_GNUC_UNUSED GtkButton*const         io_button,
  gpointer                              io_user_data)
{
  GtkDialog*                            l_dialog;

  l_dialog= GTK_DIALOG(io_user_data);
  save(l_dialog);

  return;
}

G_MODULE_EXPORT void
on_sales_detail_delete_button_clicked(
  G_GNUC_UNUSED GtkButton*const         io_button,
  gpointer                              io_user_data)
{
  gboolean                              l_bool;
  gchar                                 l_buffer[size_pg_small_int];
  GtkDialog*                            l_dialog;
  GError*                               l_error;
  int                                   l_exit;
  GtkTreeIter                           l_iter;
  gchar*                                l_line_number;
  gchar*                                l_line_number_hidden;
  GtkTreeModel*                         l_model;
  gsize                                 l_number;
  struct property*                      l_property;
  gboolean                              l_rc;
  gchar const*                          l_sales_id;
  GtkTreeSelection*                     l_selection;
  struct session*                       l_session;

  l_line_number= 0;
  l_line_number_hidden= 0;
  l_error= 0;
  l_dialog= GTK_DIALOG(io_user_data);
  l_property= (struct property*)g_object_get_data(G_OBJECT(l_dialog), "property");
  l_session= (struct session*)g_object_get_data(G_OBJECT(l_dialog), "session");

  do
  {

    l_selection= gtk_tree_view_get_selection((*l_property).m_treeview);

    l_rc= gtk_tree_selection_get_selected(l_selection, &l_model, &l_iter);

    if (FALSE == l_rc)

    {
      break;
    }

    gtk_tree_model_get(
      l_model,
      &l_iter,
      store_col_line_number_hidden, &l_line_number_hidden,
      store_col_line_number, &l_line_number,
      -1);

    if (l_line_number_hidden[0])
    {
      l_sales_id= gtk_entry_get_text((*l_property).m_sales_id);
      l_exit= sales_detail_delete(&l_error, l_session, l_sales_id, l_line_number_hidden);
      if (l_exit)
      {
        break;
      }
    }

    l_bool= gtk_list_store_remove(GTK_LIST_STORE(l_model), &l_iter);

    do
    {

      g_free(l_line_number_hidden);
      l_line_number_hidden= 0;
      g_free(l_line_number);
      l_line_number= 0;

      if (FALSE == l_bool)
      {
        break;
      }

      gtk_tree_model_get(
        l_model,
        &l_iter,
        store_col_line_number_hidden, &l_line_number_hidden,
        store_col_line_number, &l_line_number,
        -1);

      if (l_line_number_hidden[0])
      {
        l_number= strtoul(l_line_number_hidden, 0, 10);
        l_number--;
        g_snprintf(l_buffer, sizeof(l_buffer), "%lu", l_number);
        gtk_list_store_set(
          (*l_property).m_liststore,
          &l_iter,
          store_col_line_number_hidden, l_buffer,
          -1);
      }
      
      l_number= strtoul(l_line_number, 0, 10);
      l_number--;
      g_snprintf(l_buffer, sizeof(l_buffer), "%lu", l_number);
      gtk_list_store_set(
        (*l_property).m_liststore,
        &l_iter,
        store_col_line_number, l_buffer,
        -1);

      l_bool= gtk_tree_model_iter_next(l_model, &l_iter);

    }while(1);

  }while(0);

  g_free(l_line_number);
  g_free(l_line_number_hidden);

  if (l_error)
  {
    _error_display(GTK_WINDOW(l_dialog), l_error);
    g_clear_error(&l_error);
  }

  return;
}

G_MODULE_EXPORT void
on_sales_detail_edit_button_clicked(
  G_GNUC_UNUSED GtkButton*const         io_button,
  gpointer                              io_user_data)
{
  GtkBuilder*                           l_builder;
  gchar*                                l_description;
  struct sales_detail*                  l_detail;
  GtkDialog*                            l_dialog;
  GError*                               l_error;
  int                                   l_exit;
  GtkTreeIter                           l_iter;
  GtkTreeModel*                         l_model;
  gchar*                                l_product_id;
  struct property*                      l_property;
  gchar*                                l_quantity;
  gboolean                              l_rc;
  GtkTreeSelection*                     l_selection;
  struct session*                       l_session;
  gchar*                                l_unit_price;

  l_detail= (struct sales_detail*)g_malloc0(sizeof(*l_detail));

  l_error= 0;
  l_dialog= GTK_DIALOG(io_user_data);
  l_property= (struct property*)g_object_get_data(G_OBJECT(l_dialog), "property");
  l_session= (struct session*)g_object_get_data(G_OBJECT(l_dialog), "session");
  l_builder= GTK_BUILDER(g_object_get_data(G_OBJECT(l_dialog), "builder"));

  do
  {

    l_selection= gtk_tree_view_get_selection((*l_property).m_treeview);

    l_rc= gtk_tree_selection_get_selected(l_selection, &l_model, &l_iter);

    if (FALSE == l_rc)
    {
      break;
    }

    gtk_tree_model_get(
      l_model,
      &l_iter,
      store_col_product_id, &l_product_id,
      store_col_description, &l_description,
      store_col_quantity, &l_quantity,
      store_col_unit_price, &l_unit_price,
      -1);

    g_strlcpy((*l_detail).m_product_id, l_product_id, sizeof((*l_detail).m_product_id));
    g_strlcpy((*l_detail).m_description, l_description, sizeof((*l_detail).m_description));
    g_strlcpy((*l_detail).m_quantity, l_quantity, sizeof((*l_detail).m_quantity));
    g_strlcpy((*l_detail).m_unit_price, l_unit_price, sizeof((*l_detail).m_unit_price));

    g_free(l_product_id);
    g_free(l_description);
    g_free(l_quantity);
    g_free(l_unit_price);

    l_exit= sales_detail_form(l_detail, l_session, GTK_WINDOW(l_dialog), l_builder);

    if (l_exit)
    {
      break;
    }

    gtk_list_store_set(
      (*l_property).m_liststore,
      &l_iter,
      store_col_product_id, (*l_detail).m_product_id,
      store_col_description, (*l_detail).m_description,
      store_col_quantity, (*l_detail).m_quantity,
      store_col_unit_price, (*l_detail).m_unit_price,
      -1);

  }while(0);

  g_free(l_detail);

  if (l_error)
  {
    _error_display(GTK_WINDOW(l_dialog), l_error);
    g_clear_error(&l_error);
  }

  return;
}

G_MODULE_EXPORT void
on_sales_detail_treeview_row_activated(
  GtkTreeView*                          io_tree_view,
  GtkTreePath*                          io_path,
  G_GNUC_UNUSED GtkTreeViewColumn*      io_column,
  gpointer                              io_user_data)
{
  GtkBuilder*                           l_builder;
  gchar*                                l_description;
  struct sales_detail*                  l_detail;
  GtkDialog*                            l_dialog;
  int                                   l_exit;
  GtkTreeIter                           l_iter;
  GtkTreeModel*                         l_model;
  gchar*                                l_product_id;
  struct property*                      l_property;
  gchar*                                l_quantity;
  gboolean                              l_rc;
  struct session*                       l_session;
  gchar*                                l_unit_price;

  l_detail= (struct sales_detail*)g_malloc0(sizeof(*l_detail));

  l_dialog= GTK_DIALOG(io_user_data);
  l_property= (struct property*)g_object_get_data(G_OBJECT(l_dialog), "property");
  l_session= (struct session*)g_object_get_data(G_OBJECT(l_dialog), "session");
  l_builder= GTK_BUILDER(g_object_get_data(G_OBJECT(l_dialog), "builder"));

  do
  {

    l_model= gtk_tree_view_get_model(io_tree_view);

    l_rc= gtk_tree_model_get_iter(l_model, &l_iter, io_path);

    if (FALSE == l_rc)
    {
      break;
    }

    gtk_tree_model_get(
      l_model,
      &l_iter,
      store_col_product_id, &l_product_id,
      store_col_description, &l_description,
      store_col_quantity, &l_quantity,
      store_col_unit_price, &l_unit_price,
      -1);

    g_strlcpy((*l_detail).m_product_id, l_product_id, sizeof((*l_detail).m_product_id));
    g_strlcpy((*l_detail).m_description, l_description, sizeof((*l_detail).m_description));
    g_strlcpy((*l_detail).m_quantity, l_quantity, sizeof((*l_detail).m_quantity));
    g_strlcpy((*l_detail).m_unit_price, l_unit_price, sizeof((*l_detail).m_unit_price));

    g_free(l_product_id);
    g_free(l_description);
    g_free(l_quantity);
    g_free(l_unit_price);

    l_exit= sales_detail_form(l_detail, l_session, GTK_WINDOW(l_dialog), l_builder);

    if (l_exit)
    {
      break;
    }

    gtk_list_store_set(
      (*l_property).m_liststore,
      &l_iter,
      store_col_product_id, (*l_detail).m_product_id,
      store_col_description, (*l_detail).m_description,
      store_col_quantity, (*l_detail).m_quantity,
      store_col_unit_price, (*l_detail).m_unit_price,
      -1);

  }while(0);

  g_free(l_detail);

  return;
}

extern int
sales_form(
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

    l_dialog= GTK_DIALOG(gtk_builder_get_object(io_builder, "dialog_sales"));

    if (0 == l_dialog)
    {
      l_error= g_error_new(
        domain_general,
        error_generic,
        "Unable to find dialog: '%s'",
        "dialog_sales");
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
        "dialog_sales");
      break;
    }

    set_defaults(l_property);

    g_object_set_data(G_OBJECT(l_dialog), "builder", io_builder);
    g_object_set_data(G_OBJECT(l_dialog), "session", io_session);
    g_object_set_data(G_OBJECT(l_dialog), "property", l_property);

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
