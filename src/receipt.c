/*
  receipt.c

  License CC0 PUBLIC DOMAIN

  To the extent possible under law, Mark J. Olesen has waived all copyright
  and related or neighboring rights to tabitha. This work is published from:
  United States.
*/
#include "receipt.h"
#include "trim.h"
#include "error.h"

extern void
receipt_trim(
  struct receipt*const                  io_receipt)
{

  g_strstrip((*io_receipt).m_receipt_id);
  g_strstrip((*io_receipt).m_sales_id);
  g_strstrip((*io_receipt).m_received_date);
  g_strstrip((*io_receipt).m_amount);
  trim_money((*io_receipt).m_amount);
  g_strstrip((*io_receipt).m_reference);

  return;
}

extern int
receipt_insert(
  GError**                              o_error,
	gchar                                 o_receipt_id[size_pg_big_int],
  struct session*const                  io_session,
  struct receipt const*const            i_receipt)
{
  GError*                               l_error;
  int                                   l_exit;
  PGresult*                             l_result;
  ExecStatusType                        l_status;
  gchar const*                          l_text;
  gchar const*                          l_values[4];

  memset(o_receipt_id, 0, size_pg_big_int);

  l_error= 0;
  l_exit= 0;
  l_result= 0;

  l_values[0]= __nullify(&(*i_receipt).m_sales_id[0]);
  l_values[1]= __nullify(&(*i_receipt).m_received_date[0]);
  l_values[2]= __nullify(&(*i_receipt).m_amount[0]);
  l_values[3]= __nullify(&(*i_receipt).m_reference[0]);
  
  l_result= PQexecParams(
    (*io_session).m_connection,
    "INSERT INTO receipt ("
    " sales_id,"
    " received_date,"
    " amount,"
    " reference)"
    " VALUES"
    " ($1,$2,$3,$4)"
    " RETURNING receipt_id;",
    4,
    0,
    l_values,
    0,
    0,
    0);

  do
  {

    l_status= PQresultStatus(l_result);

    if (PGRES_COMMAND_OK != l_status)
    {
      l_error= g_error_new(
        domain_receipt,
        error_receipt_unable_to_insert,
        "Unable to insert into receipt table: '%s'",
        PQerrorMessage((*io_session).m_connection));
      l_exit= -1;
      break;
    }

    l_text= PQgetvalue(l_result, 0, 0);
    g_strlcpy(o_receipt_id, l_text, size_pg_big_int);

  }while(0);

  PQclear(l_result);

  if (l_error)
  {
    g_propagate_error(o_error, l_error);
  }

  return l_exit;
}

extern int
receipt_update(
  GError**                              o_error,
  struct session*const                  io_session,
  struct receipt const*const            i_receipt)
{
  GError*                               l_error;
  gchar*                                l_escaped;
  int                                   l_exit;
  PGresult*                             l_result;
  gchar*                                l_statement;
  ExecStatusType                        l_status;
  gchar const*                          l_values[4];

  l_error= 0;
  l_exit= 0;
  l_result= 0;

  l_values[0]= __nullify(&(*i_receipt).m_sales_id[0]);
  l_values[1]= __nullify(&(*i_receipt).m_received_date[0]);
  l_values[2]= __nullify(&(*i_receipt).m_amount[0]);
  l_values[3]= __nullify(&(*i_receipt).m_reference[0]);

  l_escaped= PQescapeLiteral((*io_session).m_connection, (*i_receipt).m_sales_id, strlen((*i_receipt).m_sales_id));

  l_statement= g_malloc0(1024);

  g_snprintf(
    l_statement,
    1024,
    "UPDATE receipt"
    " SET"
    " sales_id = $1,"
    " received_date = $2,"
    " amount = $3,"
    " reference = $4"
    " WHERE receipt_id = %s;",
    l_escaped);

  PQfreemem(l_escaped);
  l_escaped= 0;
  
  l_result= PQexecParams(
    (*io_session).m_connection,
    l_statement,
    4,
    0,
    l_values,
    0,
    0,
    0);

  l_status= PQresultStatus(l_result);

