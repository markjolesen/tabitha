/*
  company.c

  License CC0 PUBLIC DOMAIN

  To the extent possible under law, Mark J. Olesen has waived all copyright
  and related or neighboring rights to tabitha. This work is published from:
  United States.
*/
#include "company.h"
#include "error.h"
#include "password.h"
#include "trim.h"

extern void
company_trim(
  struct company*const                  io_company)
{

  g_strstrip((*io_company).m_company_name);
  g_strstrip((*io_company).m_street1);
  g_strstrip((*io_company).m_street2);
  g_strstrip((*io_company).m_street3);
  g_strstrip((*io_company).m_city);
  g_strstrip((*io_company).m_state);
  g_strstrip((*io_company).m_zipcode);
  g_strstrip((*io_company).m_phone);
  g_strstrip((*io_company).m_cellphone);
  g_strstrip((*io_company).m_fax);
  g_strstrip((*io_company).m_email);
  g_strstrip((*io_company).m_smtp_server);
  g_strstrip((*io_company).m_smtp_username);
  g_strstrip((*io_company).m_smtp_password);
  g_strstrip((*io_company).m_smtp_port);
  trim_small_int((*io_company).m_smtp_port);

  return;
}

extern int
company_update(
  GError**                              o_error,
  struct session*const                  io_session,
  struct company const*const            i_company)
{
  GError*                               l_error;
  int                                   l_exit;
  gchar*                                l_password;
  PGresult*                             l_result;
  gchar*                                l_statement;
  ExecStatusType                        l_status;
  gchar const*                          l_values[16];

  l_error= 0;
  l_exit= 0;
  l_password= password_encrypt(io_session, (*i_company).m_smtp_password);
  l_result= 0;

  l_values[0]= __nullify(&(*i_company).m_company_name[0]);
  l_values[1]= __nullify(&(*i_company).m_street1[0]);
  l_values[2]= __nullify(&(*i_company).m_street2[0]);
  l_values[3]= __nullify(&(*i_company).m_street3[0]);
  l_values[4]= __nullify(&(*i_company).m_city[0]);
  l_values[5]= __nullify(&(*i_company).m_state[0]);
  l_values[6]= __nullify(&(*i_company).m_zipcode[0]);
  l_values[7]= __nullify(&(*i_company).m_phone[0]);
  l_values[8]= __nullify(&(*i_company).m_cellphone[0]);
  l_values[9]= __nullify(&(*i_company).m_fax[0]);
  l_values[10]= __nullify(&(*i_company).m_email[0]);
  l_values[11]= __nullify(&(*i_company).m_website[0]);
  l_values[12]= __nullify(&(*i_company).m_smtp_server[0]);
  l_values[13]= __nullify(&(*i_company).m_smtp_username[0]);
  l_values[14]= __nullify(l_password);
  l_values[15]= __nullify(&(*i_company).m_smtp_port[0]);

  l_statement= g_malloc0(1024);

  g_snprintf(
    l_statement,
    1024,
    "UPDATE company"
    " SET"
    " company_name = $1,"
    " street1 = $2,"
    " street2 = $3,"
    " street3 = $4,"
    " city = $5,"
    " state = $6,"
    " zipcode = $7,"
    " phone = $8,"
    " cellphone = $9,"
    " fax = $10,"
    " email = $11,"
    " website = $12,"
    " smtp_server = $13,"
    " smtp_username = $14,"
    " smtp_password = $15,"
    " smtp_port = $16"
    " WHERE company_id = 0;");
  
  l_result= PQexecParams(
    (*io_session).m_connection,
    l_statement,
    16,
    0,
    l_values,
    0,
    0,
    0);

  l_status= PQresultStatus(l_result);

  if (PGRES_COMMAND_OK != l_status)
  {
    l_error= g_error_new(
      domain_company,
      error_company_unable_to_update,
      "Unable to update company table: '%s'",
      PQerrorMessage((*io_session).m_connection));
    l_exit= -1;
  }

  g_free(l_password);
  g_free(l_statement);
  PQclear(l_result);

