/*
  sales.c

  License CC0 PUBLIC DOMAIN

  To the extent possible under law, Mark J. Olesen has waived all copyright
  and related or neighboring rights to tabitha. This work is published from:
  United States.
*/
#include "sales.h"
#include "trim.h"
#include "error.h"

extern void
sales_trim(
  struct sales*const                    io_sales)
{

  g_strstrip((*io_sales).m_sales_id);
  g_strstrip((*io_sales).m_billing_id);
  g_strstrip((*io_sales).m_sales_date);
  g_strstrip((*io_sales).m_start_date);
  g_strstrip((*io_sales).m_completed_date);
  g_strstrip((*io_sales).m_due_date);
  g_strstrip((*io_sales).m_customer_po);
  g_strstrip((*io_sales).m_discount);
  trim_money((*io_sales).m_discount);

  return;
}

extern enum salestype
sales_type_as_enum(
  gchar const*                          i_sales_type)
{
  int                                   l_rc;
  enum salestype                        l_sales_type;

  do
  {

    l_rc= g_strcmp0("Invoice", i_sales_type);

    if (0 == l_rc)
    {
      l_sales_type= invoice;
      break;
    }

    l_sales_type= estimate;

  }while(0);

  return l_sales_type;
}

extern gchar const*
sales_type_as_string(
  enum salestype const                   i_sales_type)
{
  gchar const*                           l_type;

  switch(i_sales_type)
  {
    case invoice:
      l_type= "Invoice";
      break;
    default:
      l_type= "Estimate";
      break;
  }

  return l_type;
} 

extern int
sales_insert(
  GError**                              o_error,
	gchar                                 o_sales_id[size_pg_big_int],
  struct session*const                  io_session,
  struct sales const*const       	      i_sales)
{
  GError*                               l_error;
  int                                   l_exit;
  PGresult*                             l_result;
  ExecStatusType                        l_status;
  gchar*                                l_text;
  gchar const*                          l_values[11];

  memset(o_sales_id, 0, size_pg_big_int);

  l_error= 0;
  l_exit= 0;
  l_result= 0;

  l_values[0]= sales_type_as_string((*i_sales).m_sales_type);
  l_values[1]= __nullify(&(*i_sales).m_billing_id[0]);
  l_values[2]= __nullify((*i_sales).m_service_contact);
  l_values[3]= __nullify(&(*i_sales).m_sales_date[0]);
  l_values[4]= __nullify(&(*i_sales).m_start_date[0]);
  l_values[5]= __nullify(&(*i_sales).m_completed_date[0]);
  l_values[6]= __nullify(&(*i_sales).m_due_date[0]);
  l_values[7]= __nullify(&(*i_sales).m_customer_po[0]);
  l_values[8]= __nullify(&(*i_sales).m_discount[0]);
  l_values[9]= __nullify((*i_sales).m_notes);
  l_values[10]= __nullify((*i_sales).m_notes_internal);

  l_result= PQexecParams(
    (*io_session).m_connection,
    "INSERT INTO sales ("
    "sales_type,"
    " billing_id,"
    " service_contact,"
    " sales_date,"
    " start_date,"
    " completed_date,"
    " due_date,"
    " customer_po,"
    " discount,"
    " notes,"
    " notes_internal)"
    " VALUES"
    " ($1,$2,$3,$4,$5,$6,$7,$8,$9,$10,$11)"
    " RETURNING sales_id;",
    11,
    0,
    l_values,
    0,
    0,
    0);

  do
  {

    l_status= PQresultStatus(l_result);

    if (PGRES_TUPLES_OK != l_status)
    {
      l_error= g_error_new(
        domain_sales,
        error_sales_unable_to_insert,
        "Unable to insert into sales table: '%s'",
        PQerrorMessage((*io_session).m_connection));
      l_exit= -1;
      break;
    }

    l_text= PQgetvalue(l_result, 0, 0);
    g_strlcpy(o_sales_id, l_text, size_pg_big_int);

  }while(0);

  PQclear(l_result);

  if (l_error)
  {
    g_propagate_error(o_error, l_error);
  }

  return l_exit;
}

extern int
sales_update(
  GError**                              o_error,
  struct session*const                  io_session,
  struct sales const*const              i_sales)
{
  GError*                               l_error;
  gchar*                                l_escaped;
  int                                   l_exit;
  PGresult*                             l_result;
  gchar*                                l_statement;
  ExecStatusType                        l_status;
  gchar const*                          l_values[11];

  l_error= 0;
  l_exit= 0;
  l_result= 0;

