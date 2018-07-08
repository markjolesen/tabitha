/*
  product.h

  License CC0 PUBLIC DOMAIN

  To the extent possible under law, Mark J. Olesen has waived all copyright
  and related or neighboring rights to tabitha. This work is published from:
  United States.
*/
#if !defined(__product_h__)

#include "contact.h"

struct product
{
  gchar                                 m_product_id[size_product_id];
  gchar                                 m_description[size_description];
  gchar                                 m_unit_price[size_money];
};

extern void
product_trim(
  struct product*const                  io_product);

extern int
product_exists(
  GError**                              o_error,
  gboolean*                             o_exists,
  struct session*const                  io_session,
  gchar const                           i_product_id[size_product_id]);

extern int
product_form(
  struct session*const                  io_session,
  GtkWindow*const                       io_parent,
  GtkBuilder*const                      io_builder);

extern int
product_save(
  GError**                              o_error,
  struct session*const                  io_session,
  struct product const*const            i_product);

extern int
product_fetch(
  GError**                              o_error,
  struct product*const                  o_product,
  struct session*const                  io_session,
  gchar const                           i_product_id[size_product_id]);

extern int
product_index_form(
  gchar                                 o_product_id[size_product_id],
  struct session*const                  io_session,
  GtkWindow*const                       io_parent,
  GtkBuilder*const                      io_builder);

#define __product_h__
#endif
/* 
vim:expandtab:softtabstop=2:tabstop=2:shiftwidth=2:nowrap:ruler 
*/
