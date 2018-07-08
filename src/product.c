/*
  product.c

  License CC0 PUBLIC DOMAIN

  To the extent possible under law, Mark J. Olesen has waived all copyright
  and related or neighboring rights to tabitha. This work is published from:
  United States.
*/
#include "product.h"
#include "trim.h"
#include "error.h"

extern void
product_trim(
  struct product*const                  io_product)
{

  g_strstrip((*io_product).m_product_id);
  g_strstrip((*io_product).m_description);
  g_strstrip((*io_product).m_unit_price);
  trim_money((*io_product).m_unit_price);

  return;
}

static int
product_insert(
  GError**                              o_error,
  struct session*const                  io_session,
  struct product const*const            i_product)
{
  GError*                               l_error;
  int                                   l_exit;
  PGresult*                             l_result;
  ExecStatusType                        l_status;
  gchar const*                          l_values[3];

  l_error= 0;
  l_exit= 0;
  l_result= 0;

  l_values[0]= __nullify(&(*i_product).m_product_id[0]);
  l_values[1]= __nullify(&(*i_product).m_description[0]);
  l_values[2]= __nullify(&(*i_product).m_unit_price[0]);
  
  l_result= PQexecParams(
    (*io_session).m_connection,
    "INSERT INTO product ("
    " product_id,"
    " description,"
    " unit_price)"
    " VALUES"
    " ($1,$2,$3);",
    3,
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

static int
product_update(
  GError**                              o_error,
  struct session*const                  io_session,
  struct product const*const            i_product)
{
  GError*                               l_error;
  gchar*                                l_escaped;
  int                                   l_exit;
  PGresult*                             l_result;
  gchar*                                l_statement;
  ExecStatusType                        l_status;
  gchar const*                          l_values[2];

  l_error= 0;
  l_exit= 0;
  l_result= 0;

  l_values[0]= __nullify(&(*i_product).m_description[0]);
  l_values[1]= __nullify(&(*i_product).m_unit_price[0]);

  l_escaped= PQescapeLiteral((*io_session).m_connection, (*i_product).m_product_id, strlen((*i_product).m_product_id));

  l_statement= g_malloc0(1024);

  g_snprintf(
    l_statement,
    1024,
    "UPDATE product"
    " SET"
    " description = $1,"
    " unit_price = $2"
    " WHERE product_id = %s;",
    l_escaped);

  PQfreemem(l_escaped);
  l_escaped= 0;
  
  l_result= PQexecParams(
    (*io_session).m_connection,
    l_statement,
    2,
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
      error_product_unable_to_update,
      "Unable to update product table: '%s'",
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
product_exists(
  GError**                              o_error,
  gboolean*                             o_exists,
  struct session*const                  io_session,
  gchar const                           i_product_id[size_product_id])
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

    l_escaped= PQescapeLiteral((*io_session).m_connection, i_product_id, strlen(i_product_id));

    l_statement= g_malloc0(256);

    g_snprintf(
      l_statement,
      256,
      "SELECT EXISTS"
      " (SELECT true from product" 
      " WHERE product_id = %s);",
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
        domain_product,
        error_product_unable_to_count_rows,
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
product_save(
  GError**                              o_error,
  struct session*const                  io_session,
  struct product const*const            i_product)
{
  GError*                               l_error;
  int                                   l_exit;
  gboolean                              l_exists;

  l_error= 0;
  l_exit= 0;

  l_exit= product_exists(&l_error, &l_exists, io_session, (*i_product).m_product_id);
  
  if (0 == l_exit)
  {
    if (l_exists)
    {
      l_exit= product_update(&l_error, io_session, i_product);
    }
    else
    {
      l_exit= product_insert(&l_error, io_session, i_product);
    }
  }

  if (l_error)
  {
    g_propagate_error(o_error, l_error);
  }

  return l_exit;
}

extern int
product_fetch(
  GError**                              o_error,
  struct product*const                  o_product,
  struct session*const                  io_session,
  gchar const                           i_product_id[size_product_id])

{
  gchar                                 l_product_id[size_product_id];
  GError*                               l_error;
  char*                                 l_escaped;
  int                                   l_exit;
  PGresult*                             l_result;
  gchar*                                l_statement;
  ExecStatusType                        l_status;
  char*                                 l_text;
  int                                   l_tuples;

  g_strlcpy(l_product_id, i_product_id, size_product_id);

  memset(o_product, 0, sizeof(*o_product));
  l_error= 0;
  l_exit= 0;
  l_result= 0;

  do
  {

    l_escaped= PQescapeLiteral((*io_session).m_connection, l_product_id, strlen(l_product_id));

    l_statement= g_malloc0(1024);

    g_snprintf(
      l_statement,
      1024,
      "SELECT"
      " product_id,"
      " description,"
      " unit_price"
      " FROM product"
      " WHERE product_id = %s"
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
        domain_product,
        error_product_unable_to_fetch_row,
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
    g_strlcpy((*o_product).m_product_id, l_text, sizeof((*o_product).m_product_id));

    l_text= PQgetvalue(l_result, 0, 1);
    g_strlcpy((*o_product).m_description, l_text, sizeof((*o_product).m_description));

    l_text= PQgetvalue(l_result, 0, 2);
    g_strlcpy((*o_product).m_unit_price, l_text, sizeof((*o_product).m_unit_price));

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
