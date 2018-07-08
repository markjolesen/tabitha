/*
  calendar.c

  License CC0 PUBLIC DOMAIN

  To the extent possible under law, Mark J. Olesen has waived all copyright
  and related or neighboring rights to tabitha. This work is published from:
  United States.
*/
#include "calendar.h"

extern gchar*
calendar_format(
  struct calendar const*const           i_calendar)
{
  gchar*                                l_text;

  l_text= g_malloc0(11);

  if ((*i_calendar).m_is_set)
  {
    g_snprintf(
      l_text,
      11,
      "%04d-%02d-%02d",
      (*i_calendar).m_year,
      (*i_calendar).m_month,
      (*i_calendar).m_day);
  }

  return l_text;
}

extern gchar*
calendar_format_now()
{
  struct calendar                       l_cal;
  gchar*                                l_format;

  calendar_set_now(&l_cal);
  l_format= calendar_format(&l_cal);

  return l_format;
}

extern void
calendar_set_now(
  struct calendar*const                 o_calendar)
{
  GDateTime*                            l_date;

  memset(o_calendar, 0, sizeof(*o_calendar));

  l_date= g_date_time_new_now_local();

  g_date_time_get_ymd(
    l_date,
    (gint*)&(*o_calendar).m_year,
    (gint*)&(*o_calendar).m_month,
    (gint*)&(*o_calendar).m_day); 

  (*o_calendar).m_hour= g_date_time_get_hour(l_date);
  (*o_calendar).m_minute= g_date_time_get_minute(l_date);
  (*o_calendar).m_is_set= TRUE;

  g_date_time_unref(l_date);

  return;
}
/* 
vim:expandtab:softtabstop=2:tabstop=2:shiftwidth=2:nowrap:ruler 
*/