  if (PGRES_COMMAND_OK != l_status)
  {
    l_error= g_error_new(
      domain_receipt,
      error_receipt_unable_to_update,
      "Unable to update receipt table: '%s'",
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
receipt_exists(
  GError**                              o_error,
  gboolean*                             o_exists,
  struct session*const                  io_session,
  gchar const                           i_receipt_id[size_pg_big_int])
{
  GError*                               l_error;
  char*                                 l_escaped;
  gboolean                              l_exists;
  int                                   l_exit;
  PGresult*                             l_result;
  gchar*                                l_statement;
  ExecStatusType                        l_status;
  gchar*                                l_text;

  l_error= 0;
  l_escaped= 0;
  l_exists= FALSE;
  l_exit= 0;
  l_result= 0;

  do
  {

    if (0 == i_receipt_id[0])
    {
      break;
    }

    l_escaped= PQescapeLiteral((*io_session).m_connection, i_receipt_id, strlen(i_receipt_id));

    l_statement= g_malloc0(256);

    g_snprintf(
      l_statement,
      256,
      "SELECT EXISTS"
      " (SELECT true from receipt" 
      " WHERE receipt_id = %s);",
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
        domain_receipt,
        error_receipt_unable_to_count_rows,
        "Unable to count rows: '%s'",
        PQerrorMessage((*io_session).m_connection));
      l_exit= -1;
      break;
    }

    l_text= PQgetvalue(l_result, 0, 0);

    l_exists= ('t' == l_text[0]);

  }while(0);

  PQclear(l_result);
  (*o_exists)= l_exists;

  if (l_error)
  {
    g_propagate_error(o_error, l_error);
  }

  return l_exit;
}

extern int
receipt_fetch(
  GError**                              o_error,
  struct receipt*const                  o_receipt,
  struct session*const                  io_session,
  gchar const                           i_receipt_id[size_pg_big_int])

{
  gchar                                 l_receipt_id[size_pg_big_int];
  GError*                               l_error;
  char*                                 l_escaped;
  int                                   l_exit;
  PGresult*                             l_result;
  gchar*                                l_statement;
  ExecStatusType                        l_status;
  char*                                 l_text;
  int                                   l_tuples;

  g_strlcpy(l_receipt_id, i_receipt_id, size_pg_big_int);

  memset(o_receipt, 0, sizeof(*o_receipt));
  l_error= 0;
  l_exit= 0;
  l_result= 0;

  do
  {

    l_escaped= PQescapeLiteral((*io_session).m_connection, l_receipt_id, strlen(l_receipt_id));

    l_statement= g_malloc0(1024);

    g_snprintf(
      l_statement,
      1024,
      "SELECT"
      " receipt_id,"
      " sales_id,"
      " received_date,"
      " amount,"
      " reference"
      " FROM receipt"
      " WHERE receipt_id = %s"
      " LIMIT 1;",
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
        domain_receipt,
        error_receipt_unable_to_fetch_row,
        "Unable to fetch row: '%s'",
        PQerrorMessage((*io_session).m_connection));
      l_exit= -1;
      break;
    }

    l_tuples= PQntuples(l_result);

    if (0 >= l_tuples)
    {
      l_exit= -1;
      break;
    }

    l_text= PQgetvalue(l_result, 0, 0);
    g_strlcpy((*o_receipt).m_receipt_id, l_text, sizeof((*o_receipt).m_receipt_id));

    l_text= PQgetvalue(l_result, 0, 1);
    g_strlcpy((*o_receipt).m_sales_id, l_text, sizeof((*o_receipt).m_sales_id));

    l_text= PQgetvalue(l_result, 0, 2);
    g_strlcpy((*o_receipt).m_received_date, l_text, sizeof((*o_receipt).m_received_date));

    l_text= PQgetvalue(l_result, 0, 3);
    g_strlcpy((*o_receipt).m_amount, l_text, sizeof((*o_receipt).m_amount));

    l_text= PQgetvalue(l_result, 0, 4);
    g_strlcpy((*o_receipt).m_reference, l_text, sizeof((*o_receipt).m_reference));

  }while(0);

  PQclear(l_result);

  if (l_error)
  {
    g_propagate_error(o_error, l_error);
  }

  return l_exit;
}

extern int
receipt_tally(
  GError**                              o_error,
  gchar                                 o_amount[size_money],
  struct session*const                  io_session,
	gchar const                           i_sales_id[size_pg_big_int])
{
  GError*                               l_error;
  char*                                 l_escaped;
  int                                   l_exit;
  PGresult*                             l_result;
  gchar*                                l_statement;
  ExecStatusType                        l_status;
  gchar*                                l_text;

  memset(o_amount, 0, size_money);

  l_error= 0;
  l_escaped= 0;
  l_exit= 0;
  l_result= 0;

  do
  {

    if (0 == i_sales_id[0])
    {
      break;
    }

    l_escaped= PQescapeLiteral((*io_session).m_connection, i_sales_id, strlen(i_sales_id));

    l_statement= g_malloc0(1024);

    g_snprintf(
      l_statement,
      1024,
      "SELECT"
      " SUM(amount)" 
      " FROM receipt"
      " WHERE sales_id = %s;",
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
        domain_receipt,
        error_receipt_unable_to_sum_amount,
        "Unable to sum amount: '%s'",
        PQerrorMessage((*io_session).m_connection));
      l_exit= -1;
      break;
    }

    l_text= PQgetvalue(l_result, 0, 0);
    g_strlcpy(o_amount, l_text, size_money);

  }while(0);

  PQclear(l_result);

  if (l_error)
  {
    g_propagate_error(o_error, l_error);
  }

  return l_exit;
}
/* 
vim:expandtab:softtabstop=2:tabstop=2:shiftwidth=2:nowrap:ruler 
*/
