/*
  contact.c

  License CC0 PUBLIC DOMAIN

  To the extent possible under law, Mark J. Olesen has waived all copyright
  and related or neighboring rights to tabitha. This work is published from:
  United States.
*/
#include "contact.h"
#include "error.h"

extern void
contact_trim(
  struct contact*const                  io_contact)
{

  g_strstrip((*io_contact).m_contact_id);
  g_strstrip((*io_contact).m_company_name);
  g_strstrip((*io_contact).m_first_name);
  g_strstrip((*io_contact).m_last_name);
  g_strstrip((*io_contact).m_street1);
  g_strstrip((*io_contact).m_street2);
  g_strstrip((*io_contact).m_street3);
  g_strstrip((*io_contact).m_city);
  g_strstrip((*io_contact).m_state);
  g_strstrip((*io_contact).m_zipcode);
  g_strstrip((*io_contact).m_phone);
  g_strstrip((*io_contact).m_cellphone);
  g_strstrip((*io_contact).m_fax);
  g_strstrip((*io_contact).m_email);
  g_strstrip((*io_contact).m_website);

  return;
}

extern enum contacttype
contact_type_as_enum(
  gchar const*                          i_contact_type)
{
  int                                   l_rc;
  enum contacttype                      l_contact_type;

  do
  {

    l_rc= g_strcmp0("Residential", i_contact_type);

    if (0 == l_rc)
    {
      l_contact_type= residential;
      break;
    }

    l_rc= g_strcmp0("Commercial", i_contact_type);

    if (0 == l_rc)
    {
      l_contact_type= commercial;
      break;
    }

    l_contact_type= vendor;

  }while(0);

  return l_contact_type;
}

extern gchar const*
contact_type_as_string(
  enum contacttype const                i_contact_type)
{
  gchar const*                           l_type;

  switch(i_contact_type)
  {
    case residential:
      l_type= "Residential";
      break;
    case commercial:
      l_type= "Commercial";
      break;
    case vendor:
      l_type= "Vendor";
      break;
  }

  return l_type;
} 

static int
contact_insert(
  GError**                              o_error,
  struct session*const                  io_session,
  struct contact const*const            i_contact)
{
  GError*                               l_error;
  int                                   l_exit;
  PGresult*                             l_result;
  ExecStatusType                        l_status;
  gchar const*                          l_values[17];

  l_error= 0;
  l_exit= 0;
  l_result= 0;

  l_values[0]= __nullify(&(*i_contact).m_contact_id[0]);
  l_values[1]= contact_type_as_string((*i_contact).m_contact_type);
  l_values[2]= __nullify(&(*i_contact).m_company_name[0]);
  l_values[3]= __nullify(&(*i_contact).m_first_name[0]);
  l_values[4]= __nullify(&(*i_contact).m_last_name[0]);
  l_values[5]= __nullify(&(*i_contact).m_street1[0]);
  l_values[6]= __nullify(&(*i_contact).m_street2[0]);
  l_values[7]= __nullify(&(*i_contact).m_street3[0]);
  l_values[8]= __nullify(&(*i_contact).m_city[0]);
  l_values[9]= __nullify(&(*i_contact).m_state[0]);
  l_values[10]= __nullify(&(*i_contact).m_zipcode[0]);
  l_values[11]= __nullify(&(*i_contact).m_phone[0]);
  l_values[12]= __nullify(&(*i_contact).m_cellphone[0]);
  l_values[13]= __nullify(&(*i_contact).m_fax[0]);
  l_values[14]= __nullify(&(*i_contact).m_email[0]);
  l_values[15]= __nullify(&(*i_contact).m_website[0]);
  l_values[16]= __nullify((*i_contact).m_notes);
  
  l_result= PQexecParams(
    (*io_session).m_connection,
    "INSERT INTO contact ("
    "contact_id,"
    "contact_type,"
    "company_name,"
    "first_name,"
    "last_name,"
    "street1,"
    "street2,"
    "street3,"
    "city,"
    "state,"
    "zipcode,"
    "phone,"
    "cellphone,"
    "fax,"
    "email,"
    "website,"
    "notes)"
    " VALUES "
    "($1,$2,$3,$4,$5,$6,$7,$8,$9,$10,$11,$12,$13,$14,$15,$16,$17);",
    17,
    0,
    l_values,
    0,
    0,
    0);

