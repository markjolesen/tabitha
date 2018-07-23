/*
  email.c

  License CC0 PUBLIC DOMAIN

  To the extent possible under law, Mark J. Olesen has waived all copyright
  and related or neighboring rights to tabitha. This work is published from:
  United States.
*/
#include "email.h"
#include "company.h"
#include "contact.h"
#include "error.h"
#include "sales.h"
#include "trim.h"
#include <string.h>

extern void
email_trim(
  struct email*const                    io_email)
{

  if ((*io_email).m_from)
  {
    g_strstrip((*io_email).m_from);
  }

  if ((*io_email).m_to)
  {
    g_strstrip((*io_email).m_to);
  }

  if ((*io_email).m_subject)
  {
    g_strstrip((*io_email).m_subject);
  }

  if ((*io_email).m_message)
  {
    g_strstrip((*io_email).m_message);
  }

  if ((*io_email).m_server)
  {
    g_strstrip((*io_email).m_server);
  }

  if ((*io_email).m_username)
  {
    g_strstrip((*io_email).m_username);
  }

  if ((*io_email).m_password)
  {
    g_strstrip((*io_email).m_password);
  }

  if ((*io_email).m_port)
  {
    g_strstrip((*io_email).m_port);
  }

  return;
}

extern void
email_assign(
  struct email*const                    o_email)
{

  memset(o_email, 0, sizeof(*o_email));

  return;
}

extern void
email_discharge(
  struct email*const                    io_email)
{

  g_free((*io_email).m_from);
  g_free((*io_email).m_to);
  g_free((*io_email).m_subject);
  g_free((*io_email).m_message);
  g_free((*io_email).m_server);
  g_free((*io_email).m_username);
  g_free((*io_email).m_password);
  g_free((*io_email).m_port);

  memset(io_email, 0, sizeof(*io_email));

  return;
}

static int
email_load_company(
  GError**                              o_error,
  gchar                                 o_company_name[size_company_name],
  struct email*const                    io_email,
  struct session*const                  io_session)
{
  struct company*                       l_company;
  int                                   l_exit;

  memset(o_company_name, 0, size_company_name);
  l_company= (struct company*)g_malloc(sizeof(*l_company));

  do
  {

    l_exit= company_fetch(o_error, l_company, io_session);

    if (l_exit)
    {
      break;
    }

    g_strlcpy(o_company_name, (*l_company).m_company_name, size_company_name);

    if ((*l_company).m_email[0])
    {
      g_free((*io_email).m_from);
      (*io_email).m_from= g_strdup((*l_company).m_email);
    }

    if ((*l_company).m_smtp_server[0])
    {
      g_free((*io_email).m_server);
      (*io_email).m_server= g_strdup((*l_company).m_smtp_server);
    }

    if ((*l_company).m_smtp_username[0])
    {
      g_free((*io_email).m_username);
      (*io_email).m_username= g_strdup((*l_company).m_smtp_username);
    }

    if ((*l_company).m_smtp_password[0])
    {
      g_free((*io_email).m_password);
      (*io_email).m_password= g_strdup((*l_company).m_smtp_password);
    }

    if ((*l_company).m_smtp_port[0])
    {
      g_free((*io_email).m_port);
      (*io_email).m_port= g_strdup((*l_company).m_smtp_port);
    }

  }while(0);

  g_free(l_company);

  return l_exit;
}

