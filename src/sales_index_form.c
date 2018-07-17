/*
  sales_index_form.c

  License CC0 PUBLIC DOMAIN

  To the extent possible under law, Mark J. Olesen has waived all copyright
  and related or neighboring rights to tabitha. This work is published from:
  United States.
*/
#include "sales.h"
#include "error.h"

enum
{
  store_col_sales_id                    = 0,
  store_col_sales_type,
  store_col_billing_id,
  store_col_sales_date
};

struct property
{
  GtkSearchEntry*                       m_search;
  GtkRadioButton*                       m_search_sales_id;
  GtkRadioButton*                       m_search_billing_id;
  GtkRadioButton*                       m_search_sales_date;
  GtkComboBoxText*                      m_filter;
  GtkTreeView*                          m_treeview;
  GtkListStore*                         m_liststore;
  GtkButton*                            m_nav_first;
  GtkButton*                            m_nav_previous;
  GtkButton*                            m_nav_next;
  GtkButton*                            m_nav_last;
  GtkSpinButton*                        m_limit;
  GtkSearchEntry*                       m_treeview_search;
  PGresult*                             m_result;
  gsize                                 m_tuples;
  gsize                                 m_tuple_offset;
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

    (*o_property).m_search=
      GTK_SEARCH_ENTRY(gtk_builder_get_object(io_builder, "sales_index_search"));

    if (0 == (*o_property).m_search)
    {
      break;
    }

    (*o_property).m_search_sales_id=
      GTK_RADIO_BUTTON(gtk_builder_get_object(io_builder, "sales_index_search_sales_id"));

    if (0 == (*o_property).m_search_sales_id)
    {
      break;
    }

    (*o_property).m_search_billing_id=
      GTK_RADIO_BUTTON(gtk_builder_get_object(io_builder, "sales_index_search_billing_id"));

    if (0 == (*o_property).m_search_billing_id)
    {
      break;
    }

    (*o_property).m_search_sales_date=
      GTK_RADIO_BUTTON(gtk_builder_get_object(io_builder, "sales_index_search_sales_date"));

    if (0 == (*o_property).m_search_sales_date)
    {
      break;
    }

    (*o_property).m_filter=
      GTK_COMBO_BOX_TEXT(gtk_builder_get_object(io_builder, "sales_index_filter"));

    if (0 == (*o_property).m_filter)
    {
      break;
    }

    (*o_property).m_treeview=
      GTK_TREE_VIEW(gtk_builder_get_object(io_builder, "sales_index_treeview"));

    if (0 == (*o_property).m_treeview)
    {
      break;
    }

    (*o_property).m_liststore=
      GTK_LIST_STORE(gtk_builder_get_object(io_builder, "sales_index_liststore"));

    if (0 == (*o_property).m_liststore)
    {
      break;
    }

    (*o_property).m_nav_first=
      GTK_BUTTON(gtk_builder_get_object(io_builder, "sales_index_first_button"));

    if (0 == (*o_property).m_nav_first)
    {
      break;
    }

    (*o_property).m_nav_previous=
      GTK_BUTTON(gtk_builder_get_object(io_builder, "sales_index_previous_button"));

    if (0 == (*o_property).m_nav_previous)
    {
      break;
    }

    (*o_property).m_nav_next=
      GTK_BUTTON(gtk_builder_get_object(io_builder, "sales_index_next_button"));

    if (0 == (*o_property).m_nav_next)
    {
      break;
    }

    (*o_property).m_nav_last=
      GTK_BUTTON(gtk_builder_get_object(io_builder, "sales_index_last_button"));

    if (0 == (*o_property).m_nav_last)
    {
      break;
    }

    (*o_property).m_limit=
      GTK_SPIN_BUTTON(gtk_builder_get_object(io_builder, "sales_index_limit"));

    if (0 == (*o_property).m_limit)
    {
      break;
    }

    (*o_property).m_treeview_search=
      GTK_SEARCH_ENTRY(gtk_builder_get_object(io_builder, "sales_index_treeview_search"));

    if (0 == (*o_property).m_treeview_search)
    {
      break;
    }