  if (l_error)
  {
    g_propagate_error(o_error, l_error);
  }

  return l_exit;
}

extern int
company_fetch(
  GError**                              o_error,
  struct company*const                  o_company,
  struct session*const                  io_session)
{
  GError*                               l_error;
  int                                   l_exit;
  gchar*                                l_password;
  PGresult*                             l_result;
  gchar*                                l_statement;
  ExecStatusType                        l_status;
  char*                                 l_text;
  int                                   l_tuples;

  memset(o_company, 0, sizeof(*o_company));
  l_error= 0;
  l_exit= 0;
  l_result= 0;

  do
  {

    l_statement= g_malloc0(1024);

    g_snprintf(
      l_statement,
      1024,
      "SELECT"
      " company_name,"
      " street1,"
      " street2,"
      " street3,"
      " city,"
      " state,"
      " zipcode,"
      " phone,"
      " cellphone,"
      " fax,"
      " email,"
      " website,"
      " smtp_server,"
      " smtp_username,"
      " smtp_password,"
      " smtp_port"
      " FROM company"
      " WHERE company_id = 0"
      " LIMIT 1;");

    l_result= PQexec((*io_session).m_connection, l_statement);

    g_free(l_statement);
    l_statement= 0;

    l_status= PQresultStatus(l_result);

    if (PGRES_TUPLES_OK != l_status)
    {
      l_error= g_error_new(
        domain_company,
        error_company_unable_to_fetch_row,
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
    g_strlcpy((*o_company).m_company_name, l_text, sizeof((*o_company).m_company_name));

    l_text= PQgetvalue(l_result, 0, 1);
    g_strlcpy((*o_company).m_street1, l_text, sizeof((*o_company).m_street1));

    l_text= PQgetvalue(l_result, 0, 2);
    g_strlcpy((*o_company).m_street2, l_text, sizeof((*o_company).m_street2));

    l_text= PQgetvalue(l_result, 0, 3);
    g_strlcpy((*o_company).m_street3, l_text, sizeof((*o_company).m_street3));

    l_text= PQgetvalue(l_result, 0, 4);
    g_strlcpy((*o_company).m_city, l_text, sizeof((*o_company).m_city));

    l_text= PQgetvalue(l_result, 0, 5);
    g_strlcpy((*o_company).m_state, l_text, sizeof((*o_company).m_state));

    l_text= PQgetvalue(l_result, 0, 6);
    g_strlcpy((*o_company).m_zipcode, l_text, sizeof((*o_company).m_zipcode));

    l_text= PQgetvalue(l_result, 0, 7);
    g_strlcpy((*o_company).m_phone, l_text, sizeof((*o_company).m_phone));

    l_text= PQgetvalue(l_result, 0, 8);
    g_strlcpy((*o_company).m_cellphone, l_text, sizeof((*o_company).m_cellphone));

    l_text= PQgetvalue(l_result, 0, 9);
    g_strlcpy((*o_company).m_fax, l_text, sizeof((*o_company).m_fax));

    l_text= PQgetvalue(l_result, 0, 10);
    g_strlcpy((*o_company).m_email, l_text, sizeof((*o_company).m_email));

    l_text= PQgetvalue(l_result, 0, 11);
    g_strlcpy((*o_company).m_website, l_text, sizeof((*o_company).m_website));

    l_text= PQgetvalue(l_result, 0, 12);
    g_strlcpy((*o_company).m_smtp_server, l_text, sizeof((*o_company).m_smtp_server));

    l_text= PQgetvalue(l_result, 0, 13);
    g_strlcpy((*o_company).m_smtp_username, l_text, sizeof((*o_company).m_smtp_username));

    l_text= PQgetvalue(l_result, 0, 14);
    l_password= password_decrypt(io_session, l_text);

    if (l_password)
    {
      g_strlcpy((*o_company).m_smtp_password, l_password, sizeof((*o_company).m_smtp_password));
      g_free(l_password);
    }

    l_text= PQgetvalue(l_result, 0, 15);
    g_strlcpy((*o_company).m_smtp_port, l_text, sizeof((*o_company).m_smtp_port));

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
