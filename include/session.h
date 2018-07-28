/*
  session.h

  License CC0 PUBLIC DOMAIN

  To the extent possible under law, Mark J. Olesen has waived all copyright
  and related or neighboring rights to tabitha. This work is published from:
  United States.
*/
#if !defined(__session_h__)

#include <libpq-fe.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <glib/gi18n.h>
#include "aspect.h"

#define __nullify(s) ((s) ? ((s)[0] ? (s) : 0) : 0)

enum size
{
  size_cellphone                        = (16+1),
  size_city                             = (24+1),
  size_company_name                     = (32+1),
  size_contact_id                       = (19+1),
  size_customer_po                      = (32+1),
  size_date                             = (10+1),
  size_description                      = (64+1),
  size_email                            = (41+1),
  size_fax                              = (16+1),
  size_first_name                       = (32+1),
  size_last_name                        = (32+1),
  size_money                            = (14+1),
  size_pg_big_int                       = (19+1),
  size_pg_small_int                     = (5+1),
  size_phone                            = (16+1),
  size_product_id                       = (19+1),
  size_reference                        = (32+1),
  size_state                            = (4+1),
  size_street1                          = (64+1),
  size_street2                          = (64+1),
  size_street3                          = (64+1),
  size_website                          = (41+1),
  size_zipcode                          = (11+1),
  size_smtp_server                      = (41+1),
  size_smtp_username                    = (41+1),
  size_smtp_password                    = (41+1)
};

struct cursor
{
  PGresult*                             m_result;
  guint64                               m_tuples;
  guint64                               m_tuple;
};

struct session
{
  PGconn*                               m_connection;
};

extern void
cursor_assign(
  struct cursor*const                   o_cursor);

extern void
cursor_discharge(
  struct cursor*const                   io_cursor);

extern void
session_assign(
  struct session*const                  o_session);

extern int
session_connect(
  GError**                              o_error,
  struct session*const                  io_session,
  struct aspect const*const             i_aspect);

extern void
session_discharge(
  struct session*const                  io_session);

extern void
session_disconnect(
  struct session*const                  io_session);

#define __session_h__
#endif
/* 
vim:expandtab:softtabstop=2:tabstop=2:shiftwidth=2:nowrap:ruler 
*/