    gtk_tree_view_set_search_entry((*o_property).m_treeview, GTK_ENTRY((*o_property).m_treeview_search));

    l_exit= 0;

  }while(0);

  return l_exit;
}

static void
copy(
  gchar                                 o_sales_id[size_pg_big_int],
  struct property const*const           i_property)
{
  gchar*                                l_sales_id;
  GtkTreeIter                           l_iter;
  GtkTreeModel*                         l_model;
  gboolean                              l_rc;
  GtkTreeSelection*                     l_selection;

  memset(o_sales_id, 0, size_pg_big_int);
  l_sales_id= 0;

  l_selection = gtk_tree_view_get_selection((*i_property).m_treeview);

  do
  {

    l_rc= gtk_tree_selection_get_selected(l_selection, &l_model, &l_iter);

    if (FALSE == l_rc)
    {
      break;
    }

    gtk_tree_model_get(l_model, &l_iter, store_col_sales_id, &l_sales_id, -1);

    g_strlcpy(o_sales_id, l_sales_id, size_pg_big_int);

  }while(0);

  g_free(l_sales_id);

  return;
}

static void
navigation_buttons_set(
  GtkDialog*const                       io_dialog)
{
  gsize                                 l_limit;
  struct property*                      l_property;

  l_property= (struct property*)g_object_get_data(G_OBJECT(io_dialog), "property");
  l_limit= (gsize)gtk_spin_button_get_value_as_int((*l_property).m_limit);

  if ((*l_property).m_tuples < l_limit)
  {
    gtk_widget_set_sensitive(GTK_WIDGET((*l_property).m_nav_first), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET((*l_property).m_nav_previous), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET((*l_property).m_nav_next), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET((*l_property).m_nav_last), FALSE);
  }
  else
  {
    if (0 == (*l_property).m_tuple_offset)
    {
      gtk_widget_set_sensitive(GTK_WIDGET((*l_property).m_nav_first), FALSE);
      gtk_widget_set_sensitive(GTK_WIDGET((*l_property).m_nav_previous), FALSE);
    }
    else
    {
      gtk_widget_set_sensitive(GTK_WIDGET((*l_property).m_nav_first), TRUE);
      gtk_widget_set_sensitive(GTK_WIDGET((*l_property).m_nav_previous), TRUE);
    }

    if ((*l_property).m_tuples <= (*l_property).m_tuple_offset + l_limit)
    {
      gtk_widget_set_sensitive(GTK_WIDGET((*l_property).m_nav_next), FALSE);
      gtk_widget_set_sensitive(GTK_WIDGET((*l_property).m_nav_last), FALSE);
    }
    else
    {
      gtk_widget_set_sensitive(GTK_WIDGET((*l_property).m_nav_next), TRUE);
      gtk_widget_set_sensitive(GTK_WIDGET((*l_property).m_nav_last), TRUE);
    }
  }

  return;
}

static void
treeview_populate(
  GtkDialog*const                       io_dialog)
{
  gchar const*                          l_billing_id;
  gsize                                 l_count;
  GtkTreeIter                           l_iter;
  gsize                                 l_limit;
  struct property*                      l_property;
  gsize                                 l_row;
  gchar const*                          l_sales_date;
  gchar const*                          l_sales_id;
  gchar const*                          l_sales_type;

  l_property= (struct property*)g_object_get_data(G_OBJECT(io_dialog), "property");

  gtk_list_store_clear((*l_property).m_liststore);

  l_limit= (gsize)gtk_spin_button_get_value_as_int((*l_property).m_limit);

  for (
    l_count= 0, l_row= (*l_property).m_tuple_offset;
    (((*l_property).m_tuples > l_row) && (l_limit > l_count));
    l_row++, l_count++)
  {

    l_sales_id= PQgetvalue((*l_property).m_result, l_row, store_col_sales_id);
    l_sales_type= PQgetvalue((*l_property).m_result, l_row, store_col_sales_type);
    l_billing_id= PQgetvalue((*l_property).m_result, l_row, store_col_billing_id);
    l_sales_date= PQgetvalue((*l_property).m_result, l_row, store_col_sales_date);

    gtk_list_store_append((*l_property).m_liststore, &l_iter);
    gtk_list_store_set(
      (*l_property).m_liststore,
      &l_iter,
      store_col_sales_id,   l_sales_id,
      store_col_sales_type, l_sales_type,
      store_col_billing_id, l_billing_id,
      store_col_sales_date, l_sales_date,
      -1);
  }

  navigation_buttons_set(io_dialog);

  return;
}

