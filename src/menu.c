/*
  menu.c

  License CC0 PUBLIC DOMAIN

  To the extent possible under law, Mark J. Olesen has waived all copyright
  and related or neighboring rights to tabitha. This work is published from:
  United States.
*/
#include "menu.h"
#include "company.h"
#include "product.h"
#include "receipt.h"
#include "sales.h"

G_MODULE_EXPORT void
on_quit_menuitem_activate(
  GtkMenuItem*                          object,
  gpointer                              user_data)
{

  gtk_main_quit();

  return;
}

G_MODULE_EXPORT void
on_contacts_menuitem_activate(
  GtkMenuItem*                          object,
  gpointer                              user_data)
{
  GtkWindow*                            l_window;
  struct session*                       l_session;
  GtkBuilder*                           l_builder;

  l_window= GTK_WINDOW(GTK_WIDGET(user_data));
  l_builder= GTK_BUILDER(g_object_get_data(G_OBJECT(l_window), "builder"));
  l_session= (struct session*)g_object_get_data(G_OBJECT(l_window), "session");
  contact_form(l_session, l_window, l_builder);

  return;
}

G_MODULE_EXPORT void
on_sales_menuitem_activate(
  GtkMenuItem*                          object,
  gpointer                              user_data)
{
  GtkWindow*                            l_window;
  struct session*                       l_session;
  GtkBuilder*                           l_builder;

  l_window= GTK_WINDOW(GTK_WIDGET(user_data));
  l_builder= GTK_BUILDER(g_object_get_data(G_OBJECT(l_window), "builder"));
  l_session= (struct session*)g_object_get_data(G_OBJECT(l_window), "session");
  sales_form(l_session, l_window, l_builder);

  return;
}

G_MODULE_EXPORT void
on_product_menuitem_activate(
  GtkMenuItem*                          object,
  gpointer                              user_data)
{
  GtkWindow*                            l_window;
  struct session*                       l_session;
  GtkBuilder*                           l_builder;

  l_window= GTK_WINDOW(GTK_WIDGET(user_data));
  l_builder= GTK_BUILDER(g_object_get_data(G_OBJECT(l_window), "builder"));
  l_session= (struct session*)g_object_get_data(G_OBJECT(l_window), "session");
  product_form(l_session, l_window, l_builder);

  return;
}

G_MODULE_EXPORT void
on_company_menuitem_activate(
  GtkMenuItem*                          object,
  gpointer                              user_data)
{
  GtkWindow*                            l_window;
  struct session*                       l_session;
  GtkBuilder*                           l_builder;

  l_window= GTK_WINDOW(GTK_WIDGET(user_data));
  l_builder= GTK_BUILDER(g_object_get_data(G_OBJECT(l_window), "builder"));
  l_session= (struct session*)g_object_get_data(G_OBJECT(l_window), "session");
  company_form(l_session, l_window, l_builder);

  return;
}

G_MODULE_EXPORT void
on_receipt_menuitem_activate(
  GtkMenuItem*                          object,
  gpointer                              user_data)
{
  GtkWindow*                            l_window;
  struct session*                       l_session;
  GtkBuilder*                           l_builder;

  l_window= GTK_WINDOW(GTK_WIDGET(user_data));
  l_builder= GTK_BUILDER(g_object_get_data(G_OBJECT(l_window), "builder"));
  l_session= (struct session*)g_object_get_data(G_OBJECT(l_window), "session");
  receipt_form(l_session, l_window, l_builder);

  return;
}
/* 
vim:expandtab:softtabstop=2:tabstop=2:shiftwidth=2:nowrap:ruler 
*/
