/*
  receipt.h

  License CC0 PUBLIC DOMAIN

  To the extent possible under law, Mark J. Olesen has waived all copyright
  and related or neighboring rights to tabitha. This work is published from:
  United States.
*/
#if !defined(__receipt_h__)

#include "session.h"

struct receipt
{
	gchar                                 m_receipt_id[size_pg_big_int];
	gchar                                 m_sales_id[size_pg_big_int];
  gchar                                 m_received_date[size_date];
  gchar                                 m_amount[size_money];
  gchar                                 m_reference[size_reference];
};

extern int
receipt_exists(
  GError**                              o_error,
  gboolean*                             o_exists,
  struct session*const                  io_session,
  gchar const                           i_receipt_id[size_pg_big_int]);

extern int
receipt_index_form(
  gchar                                 o_receipt_id[size_pg_big_int],
  struct session*const                  io_session,
  GtkWindow*const                       io_parent,
  GtkBuilder*const                      io_builder);

extern int
receipt_fetch(
  GError**                              o_error,
  struct receipt*const                  o_receipt,
  struct session*const                  io_session,
  gchar const                           i_receipt_id[size_pg_big_int]);

extern int
receipt_form(
  struct session*const                  io_session,
  GtkWindow*const                       io_parent,
  GtkBuilder*const                      io_builder);

extern int
receipt_save(
  GError**                              o_error,
  struct session*const                  io_session,
  struct receipt const*const            i_receipt);

extern void
receipt_trim(
  struct receipt*const                  io_receipt);

#define __receipt_h__
#endif
/* 
vim:expandtab:softtabstop=2:tabstop=2:shiftwidth=2:nowrap:ruler 
*/