static void
query(
  GtkDialog*const                       io_dialog)
{
  GError*                               l_error;
  char*                                 l_escaped;
  gint                                  l_filter;
  gsize                                 l_size;
  struct property*                      l_property;
  gchar const*                          l_search;
  struct session*                       l_session;
  gboolean                              l_state;
  gchar*                                l_statement;
  ExecStatusType                        l_status;
  gchar const*                          l_what;
  gchar*                                l_where;

  l_error= 0;

  l_session= (struct session*)g_object_get_data(G_OBJECT(io_dialog), "session");
  l_property= (struct property*)g_object_get_data(G_OBJECT(io_dialog), "property");

  PQclear((*l_property).m_result);
  (*l_property).m_result= 0;
  (*l_property).m_tuples= 0;
  (*l_property).m_tuple_offset= 0;

  gtk_list_store_clear((*l_property).m_liststore);

  l_what= 0;
  l_where= g_malloc0(256);

  l_search= gtk_entry_get_text(GTK_ENTRY((*l_property).m_search));

  do
  {

    if (0 == l_search[0])
    {
      break;
    }

    l_state= gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON((*l_property).m_search_billing_id));

    if (l_state)
    {
      l_what= "billing_id";
      break;
    }

    l_state= gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON((*l_property).m_search_sales_date));

    if (l_state)
    {
      l_what= "CAST(sales_date AS VARCHAR(32))";
      break;
    }

    l_what= "CAST(sales_id AS VARCHAR(19))";

  }while(0);

  if (l_what)
  {
    l_escaped= PQescapeLiteral((*l_session).m_connection, l_search, strlen(l_search));
    g_snprintf(l_where, 256, " WHERE %s LIKE %s", l_what, l_escaped);
    PQfreemem(l_escaped);
    l_escaped= 0;
  }

  l_filter= gtk_combo_box_get_active(GTK_COMBO_BOX((*l_property).m_filter));

  switch(l_filter)
  {
    case estimate:
      l_what= "Estimate";
      break;
    case invoice:
      l_what= "Invoice";
      break;
    default:
      l_what= 0;
  };

  if (l_what)
  {
    l_size= strlen(l_where);
    g_snprintf(
      &l_where[l_size],
      (256 - l_size),
      " %s sales_type = '%s'",
      ((l_size) ? " AND " : " WHERE "),
      l_what);
  }

  l_statement= g_malloc0(1024);

  g_snprintf(
    l_statement,
    1024,
    "SELECT"
    " sales_id,"
    " sales_type,"
    " billing_id,"
    " sales_date"
    " FROM sales "
    "%s"
    " ORDER BY sales_id DESC;",
    l_where);

  g_free(l_where);

  do
  {

    (*l_property).m_result= PQexec((*l_session).m_connection, l_statement);

    l_status= PQresultStatus((*l_property).m_result);

    if (PGRES_TUPLES_OK != l_status)
    {
      l_error= g_error_new(
        domain_sales,
        error_sales_unable_to_populate,
        "Unable to populate: '%s'",
        PQerrorMessage((*l_session).m_connection));
      break;
    }

    (*l_property).m_tuples= PQntuples((*l_property).m_result);
    treeview_populate(io_dialog);

  }while(0);

  g_free(l_statement);

  if (l_error)
  {
    _error_display(GTK_WINDOW(io_dialog), l_error);
    g_clear_error(&l_error);
  }

  return;
}

G_MODULE_EXPORT void
on_sales_index_spin_button_value_changed(
  G_GNUC_UNUSED GtkSpinButton*          io_spin_button,
  gpointer                              io_user_data)
{
  GtkDialog*                            l_dialog;

  l_dialog= GTK_DIALOG(io_user_data);
  treeview_populate(l_dialog);

  return;
}