  l_status= PQresultStatus(l_result);

  if (PGRES_COMMAND_OK != l_status)
  {
    l_error= g_error_new(
      domain_contact,
      error_contact_unable_to_insert,
      "Unable to insert into contact table: '%s'",
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
contact_update(
  GError**                              o_error,
  struct session*const                  io_session,
  struct contact const*const            i_contact)
{
  GError*                               l_error;
  int                                   l_exit;
  PGresult*                             l_result;
  gchar*                                l_statement;
  ExecStatusType                        l_status;
  gchar const*                          l_values[16];

  l_error= 0;
  l_exit= 0;
  l_result= 0;

  l_values[0]= contact_type_as_string((*i_contact).m_contact_type);
  l_values[1]= __nullify(&(*i_contact).m_company_name[0]);
  l_values[2]= __nullify(&(*i_contact).m_first_name[0]);
  l_values[3]= __nullify(&(*i_contact).m_last_name[0]);
  l_values[4]= __nullify(&(*i_contact).m_street1[0]);
  l_values[5]= __nullify(&(*i_contact).m_street2[0]);
  l_values[6]= __nullify(&(*i_contact).m_street3[0]);
  l_values[7]= __nullify(&(*i_contact).m_city[0]);
  l_values[8]= __nullify(&(*i_contact).m_state[0]);
  l_values[9]= __nullify(&(*i_contact).m_zipcode[0]);
  l_values[10]= __nullify(&(*i_contact).m_phone[0]);
  l_values[11]= __nullify(&(*i_contact).m_cellphone[0]);
  l_values[12]= __nullify(&(*i_contact).m_fax[0]);
  l_values[13]= __nullify(&(*i_contact).m_email[0]);
  l_values[14]= __nullify(&(*i_contact).m_website[0]);
  l_values[15]= __nullify((*i_contact).m_notes);

  l_statement= g_malloc0(1024);

  g_snprintf(
    l_statement,
    1024,
    "UPDATE contact"
    " SET "
    "contact_type = $1,"
    "company_name = $2,"
    "first_name = $3,"
    "last_name = $4,"
    "street1 = $5,"
    "street2 = $6,"
    "street3 = $7,"
    "city = $8,"
    "state = $9,"
    "zipcode = $10,"
    "phone = $11,"
    "cellphone = $12,"
    "fax = $13,"
    "email = $14,"
    "website = $15,"
    "notes = $16"
    " WHERE contact_id = '%s';",
    (*i_contact).m_contact_id);
  
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
      domain_contact,
      error_contact_unable_to_update,
      "Unable to update contact table: '%s'",
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
contact_exists(
  GError**                              o_error,
  gboolean*                             o_exists,
  struct session*const                  io_session,
  gchar const                           i_contact_id[size_contact_id])
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

    l_escaped= PQescapeLiteral((*io_session).m_connection, i_contact_id, strlen(i_contact_id));

    l_statement= g_malloc0(256);

    g_snprintf(
      l_statement,
      256,
      " SELECT EXISTS "
      " (SELECT true from contact" 
      " WHERE contact_id = %s);",
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
        domain_contact,
        error_contact_unable_to_count_rows,
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
contact_save(
  GError**                              o_error,
  struct session*const                  io_session,
  struct contact const*const            i_contact)
{
  GError*                               l_error;
  int                                   l_exit;
  gboolean                              l_exists;

  l_error= 0;
  l_exit= 0;

  l_exit= contact_exists(&l_error, &l_exists, io_session, (*i_contact).m_contact_id);
  
  if (0 == l_exit)
  {
    if (l_exists)
    {
      l_exit= contact_update(&l_error, io_session, i_contact);
    }
    else
    {
      l_exit= contact_insert(&l_error, io_session, i_contact);
    }
  }

  if (l_error)
  {
    g_propagate_error(o_error, l_error);
  }

  return l_exit;
}

extern int
contact_fetch(
  GError**                              o_error,
  struct contact*const                  o_contact,
  struct session*const                  io_session,
  gchar const                           i_contact_id[size_contact_id])

{
  gchar                                 l_contact_id[size_contact_id];
  GError*                               l_error;
  char*                                 l_escaped;
  int                                   l_exit;
  PGresult*                             l_result;
  gchar*                                l_statement;
  ExecStatusType                        l_status;
  char*                                 l_text;
  int                                   l_tuples;

  g_strlcpy(l_contact_id, i_contact_id, size_contact_id);

  memset(o_contact, 0, sizeof(*o_contact));
  l_error= 0;
  l_exit= 0;
  l_result= 0;

  do
  {

    l_escaped= PQescapeLiteral((*io_session).m_connection, l_contact_id, strlen(l_contact_id));

    l_statement= g_malloc0(1024);

    g_snprintf(
      l_statement,
      1024,
      "SELECT "
      "contact_id,"
      "contact_type,"
      "company_name,"
      "first_name,"
      "last_name,"
      "street1,"
      "street2,"
      "street3,"
      "city,"
      "state,"
      "zipcode,"
      "phone,"
      "cellphone,"
      "fax,"
      "email,"
      "website,"
      "notes"
      " FROM contact"
      " WHERE contact_id = %s"
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
        domain_contact,
        error_contact_unable_to_fetch_row,
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
    g_strlcpy((*o_contact).m_contact_id, l_text, sizeof((*o_contact).m_contact_id));

    l_text= PQgetvalue(l_result, 0, 1);
    (*o_contact).m_contact_type= contact_type_as_enum(l_text);

    l_text= PQgetvalue(l_result, 0, 2);
    g_strlcpy((*o_contact).m_company_name, l_text, sizeof((*o_contact).m_company_name));

    l_text= PQgetvalue(l_result, 0, 3);
    g_strlcpy((*o_contact).m_first_name, l_text, sizeof((*o_contact).m_first_name));

    l_text= PQgetvalue(l_result, 0, 4);
    g_strlcpy((*o_contact).m_last_name, l_text, sizeof((*o_contact).m_last_name));

    l_text= PQgetvalue(l_result, 0, 5);
    g_strlcpy((*o_contact).m_street1, l_text, sizeof((*o_contact).m_street1));

    l_text= PQgetvalue(l_result, 0, 6);
    g_strlcpy((*o_contact).m_street2, l_text, sizeof((*o_contact).m_street2));

    l_text= PQgetvalue(l_result, 0, 7);
    g_strlcpy((*o_contact).m_street3, l_text, sizeof((*o_contact).m_street3));

    l_text= PQgetvalue(l_result, 0, 8);
    g_strlcpy((*o_contact).m_city, l_text, sizeof((*o_contact).m_city));

    l_text= PQgetvalue(l_result, 0, 9);
    g_strlcpy((*o_contact).m_state, l_text, sizeof((*o_contact).m_state));

    l_text= PQgetvalue(l_result, 0, 10);
    g_strlcpy((*o_contact).m_zipcode, l_text, sizeof((*o_contact).m_zipcode));

    l_text= PQgetvalue(l_result, 0, 11);
    g_strlcpy((*o_contact).m_phone, l_text, sizeof((*o_contact).m_phone));

    l_text= PQgetvalue(l_result, 0, 12);
    g_strlcpy((*o_contact).m_cellphone, l_text, sizeof((*o_contact).m_cellphone));

    l_text= PQgetvalue(l_result, 0, 13);
    g_strlcpy((*o_contact).m_fax, l_text, sizeof((*o_contact).m_fax));

    l_text= PQgetvalue(l_result, 0, 14);
    g_strlcpy((*o_contact).m_email, l_text, sizeof((*o_contact).m_email));

    l_text= PQgetvalue(l_result, 0, 15);
    g_strlcpy((*o_contact).m_website, l_text, sizeof((*o_contact).m_website));

    l_text= PQgetvalue(l_result, 0, 16);
    (*o_contact).m_notes= g_strdup(l_text);

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
