/*
  password.c

  License CC0 PUBLIC DOMAIN

  To the extent possible under law, Mark J. Olesen has waived all copyright
  and related or neighboring rights to tabitha. This work is published from:
  United States.
*/
#include "password.h"

static gchar const                      g_key[]= "bellaeva";
static gchar const                      g_algo[]= "bf";

extern gchar*
password_decrypt(
  struct session*const                  io_session,
  gchar const*                          i_encrypted)
{
  gchar*                                l_decode;
  gchar*                                l_plaintext;
  PGresult*                             l_result;
  gsize                                 l_size;
  GString*                              l_statement;
  ExecStatusType                        l_status;
  gchar const*                          l_text;

  l_decode= 0;
  l_plaintext= 0;
  l_result= 0;
  l_statement= g_string_sized_new(1024);

  do
  {

    l_decode= (gchar*)g_base64_decode(i_encrypted, &l_size);

    g_string_printf(
      l_statement,
      "SELECT convert_from(decrypt('%s', '%s', '%s'), 'SQL_ASCII');",
      l_decode,
      g_key,
      g_algo);

    l_result= PQexec((*io_session).m_connection, (*l_statement).str);

    l_status= PQresultStatus(l_result);

    if (PGRES_TUPLES_OK != l_status)
    {
      break;
    }

    l_text= PQgetvalue(l_result, 0, 0);
    l_plaintext= g_strdup(l_text);

  }while(0);

  g_free(l_decode);
  g_string_free(l_statement, TRUE);
  PQclear(l_result);

  return l_plaintext;
}

extern gchar*
password_encrypt(
  struct session*const                  io_session,
  gchar const*                          i_plaintext)
{
  gchar*                                l_encrypted;
  gchar*                                l_escaped;
  PGresult*                             l_result;
  GString*                              l_statement;
  ExecStatusType                        l_status;
  gchar const*                          l_text;

  l_encrypted= 0;
  l_result= 0;
  l_statement= g_string_sized_new(1024);

  do
  {

    l_escaped= PQescapeLiteral((*io_session).m_connection, i_plaintext, strlen(i_plaintext));

    g_string_printf(
      l_statement,
      "SELECT encrypt(%s, '%s', '%s');",
      l_escaped,
      g_key,
      g_algo);

    PQfreemem(l_escaped);
    l_escaped= 0;

    l_result= PQexec((*io_session).m_connection, (*l_statement).str);

    l_status= PQresultStatus(l_result);

    if (PGRES_TUPLES_OK != l_status)
    {
      break;
    }

    l_text= PQgetvalue(l_result, 0, 0);
    l_encrypted= g_base64_encode((guchar const*)l_text, (gsize)strlen(l_text));

  }while(0);

  g_string_free(l_statement, TRUE);
  PQclear(l_result);

  return l_encrypted;
}
/* 
vim:expandtab:softtabstop=2:tabstop=2:shiftwidth=2:nowrap:ruler 
*/