static int
email_load_contact(
  GError**                              o_error,
  struct email*const                    io_email,
  struct session*const                  io_session,
  GString*const                         o_buffer,
  gchar const                           i_contact_id[size_contact_id],
  gchar const                           i_sales_id[size_pg_big_int],
  enum salestype const                  i_sales_type,
  gchar const                           i_company_name[size_company_name])
{
  gchar const*                          l_email;
  GError*                               l_error;
  gchar*                                l_escaped;
  int                                   l_exit;
  gchar const*                          l_first_name;
  gchar const*                          l_last_name;
  PGresult*                             l_result;
  ExecStatusType                        l_status;

  l_error= 0;
  l_exit= 0;
  l_result= 0;

  do
  {

    l_escaped= PQescapeLiteral((*io_session).m_connection, i_contact_id, strlen(i_contact_id));

    g_string_printf(
      o_buffer,
      "SELECT "
      " first_name,"
      " last_name,"
      " email"
      " FROM contact"
      " WHERE"
      " contact_id = %s"
      " LIMIT 1;",
      l_escaped);
    
    PQfreemem(l_escaped);
    l_escaped= 0;

    l_result= PQexec((*io_session).m_connection, (*o_buffer).str);

    l_status= PQresultStatus(l_result);

    if (PGRES_TUPLES_OK != l_status)
    {
      l_error= g_error_new(
        domain_email,
        error_email_unable_to_fetch_row,
        "Unable to fetch row: '%s'",
        PQerrorMessage((*io_session).m_connection));
      l_exit= -1;
      break;
    }

    l_first_name= PQgetvalue(l_result, 0, 0);
    l_last_name= PQgetvalue(l_result, 0, 1);
    l_email= PQgetvalue(l_result, 0, 2);

    g_free((*io_email).m_to);
    (*io_email).m_to= g_strdup(l_email);

    g_string_printf(
      o_buffer,
      "%s# %s",
      sales_type_as_string(i_sales_type),
      i_sales_id);

    (*io_email).m_subject= g_strdup((*o_buffer).str);

    g_string_printf(
      o_buffer,
      "Hello %s %s,\r\n\r\n"
      "Please find attached %s# %s\r\n\r\n"
      "%s\r\n",
      ((l_first_name) ? l_first_name : ""),
      ((l_last_name) ? l_last_name : ""),
      sales_type_as_string(i_sales_type),
      i_sales_id,
      i_company_name);

    (*io_email).m_message= g_strdup((*o_buffer).str);

  }while(0);

  PQclear(l_result);

  if (l_error)
  {
    g_propagate_error(o_error, l_error);
  }

  return l_exit;
}

extern int
email_load(
  GError**                              o_error,
  struct email*const                    io_email,
  struct session*const                  io_session,
  gchar const                           i_sales_id[size_pg_big_int])
{
  GString*                              l_buffer;
  gchar                                 l_company_name[size_company_name];
  gchar                                 l_contact_id[size_contact_id];
  GError*                               l_error;
  gchar*                                l_escaped;
  int                                   l_exit;
  PGresult*                             l_result;
  enum salestype                        l_sales_type;
  ExecStatusType                        l_status;
  gchar const*                          l_text;

  l_buffer= g_string_sized_new(1024);
  memset(l_company_name, 0, size_company_name);
  memset(l_contact_id, 0, size_contact_id);
  l_error= 0;
  l_exit= 0;
  l_result= 0;

  do
  {

    l_escaped= PQescapeLiteral((*io_session).m_connection, i_sales_id, strlen(i_sales_id));

    g_string_printf(
      l_buffer,
      "SELECT "
      " sales_type,"
      " billing_id"
      " FROM sales"
      " WHERE"
      " sales_id = %s"
      " LIMIT 1;",
      l_escaped);
    
    PQfreemem(l_escaped);
    l_escaped= 0;

    l_result= PQexec((*io_session).m_connection, (*l_buffer).str);

    l_status= PQresultStatus(l_result);

    if (PGRES_TUPLES_OK != l_status)
    {
      l_error= g_error_new(
        domain_email,
        error_email_unable_to_fetch_row,
        "Unable to fetch row: '%s'",
        PQerrorMessage((*io_session).m_connection));
      l_exit= -1;
      break;
    }

    l_text= PQgetvalue(l_result, 0, 0);
    l_sales_type= sales_type_as_enum(l_text);

    l_text= PQgetvalue(l_result, 0, 1);
    g_strlcpy(l_contact_id, l_text, sizeof(l_contact_id));

    l_exit= email_load_company(&l_error, l_company_name, io_email, io_session);

    if (l_exit)
    {
      g_clear_error(&l_error);
    }

    l_exit= email_load_contact(
      &l_error,
      io_email,
      io_session,
      l_buffer,
      l_contact_id,
      i_sales_id,
      l_sales_type,
      l_company_name);

  }while(0);

  PQclear(l_result);
  g_string_free(l_buffer, TRUE);

  if (l_error)
  {
    g_propagate_error(o_error, l_error);
  }

  return l_exit;
}
/* 
vim:expandtab:softtabstop=2:tabstop=2:shiftwidth=2:nowrap:ruler 
*/