G_MODULE_EXPORT void
on_sales_index_last_button_clicked (
  G_GNUC_UNUSED GtkButton*              io_button,
  gpointer                              io_user_data) 
{
  GtkDialog*                            l_dialog;
  gsize                                 l_limit;
  gsize                                 l_offset;
  struct property*                      l_property;

  l_dialog= GTK_DIALOG(io_user_data);

  l_dialog= GTK_DIALOG(io_user_data);
  l_property= (struct property*)g_object_get_data(G_OBJECT(l_dialog), "property");

  l_limit= (gsize)gtk_spin_button_get_value_as_int((*l_property).m_limit);
  l_offset= 0;

  do
  {

    if ((l_offset + l_limit) > (*l_property).m_tuples)
    {
      break;
    }

    l_offset+= l_limit;

  }while(1);

  if (l_offset != (*l_property).m_tuple_offset)
  {
    (*l_property).m_tuple_offset= l_offset;
    treeview_populate(l_dialog);
  }

  return;
}

G_MODULE_EXPORT void
on_sales_index_next_button_clicked(
  G_GNUC_UNUSED GtkButton*              io_button,
  gpointer                              io_user_data) 
{
  GtkDialog*                            l_dialog;
  gsize                                 l_limit;
  gsize                                 l_offset;
  struct property*                      l_property;

  l_dialog= GTK_DIALOG(io_user_data);
  l_property= (struct property*)g_object_get_data(G_OBJECT(l_dialog), "property");

  l_limit= (gsize)gtk_spin_button_get_value_as_int((*l_property).m_limit);
  l_offset= ((*l_property).m_tuple_offset + l_limit);

  if ((*l_property).m_tuples < l_offset)
  {
    l_offset= ((*l_property).m_tuples - l_limit);
    if (0 > (gssize)l_offset)
    {
      l_offset= 0;
    }
  }

  if (l_offset != (*l_property).m_tuple_offset)
  {
    (*l_property).m_tuple_offset= l_offset;
    treeview_populate(l_dialog);
  }

  return;
}

G_MODULE_EXPORT void
on_sales_index_previous_button_clicked(
  G_GNUC_UNUSED GtkButton*              io_button,
  gpointer                              io_user_data) 
{
  GtkDialog*                            l_dialog;
  gsize                                 l_limit;
  gsize                                 l_offset;
  struct property*                      l_property;

  l_dialog= GTK_DIALOG(io_user_data);
  l_property= (struct property*)g_object_get_data(G_OBJECT(l_dialog), "property");

  l_limit= (gsize)gtk_spin_button_get_value_as_int((*l_property).m_limit);
  l_offset= ((*l_property).m_tuple_offset - l_limit);

  if (0 > (gssize)l_offset)
  {
    l_offset= 0;
  }

  if (l_offset != (*l_property).m_tuple_offset)
  {
    (*l_property).m_tuple_offset= l_offset;
    treeview_populate(l_dialog);
  }
  return;
}

G_MODULE_EXPORT void
on_sales_index_first_button_clicked(
  G_GNUC_UNUSED GtkButton*              io_button,
  gpointer                              io_user_data) 
{
  GtkDialog*                            l_dialog;
  gsize                                 l_offset;
  struct property*                      l_property;

  l_dialog= GTK_DIALOG(io_user_data);
  l_property= (struct property*)g_object_get_data(G_OBJECT(l_dialog), "property");

  l_offset= 0;

  if (l_offset != (*l_property).m_tuple_offset)
  {
    (*l_property).m_tuple_offset= l_offset;
    treeview_populate(l_dialog);
  }

  return;
}

G_MODULE_EXPORT void
on_sales_index_treeview_row_activated(
  G_GNUC_UNUSED GtkTreeView*            io_tree_view,
  G_GNUC_UNUSED GtkTreePath*            io_path,
  G_GNUC_UNUSED GtkTreeViewColumn*      io_column,
  gpointer                              io_user_data)
{
  GtkDialog*                            l_dialog;

  l_dialog= GTK_DIALOG(io_user_data);
  gtk_dialog_response(l_dialog, GTK_RESPONSE_OK);

  return;
}

