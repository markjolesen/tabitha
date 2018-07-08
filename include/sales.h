/*
  sales.h

  License CC0 PUBLIC DOMAIN

  To the extent possible under law, Mark J. Olesen has waived all copyright
  and related or neighboring rights to tabitha. This work is published from:
  United States.
*/
#if !defined(__sales_h__)

#include "contact.h"
#include "product.h"

enum salestype
{
  estimate                              = 0,
  invoice
};

struct sales
{
	gchar                                 m_sales_id[size_pg_big_int];
  enum salestype                        m_sales_type;
  gchar                                 m_billing_id[size_contact_id];
  gchar*                                m_service_contact;
  gchar                                 m_sales_date[size_date];
  gchar                                 m_start_date[size_date];
  gchar                                 m_completed_date[size_date];
  gchar                                 m_due_date[size_date];
  gchar                                 m_customer_po[size_customer_po];
  gchar                                 m_discount[size_money];
  gchar*                                m_notes;
  gchar*                                m_notes_internal;
};

struct sales_detail
{
  gchar                                 m_line_number[size_pg_small_int];
  gchar                                 m_product_id[size_product_id];
  gchar                                 m_description[size_description];
  gchar                                 m_quantity[size_pg_big_int];
  gchar                                 m_unit_price[size_money];
};

extern void
sales_trim(
  struct sales*const                    io_sales);

extern enum salestype
sales_type_as_enum(
  gchar const*                          i_sales_type);

extern gchar const*
sales_type_as_string(
  enum salestype const                   i_sales_type);

extern int
sales_insert(
  GError**                              o_error,
	gchar                                 o_sales_id[size_pg_big_int],
  struct session*const                  io_session,
  struct sales const*const       	      i_sales);

extern int
sales_update(
  GError**                              o_error,
  struct session*const                  io_session,
  struct sales const*const              i_sales);

extern int
sales_fetch(
  GError**                              o_error,
  struct sales*const                    o_sales,
  struct session*const                  io_session,
	gchar const                           i_sales_id[size_pg_big_int]);

extern int
sales_index_form(
  gchar                                 o_sales_id[size_pg_big_int],
  struct session*const                  io_session,
  GtkWindow*const                       io_parent,
  GtkBuilder*const                      io_builder);

extern int
sales_detail_delete(
  GError**                              o_error,
  struct session*const                  io_session,
	gchar const                           i_sales_id[size_pg_big_int],
  gchar const                           i_line_number[size_pg_small_int]);

extern int
sales_detail_fetch_query(
  GError**                              o_error,
  struct cursor*const                   o_cursor,
  struct session*const                  io_session,
  gchar const                           i_sales_id[size_pg_big_int]);

extern gboolean
sales_detail_fetch_next(
  struct sales_detail*const             o_detail,
  struct cursor*const                   io_cursor);

extern int
sales_detail_form(
  struct sales_detail*const             io_object,
  struct session*const                  io_session,
  GtkWindow*const                       io_parent,
  GtkBuilder*const                      io_builder);

extern int
sales_detail_insert(
  GError**                              o_error,
  struct session*const                  io_session,
	gchar const                           i_sales_id[size_pg_big_int],
  struct sales_detail const*const       i_detail);

extern int
sales_detail_tuple_count(
  GError**                              o_error,
  gsize*const                           o_tuples,
  struct session*const                  io_session,
	gchar const                           i_sales_id[size_pg_big_int]);

extern int
sales_detail_update(
  GError**                              o_error,
  struct session*const                  io_session,
	gchar const                           i_sales_id[size_pg_big_int],
  gchar const                           i_line_number[size_pg_small_int],
  struct sales_detail const*const       i_detail);

extern void
sales_detail_trim(
  struct sales_detail*const             io_detail);

extern int
sales_form(
  struct session*const                  io_session,
  GtkWindow*const                       io_parent,
  GtkBuilder*const                      io_builder);

extern int
sales_print(
  struct session*const                  io_session,
  GtkWindow*const                       io_parent,
  gchar const                           i_sales_id[size_pg_big_int]);

extern int
sales_update(
  GError**                              o_error,
  struct session*const                  io_session,
  struct sales const*const              i_sales);

#define __sales_h__
#endif
/* 
vim:expandtab:softtabstop=2:tabstop=2:shiftwidth=2:nowrap:ruler 
*/
