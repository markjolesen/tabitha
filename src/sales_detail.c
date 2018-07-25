/*
  sales_form.c

  License CC0 PUBLIC DOMAIN

  To the extent possible under law, Mark J. Olesen has waived all copyright
  and related or neighboring rights to tabitha. This work is published from:
  United States.
*/
#include "sales.h"
#include "trim.h"
#include "error.h"
#include <stdlib.h>

extern void
sales_detail_trim(
  struct sales_detail*const             io_detail)
{

  g_strstrip((*io_detail).m_line_number);
  g_strstrip((*io_detail).m_product_id);
  g_strstrip((*io_detail).m_description);
  g_strstrip((*io_detail).m_unit_price);
  trim_money((*io_detail).m_unit_price);
  g_strstrip((*io_detail).m_quantity);
  trim_big_int((*io_detail).m_quantity);

  return;
}

static int
sales_detail_line_number_reorder(
  GError**                              o_error,
  struct session*const                  io_session,
  gchar const                           i_sales_id[size_pg_big_int])
{
  GError*                               l_error;
  gchar*                                l_escaped_sales_id;
  int                                   l_exit;
  gsize                                 l_number;
  PGresult*                             l_result;
  gchar*                                l_statement;
  ExecStatusType                        l_status;
  gchar*                                l_text;
  gsize                                 l_tuple;
  gsize                                 l_tuples;

  l_error= 0;
  l_escaped_sales_id= 0;
  l_exit= 0;
  l_result= 0;
  l_statement= 0;

  do
  {

    l_statement= g_malloc0(1024);

    l_escaped_sales_id= PQescapeLiteral((*io_session).m_connection, i_sales_id, strlen(i_sales_id));

    g_snprintf(
      l_statement,
      1024,
      "SELECT line_number"
      " FROM sales_detail"
      " WHERE sales_id = %s"
      " ORDER BY line_number",
      l_escaped_sales_id);

    l_result= PQexec((*io_session).m_connection, l_statement);

    l_status= PQresultStatus(l_result);

    if (PGRES_TUPLES_OK != l_status)
    {
      l_error= g_error_new(
        domain_sales_detail,
        error_sales_detail_unable_to_select_reorder,
        "Unable to count rows: '%s'",
        PQerrorMessage((*io_session).m_connection));
      l_exit= -1;
      break;
    }

    l_tuples= PQntuples(l_result);

    for (l_tuple= 0; l_tuples > l_tuple; l_tuple++)
    {
      l_text= PQgetvalue(l_result, l_tuple, 0);
      l_number= strtoul(l_text, 0, 10);
      if (l_number != l_tuple)
      {
        g_snprintf(
          l_statement,
          1024,
          "UPDATE sales_detail"
          " SET"
          " line_number = (line_number - 1)"
          " WHERE"
          " sales_id = %s"
          " AND"
          " line_number >= %lu;",
          l_escaped_sales_id,
          (unsigned long)l_tuple);

        PQclear(l_result);
        l_result= PQexec((*io_session).m_connection, l_statement);
        l_status= PQresultStatus(l_result);

        if (PGRES_COMMAND_OK != l_status)
        {
          l_error= g_error_new(
            domain_sales_detail,
            error_sales_detail_unable_to_update_row,
            "Unable to update row: '%s'",
            PQerrorMessage((*io_session).m_connection));
          l_exit= -1;
        }
        break;
      }
    }

  }while(0);

  g_free(l_statement);
  PQfreemem(l_escaped_sales_id);
  PQclear(l_result);

  if (l_error)
  {
    g_propagate_error(o_error, l_error);
  }

  return l_exit;
}