G_MODULE_EXPORT void
on_sales_index_search_changed(
  G_GNUC_UNUSED GtkSearchEntry*         io_entry,
  gpointer                              io_user_data)
{
  GtkDialog*                            l_dialog;

  l_dialog= GTK_DIALOG(io_user_data);
  query(l_dialog);

  return;
}

G_MODULE_EXPORT void
on_sales_index_search_sales_type_toggled(
  G_GNUC_UNUSED GtkToggleButton*        io_togglebutton,
  gpointer                              io_user_data)
{
  GtkDialog*                            l_dialog;
  struct property*                      l_property;
  gchar const*                          l_search;
  gboolean                              l_state;

  l_dialog= GTK_DIALOG(io_user_data);
  l_property= (struct property*)g_object_get_data(G_OBJECT(l_dialog), "property");
  l_search= gtk_entry_get_text(GTK_ENTRY((*l_property).m_search));

  if (l_search[0])
  {
    query(l_dialog);
  }

  do
  {

    l_state= gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON((*l_property).m_search_billing_id));

    if (l_state)
    {
      gtk_tree_view_set_search_column((*l_property).m_treeview, 2);
      break;
    }

    l_state= gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON((*l_property).m_search_sales_date));

    if (l_state)
    {
      gtk_tree_view_set_search_column((*l_property).m_treeview, 3);
      break;
    }

    gtk_tree_view_set_search_column((*l_property).m_treeview, 0);

  }while(0);

  return;
}

G_MODULE_EXPORT void
on_sales_index_filter_changed(
  G_GNUC_UNUSED GtkComboBox*            io_widget,
  gpointer                              io_user_data)
{
  GtkDialog*                            l_dialog;

  l_dialog= GTK_DIALOG(io_user_data);
  query(l_dialog);

  return;
}

G_MODULE_EXPORT void
on_sales_index_col_changed(
  GtkTreeViewColumn*                    io_treeviewcolumn,
  gpointer                              io_user_data)
{
  GtkDialog*                            l_dialog;
  struct property*                      l_property;
  gint                                  l_column_id;

  l_dialog= GTK_DIALOG(io_user_data);
  l_property= (struct property*)g_object_get_data(G_OBJECT(l_dialog), "property");
  l_column_id= gtk_tree_view_column_get_sort_column_id(io_treeviewcolumn);

  switch(l_column_id)
  {
    case 2:
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON((*l_property).m_search_billing_id), TRUE);
      break;
    case 3:
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON((*l_property).m_search_sales_date), TRUE);
      break;
    default:
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON((*l_property).m_search_sales_id), TRUE);
      break;
  };

  return;
}

extern int
sales_index_form(
  gchar                                 o_sales_id[size_pg_big_int],
  struct session*const                  io_session,
  GtkWindow*const                       io_parent,
  GtkBuilder*const                      io_builder)
{
  GtkDialog*                            l_dialog;
  GError*                               l_error;
  int                                   l_exit;
  struct property*                      l_property;

  memset(o_sales_id, 0, size_pg_big_int);

  l_dialog= 0;
  l_error= 0;
  l_exit= -1;
  l_property= (struct property*)g_malloc0(sizeof(*l_property));

  do
  {

    l_dialog= GTK_DIALOG(gtk_builder_get_object(io_builder, "dialog_sales_index"));

    if (0 == l_dialog)
    {
      l_error= g_error_new(
        domain_general,
        error_generic,
        "Unable to find dialog: '%s'",
        "dialog_sales_index");
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
        "dialog_sales_index");
      break;
    }

    g_object_set_data(G_OBJECT(l_dialog), "session", io_session);
    g_object_set_data(G_OBJECT(l_dialog), "property", l_property);

    gtk_widget_show_all(GTK_WIDGET(l_dialog));
    gtk_window_set_modal(GTK_WINDOW(l_dialog), 1);

    query(l_dialog);

    l_exit= gtk_dialog_run(l_dialog);

    if (GTK_RESPONSE_OK != l_exit)
    {
      break;
    }

    copy(o_sales_id, l_property);
    l_exit= 0;

  }while(0);

  PQclear((*l_property).m_result);

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
