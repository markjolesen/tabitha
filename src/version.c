/*
  version.c

  License CC0 PUBLIC DOMAIN

  To the extent possible under law, Mark J. Olesen has waived all copyright
  and related or neighboring rights to tabitha. This work is published from:
  United States.
*/
#include "version.h"
#include <stdio.h>

static struct version const             g_version=
{
  1,
  0,
  stage_beta,
  0,
  {
    "2018",
    "07",
    "29"
  }
};

extern char const*
version_get_string()
{
  static char                           l_version[40];

  snprintf(
      l_version, 
      sizeof(l_version),
      "%s-%s-%s-%c %u.%u.%u",
      g_version.m_stamp.m_year,
      g_version.m_stamp.m_month,
      g_version.m_stamp.m_day,
      g_version.m_stage,
      g_version.m_major,
      g_version.m_minor,
      g_version.m_micro);

  return l_version;
}
/* 
vim:expandtab:softtabstop=2:tabstop=2:shiftwidth=2:nowrap:ruler 
*/