  l_values[0]= sales_type_as_string((*i_sales).m_sales_type);
  l_values[1]= __nullify(&(*i_sales).m_billing_id[0]);
  l_values[2]= __nullify((*i_sales).m_service_contact);
  l_values[3]= __nullify(&(*i_sales).m_sales_date[0]);
  l_values[4]= __nullify(&(*i_sales).m_start_date[0]);
  l_values[5]= __nullify(&(*i_sales).m_completed_date[0]);
  l_values[6]= __nullify(&(*i_sales).m_due_date[0]);
  l_values[7]= __nullify(&(*i_sales).m_customer_po[0]);
  l_values[8]= __nullify(&(*i_sales).m_discount[0]);
  l_values[9]= __nullify((*i_sales).m_notes);
  l_values[10]= __nullify((*i_sales).m_notes_internal);

  l_escaped= PQescapeLiteral((*io_session).m_connection, (*i_sales).m_sales_id, strlen((*i_sales).m_sales_id));

  l_statement= g_malloc0(1024);

  g_snprintf(
    l_statement,
    1024,
    "UPDATE sales"
    " SET"
    " sales_type = $1,"
    " billing_id = $2,"
    " service_contact = $3,"
    " sales_date = $4,"
    " start_date = $5,"
    " completed_date = $6,"
    " due_date = $7,"
    " customer_po = $8,"
    " discount = $9,"
    " notes = $10,"
    " notes_internal = $11"
    " WHERE sales_id = %s;",
    l_escaped);

  PQfreemem(l_escaped);
  l_escaped= 0;
  
  l_result= PQexecParams(
    (*io_session).m_connection,
    l_statement,
    11,
    0,
    l_values,
    0,
    0,
    0);

  l_status= PQresultStatus(l_result);

  if (PGRES_COMMAND_OK != l_status)
  {
    l_error= g_error_new(
      domain_sales,
      error_sales_unable_to_update,
      "Unable to update sales table: '%s'",
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
sales_fetch(
  GError**                              o_error,
  struct sales*const                    o_sales,
  struct session*const                  io_session,
	gchar const                           i_sales_id[size_pg_big_int])
{
  GError*                               l_error;
  gchar*                                l_escaped;
  int                                   l_exit;
  PGresult*                             l_result;
  gchar*                                l_statement;
  ExecStatusType                        l_status;
  char*                                 l_text;
  gint                                  l_tuples;

  memset(o_sales, 0, sizeof(*o_sales));

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
      " sales_id,"
      " sales_type,"
      " billing_id,"
      " service_contact,"
      " sales_date,"
      " start_date,"
      " completed_date,"
      " due_date,"
      " customer_po,"
      " discount,"
      " notes,"
      " notes_internal"
      " FROM sales"
      " WHERE sales_id = %s"
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
        domain_sales,
        error_sales_unable_to_fetch_row,
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
    g_strlcpy((*o_sales).m_sales_id, l_text, sizeof((*o_sales).m_sales_id));

    l_text= PQgetvalue(l_result, 0, 1);
    (*o_sales).m_sales_type= sales_type_as_enum(l_text);

    l_text= PQgetvalue(l_result, 0, 2);
    g_strlcpy((*o_sales).m_billing_id, l_text, sizeof((*o_sales).m_billing_id));

    l_text= PQgetvalue(l_result, 0, 3);
    (*o_sales).m_service_contact= g_strdup(l_text);

    l_text= PQgetvalue(l_result, 0, 4);
    g_strlcpy((*o_sales).m_sales_date, l_text, sizeof((*o_sales).m_sales_date));

    l_text= PQgetvalue(l_result, 0, 5);
    g_strlcpy((*o_sales).m_start_date, l_text, sizeof((*o_sales).m_start_date));

    l_text= PQgetvalue(l_result, 0, 6);
    g_strlcpy((*o_sales).m_completed_date, l_text, sizeof((*o_sales).m_completed_date));

    l_text= PQgetvalue(l_result, 0, 7);
    g_strlcpy((*o_sales).m_due_date, l_text, sizeof((*o_sales).m_due_date));

    l_text= PQgetvalue(l_result, 0, 8);
    g_strlcpy((*o_sales).m_customer_po, l_text, sizeof((*o_sales).m_customer_po));

    l_text= PQgetvalue(l_result, 0, 9);
    g_strlcpy((*o_sales).m_discount, l_text, sizeof((*o_sales).m_discount));

    l_text= PQgetvalue(l_result, 0, 10);
    (*o_sales).m_notes= g_strdup(l_text);

    l_text= PQgetvalue(l_result, 0, 11);
    (*o_sales).m_notes_internal= g_strdup(l_text);

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
