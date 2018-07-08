/*
  session.c

  License CC0 PUBLIC DOMAIN

  To the extent possible under law, Mark J. Olesen has waived all copyright
  and related or neighboring rights to tabitha. This work is published from:
  United States.
*/
#include "session.h"
#include "error.h"
#include <string.h>

extern void
cursor_assign(
  struct cursor*const                  o_cursor)
{

  memset(o_cursor, 0, sizeof(*o_cursor));

  return;
}

extern void
cursor_discharge(
  struct cursor*const                  io_cursor)
{

  PQclear((*io_cursor).m_result);
  memset(io_cursor, 0, sizeof(*io_cursor));

  return;
}

extern void
session_assign(
  struct session*const                  o_session)
{

  memset(o_session, 0, sizeof(*o_session));

  return;
}

#define DBNAME                          0
#define HOST                            1
#define HOSTADDR                        2
#define PORT                            3
#define USER                            4
#define PASSWORD                        5
#define END                             6
#define PARMS                           7

extern int
session_connect(
  GError**                              o_error,
  struct session*const                  io_session,
  struct aspect const*const             i_aspect)
{
  PGconn*                               l_connection;
  GError*                               l_error;
  int                                   l_exit;
  char const*                           l_keys[PARMS];
  ConnStatusType                        l_status;
  char const*                           l_values[PARMS];

  l_exit= 0;

  do
  {

    l_keys[DBNAME]= "dbname";
    l_keys[HOST]= "host";
    l_keys[HOSTADDR]= "hostaddr";
    l_keys[PORT]= "port";
    l_keys[USER]= "user";
    l_keys[PASSWORD]= "password";
    l_keys[END]= 0;

    l_values[DBNAME]= (*i_aspect).m_dbname;
    l_values[HOST]= (*i_aspect).m_hostname;
    l_values[HOSTADDR]= (*i_aspect).m_ipaddress;
    l_values[PORT]= (*i_aspect).m_port;
    l_values[USER]= (*i_aspect).m_username;
    l_values[PASSWORD]= (*i_aspect).m_password;
    l_values[END]= 0;

    l_connection= PQconnectdbParams(l_keys, l_values, 0);

    l_status= PQstatus(l_connection);

    if (CONNECTION_OK != l_status)
    {
      l_error= g_error_new(
        domain_session,
        error_session_unable_to_connect,
        "Unable to connect to database: '%s'",
        PQerrorMessage(l_connection));
      l_exit= -1;
      break;
    }

    (*io_session).m_connection= l_connection;

  }while(0);

  if (l_error)
  {
    g_propagate_error(o_error, l_error);
  }

  return l_exit;
}

extern void
session_discharge(
  struct session*const                  io_session)
{

  session_disconnect(io_session);
  memset(io_session, 0, sizeof(*io_session));

  return;
}

extern void
session_disconnect(
  struct session*const                  io_session)
{

  if ((*io_session).m_connection)
  {
    PQfinish((*io_session).m_connection);
    (*io_session).m_connection= 0;
  }

  return;
}
/* 
vim:expandtab:softtabstop=2:tabstop=2:shiftwidth=2:nowrap:ruler 
*/