extern int
sales_detail_delete(
  GError**                              o_error,
  struct session*const                  io_session,
	gchar const                           i_sales_id[size_pg_big_int],
  gchar const                           i_line_number[size_pg_small_int])
{
  GError*                               l_error;
  gchar*                                l_escaped_line_number;
  gchar*                                l_escaped_sales_id;
  int                                   l_exit;
  PGresult*                             l_result;
  gchar*                                l_statement;
  ExecStatusType                        l_status;

  l_error= 0;
  l_exit= 0;
  l_result= 0;

  do
  {

    l_result= PQexec((*io_session).m_connection, "BEGIN");

    l_status= PQresultStatus(l_result);

    if (PGRES_COMMAND_OK != l_status)
    {
      l_error= g_error_new(
        domain_sales_detail,
        error_sales_detail_unable_to_start_transaction,
        "Unable to start transaction: '%s'",
        PQerrorMessage((*io_session).m_connection));
      l_exit= -1;
      break;
    }

    l_statement= g_malloc0(1024);

    l_escaped_sales_id= PQescapeLiteral((*io_session).m_connection, i_sales_id, strlen(i_sales_id));
    l_escaped_line_number= PQescapeLiteral((*io_session).m_connection, i_line_number, strlen(i_line_number));

    g_snprintf(
      l_statement,
      1024,
      "DELETE FROM sales_detail"
      " WHERE"
      " sales_id = %s"
      " AND"
      " line_number = %s",
      l_escaped_sales_id,
      l_escaped_line_number);

    PQfreemem(l_escaped_sales_id);
    l_escaped_sales_id= 0;

    PQfreemem(l_escaped_line_number);
    l_escaped_line_number= 0;
    
    l_result= PQexec((*io_session).m_connection, l_statement);

    g_free(l_statement);
    l_statement= 0;

    l_status= PQresultStatus(l_result);

    if (PGRES_COMMAND_OK != l_status)
    {
      l_error= g_error_new(
        domain_sales_detail,
        error_sales_detail_unable_to_delete_row,
        "Unable to delete row: '%s'",
        PQerrorMessage((*io_session).m_connection));
      l_exit= -1;
      break;
    }

    l_exit= sales_detail_line_number_reorder(&l_error, io_session, i_sales_id);

    if (l_exit)
    {
      break;
    }

    PQclear(l_result);
    l_result= PQexec((*io_session).m_connection, "END");

  }while(0);

  PQclear(l_result);

  if (l_error)
  {
    g_propagate_error(o_error, l_error);
  }

  return l_exit;
}

extern int
sales_detail_update(
  GError**                              o_error,
  struct session*const                  io_session,
	gchar const                           i_sales_id[size_pg_big_int],
  gchar const                           i_line_number[size_pg_small_int],
  struct sales_detail const*const       i_detail)
{
  GError*                               l_error;
  gchar*                                l_escaped_line_number;
  gchar*                                l_escaped_sales_id;
  int                                   l_exit;
  PGresult*                             l_result;
  gchar*                                l_statement;
  ExecStatusType                        l_status;
  gchar const*                          l_values[5];

  l_error= 0;
  l_exit= 0;
  l_result= 0;

  l_values[0]= __nullify(&(*i_detail).m_line_number[0]);
  l_values[1]= __nullify(&(*i_detail).m_product_id[0]);
  l_values[2]= __nullify(&(*i_detail).m_description[0]);
  l_values[3]= __nullify(&(*i_detail).m_quantity[0]);
  l_values[4]= __nullify(&(*i_detail).m_unit_price[0]);

  l_statement= g_malloc0(1024);

  l_escaped_sales_id= PQescapeLiteral((*io_session).m_connection, i_sales_id, strlen(i_sales_id));
  l_escaped_line_number= PQescapeLiteral((*io_session).m_connection, i_line_number, strlen(i_line_number));

  g_snprintf(
    l_statement,
    1024,
    "UPDATE sales_detail"
    " SET"
    " line_number = $1,"
    " product_id = $2,"
    " description = $3,"
    " quantity = $4,"
    " unit_price = $5"
    " WHERE"
    " sales_id = %s"
    " AND"
    " line_number = %s;",
    l_escaped_sales_id,
    l_escaped_line_number);

  PQfreemem(l_escaped_sales_id);
  l_escaped_sales_id= 0;

  PQfreemem(l_escaped_line_number);
  l_escaped_line_number= 0;
  
