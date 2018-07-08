/*
  error.h

  License CC0 PUBLIC DOMAIN

  To the extent possible under law, Mark J. Olesen has waived all copyright
  and related or neighboring rights to tabitha. This work is published from:
  United States.
*/
#if !defined(__error_h__)

#include <gtk/gtk.h>
#include <glib.h>
#include <glib/gprintf.h>

enum error_domain
{
  domain_general                        = 0,
  domain_company,
  domain_contact,
  domain_glade,
  domain_product,
  domain_receipt,
  domain_sales,
  domain_sales_detail,
  domain_session
};

enum error_code
{
  error_generic                         = 0,
  error_company_unable_to_fetch_row,
  error_company_unable_to_update,
  error_contact_missing_id,
  error_contact_unable_to_count_rows,
  error_contact_unable_to_fetch_row,
  error_contact_unable_to_insert,
  error_contact_unable_to_populate,
  error_contact_unable_to_update,
  error_glade_file_not_found,
  error_product_missing_id,
  error_product_unable_to_count_rows,
  error_product_unable_to_fetch_row,
  error_product_unable_to_insert,
  error_product_unable_to_populate,
  error_product_unable_to_update,
  error_receipt_missing_amount,
  error_receipt_missing_id,
  error_receipt_unable_to_count_rows,
  error_receipt_unable_to_fetch_row,
  error_receipt_unable_to_insert,
  error_receipt_unable_to_populate,
  error_receipt_unable_to_update,
  error_sales_detail_form_product_does_not_exist,
  error_sales_detail_unable_to_delete_row,
  error_sales_detail_unable_to_fetch_row,
  error_sales_detail_unable_to_select_reorder,
  error_sales_detail_unable_to_start_transaction,
  error_sales_detail_unable_to_update_row,
  error_sales_print_detail_empty,
  error_sales_unable_to_count_rows,
  error_sales_unable_to_fetch_row,
  error_sales_unable_to_insert,
  error_sales_unable_to_populate,
  error_sales_unable_to_update,
  error_session_unable_to_connect
};

void
_error_display(
  GtkWindow*                            i_parent,
  GError const*const                    i_error);

#define __error_h__
#endif
/* 
vim:expandtab:softtabstop=2:tabstop=2:shiftwidth=2:nowrap:ruler 
*/
