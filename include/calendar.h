/*
  calendar.h

  License CC0 PUBLIC DOMAIN

  To the extent possible under law, Mark J. Olesen has waived all copyright
  and related or neighboring rights to tabitha. This work is published from:
  United States.
*/
#if !defined(__calendar_h__)

#include <gtk/gtk.h>

struct calendar
{
  gboolean                              m_is_set;
  guint                                 m_month;
  guint                                 m_day;
  guint                                 m_year;
  guint                                 m_hour;
  guint                                 m_minute;
};

extern void
calendar_set_now(
  struct calendar*const                 o_calendar);

extern gchar*
calendar_format_now();

extern int
calendar_form(
  struct calendar*const                 io_object,
  GtkWindow*const                       io_parent,
  GtkBuilder*const                      io_builder,
  gint const                            i_pos_x,
  gint const                            i_pos_y);

extern gchar*
calendar_format(
  struct calendar const*const           i_calendar);

#define __calendar_h__
#endif
/* 
vim:expandtab:softtabstop=2:tabstop=2:shiftwidth=2:nowrap:ruler 
*/