  l_result= PQexecParams(
    (*io_session).m_connection,
    l_statement,
    5,
    0,
    l_values,
    0,
    0,
    0);

  l_status= PQresultStatus(l_result);

  if (PGRES_COMMAND_OK != l_status)
  {
    l_error= g_error_new(
      domain_sales_detail,
      error_sales_detail_unable_to_update_row,
      "Unable to update sales detail table: '%s'",
      PQerrorMessage((*io_session).m_connection));
    l_exit= -1;
  }

  g_free(l_statement);

  PQclear(l_result);

  if (l_error)
  {
    g_propagate_error(o_error, l_error);
  }

  return l_exit;
}

extern int
sales_detail_tuple_count(
  GError**                              o_error,
  gsize*const                           o_tuples,
  struct session*const                  io_session,
	gchar const                           i_sales_id[size_pg_big_int])
{
  GError*                               l_error;
  gchar*                                l_escaped_sales_id;
  int                                   l_exit;
  PGresult*                             l_result;
  gchar*                                l_statement;
  ExecStatusType                        l_status;
  gchar*                                l_text;

  (*o_tuples)= 0;
  l_error= 0;
  l_escaped_sales_id= 0;
  l_exit= 0;
  l_result= 0;
  l_statement= 0;

  do
  {

    l_statement= g_malloc0(1024);

    l_escaped_sales_id= PQescapeLiteral((*io_session).m_connection, i_sales_id, strlen(i_sales_id));

    g_snprintf(
      l_statement,
      1024,
      "SELECT"
      " COUNT(*)"
      " FROM sales_detail"
      " WHERE"
      " sales_id = %s",
      l_escaped_sales_id);

    l_result= PQexec((*io_session).m_connection, l_statement);

    l_status= PQresultStatus(l_result);

    if (PGRES_TUPLES_OK != l_status)
    {
      l_error= g_error_new(
        domain_sales_detail,
        error_sales_detail_unable_to_fetch_row,
        "Unable to fetch row: '%s'",
        PQerrorMessage((*io_session).m_connection));
      l_exit= -1;
      break;
    }

    l_text= PQgetvalue(l_result, 0, 0);
    (*o_tuples)= strtoul(l_text, 0, 10);

  }while(0);

  g_free(l_statement);
  PQfreemem(l_escaped_sales_id);
  PQclear(l_result);

  if (l_error)
  {
    g_propagate_error(o_error, l_error);
  }

  return l_exit;
}

extern int
sales_detail_insert(
  GError**                              o_error,
  struct session*const                  io_session,
	gchar const                           i_sales_id[size_pg_big_int],
  struct sales_detail const*const       i_detail)
{
  GError*                               l_error;
  int                                   l_exit;
  PGresult*                             l_result;
  gchar                                 l_sales_id[size_pg_big_int];
  ExecStatusType                        l_status;
  gchar const*                          l_values[6];

  l_error= 0;
  l_exit= 0;
  l_result= 0;

  g_strlcpy(l_sales_id, i_sales_id, sizeof(l_sales_id));

  l_values[0]= __nullify(&l_sales_id[0]);
  l_values[1]= __nullify(&(*i_detail).m_line_number[0]);
  l_values[2]= __nullify(&(*i_detail).m_product_id[0]);
  l_values[3]= __nullify(&(*i_detail).m_description[0]);
  l_values[4]= __nullify(&(*i_detail).m_quantity[0]);
  l_values[5]= __nullify(&(*i_detail).m_unit_price[0]);
  
  l_result= PQexecParams(
    (*io_session).m_connection,
    "INSERT INTO sales_detail ("
    " sales_id,"
    " line_number,"
    " product_id,"
    " description,"
    " quantity,"
    " unit_price)"
    " VALUES"
    " ($1,$2,$3,$4,$5,$6);",
    6,
    0,
    l_values,
    0,
    0,
    0);

  l_status= PQresultStatus(l_result);

