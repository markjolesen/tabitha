/*
  aspect.c

  License CC0 PUBLIC DOMAIN

  To the extent possible under law, Mark J. Olesen has waived all copyright
  and related or neighboring rights to tabitha. This work is published from:
  United States.
*/
#include "aspect.h"
#include <string.h>

extern void
aspect_set_defaults(
  struct aspect*const                   o_aspect)
{

  memset(o_aspect, 0, sizeof(*o_aspect));
  g_strlcpy((*o_aspect).m_dbname, "tabitha", sizeof((*o_aspect).m_dbname));
  g_strlcpy((*o_aspect).m_hostname, "localhost", sizeof((*o_aspect).m_hostname));
  g_strlcpy((*o_aspect).m_port, "5432", sizeof((*o_aspect).m_port));
  g_strlcpy((*o_aspect).m_username, "tabitha", sizeof((*o_aspect).m_username));

  return;
}
/* 
vim:expandtab:softtabstop=2:tabstop=2:shiftwidth=2:nowrap:ruler 
*/