  if (PGRES_COMMAND_OK != l_status)
  {
    l_error= g_error_new(
      domain_product,
      error_product_unable_to_insert,
      "Unable to insert into product table: '%s'",
      PQerrorMessage((*io_session).m_connection));
    l_exit= -1;
  }

  PQclear(l_result);

  if (l_error)
  {
    g_propagate_error(o_error, l_error);
  }

  return l_exit;
}

extern int
sales_detail_fetch_query(
  GError**                              o_error,
  struct cursor*const                   o_cursor,
  struct session*const                  io_session,
  gchar const                           i_sales_id[size_pg_big_int])
{
  GError*                               l_error;
  gchar*                                l_escaped;
  int                                   l_exit;
  PGresult*                             l_result;
  gchar*                                l_statement;
  ExecStatusType                        l_status;
  gsize                                 l_tuples;

  memset(o_cursor, 0, sizeof(*o_cursor));

  l_error= 0;
  l_exit= 0;
  l_result= 0;

  do
  {

    l_escaped= PQescapeLiteral((*io_session).m_connection, i_sales_id, strlen(i_sales_id));

    l_statement= g_malloc0(1024);

    g_snprintf(
      l_statement,
      1024,
      "SELECT"
      " line_number,"
      " product_id,"
      " description,"
      " quantity,"
      " unit_price"
      " FROM sales_detail"
      " WHERE"
      " sales_id = %s"
      " ORDER BY line_number;",
      l_escaped);

    PQfreemem(l_escaped);
    l_escaped= 0;

    l_result= PQexec((*io_session).m_connection, l_statement);

    g_free(l_statement);
    l_statement= 0;

    l_status= PQresultStatus(l_result);

    if (PGRES_TUPLES_OK != l_status)
    {
      l_error= g_error_new(
        domain_sales,
        error_sales_detail_unable_to_fetch_row,
        "Unable to fetch row: '%s'",
        PQerrorMessage((*io_session).m_connection));
      l_exit= -1;
      break;
    }

    l_tuples= PQntuples(l_result);

    if (0 > (gssize)l_tuples)
    {
      l_exit= -1;
      break;
    }

    (*o_cursor).m_result= l_result;
    l_result= 0;

    (*o_cursor).m_tuples= l_tuples;

  }while(0);

  PQclear(l_result);

  if (l_error)
  {
    g_propagate_error(o_error, l_error);
  }

  return l_exit;
}

extern gboolean
sales_detail_fetch_next(
  struct sales_detail*const             o_detail,
  struct cursor*const                   io_cursor)
{
  gboolean                              l_eof;
  char*                                 l_text;

  memset(o_detail, 0, sizeof(*o_detail));

  l_eof= TRUE;

  do
  {

    if ((*io_cursor).m_tuples <= (*io_cursor).m_tuple)
    {
      break;
    }

    l_text= PQgetvalue((*io_cursor).m_result, (*io_cursor).m_tuple, 0);
    g_strlcpy((*o_detail).m_line_number, l_text, sizeof((*o_detail).m_line_number));

    l_text= PQgetvalue((*io_cursor).m_result, (*io_cursor).m_tuple, 1);
    g_strlcpy((*o_detail).m_product_id, l_text, sizeof((*o_detail).m_product_id));

    l_text= PQgetvalue((*io_cursor).m_result, (*io_cursor).m_tuple, 2);
    g_strlcpy((*o_detail).m_description, l_text, sizeof((*o_detail).m_description));

    l_text= PQgetvalue((*io_cursor).m_result, (*io_cursor).m_tuple, 3);
    g_strlcpy((*o_detail).m_quantity, l_text, sizeof((*o_detail).m_quantity));

    l_text= PQgetvalue((*io_cursor).m_result, (*io_cursor).m_tuple, 4);
    g_strlcpy((*o_detail).m_unit_price, l_text, sizeof((*o_detail).m_unit_price));

    l_eof= FALSE;
    (*io_cursor).m_tuple++;

  }while(0);

  return l_eof;
}
/* 
vim:expandtab:softtabstop=2:tabstop=2:shiftwidth=2:nowrap:ruler 
*/
