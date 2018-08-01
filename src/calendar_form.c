/*
  calendar_form.c

  License CC0 PUBLIC DOMAIN

  To the extent possible under law, Mark J. Olesen has waived all copyright
  and related or neighboring rights to tabitha. This work is published from:
  United States.
*/
#include "calendar.h"
#include "error.h"
#include <string.h>

struct property
{
  GtkCalendar*                          m_calendar;
};

static int
bind(
  struct property*const                 o_property,
  GtkBuilder*const                      io_builder)
{
  int                                   l_exit;

  memset(o_property, 0, sizeof(*o_property));
  l_exit= -1;

  do
  {

    (*o_property).m_calendar=
      GTK_CALENDAR(gtk_builder_get_object(io_builder, "calendar_calendar"));

    if (0 == (*o_property).m_calendar)
    {
      break;
    }

    l_exit= 0;

  }while(0);

  return l_exit;
}

static void
set(
  struct property*const                 io_property,
  struct calendar const*const           i_object)
{
  struct calendar                       l_cal;
  struct calendar const*                l_cur;

  if ((*i_object).m_is_set)
  {
    l_cur= i_object;
  }
  else
  {
    calendar_set_now(&l_cal);
    l_cur= &l_cal;
  }

  gtk_calendar_select_month(
    (*io_property).m_calendar,
    (*l_cur).m_month,
    (*l_cur).m_year);

  gtk_calendar_select_day((*io_property).m_calendar, (*l_cur).m_day);

  return;
}

static void
copy(
  struct calendar*const                 io_object,
  struct property const*const           i_property)
{

  memset(io_object, 0, sizeof(*io_object));

  gtk_calendar_get_date(
    (*i_property).m_calendar,
    &(*io_object).m_year,
    &(*io_object).m_month,
    &(*io_object).m_day);

  (*io_object).m_is_set= TRUE;

  return;
}

extern int
calendar_form(
  struct calendar*const                 io_object,
  GtkWindow*const                       io_parent,
  GtkBuilder*const                      io_builder,
  gint const                            i_pos_x,
  gint const                            i_pos_y)
{
  GtkDialog*                            l_dialog;
  GError*                               l_error;
  int                                   l_exit;
  struct property*                      l_property;

  l_dialog= 0;
  l_error= 0;
  l_exit= -1;
  l_property= (struct property*)g_malloc0(sizeof(*l_property));

  do
  {

    l_dialog= GTK_DIALOG(gtk_builder_get_object(io_builder, "dialog_calendar"));

    if (0 == l_dialog)
    {
      l_error= g_error_new(
        domain_general,
        error_generic,
        "Unable to find dialog: '%s'",
        "dialog_calendar");
      break;
    }

    gtk_window_set_transient_for(GTK_WINDOW(l_dialog), io_parent);
    gtk_window_move(GTK_WINDOW(l_dialog), i_pos_x, i_pos_y);

    l_exit= bind(l_property, io_builder);

    if (l_exit)
    {
      l_error= g_error_new(
        domain_general,
        error_generic,
        "Unable to load dialog: '%s'",
        "dialog_calendar");
      break;
    }

    set(l_property, io_object);

    gtk_widget_show_all(GTK_WIDGET(l_dialog));
    gtk_window_set_modal(GTK_WINDOW(l_dialog), 1);
    l_exit= gtk_dialog_run(l_dialog);

    if (GTK_RESPONSE_OK != l_exit)
    {
      break;
    }

    copy(io_object, l_property);
    l_exit= 0;

  }while(0);

  g_free(l_property);

  if (l_dialog)
  {
    gtk_widget_hide(GTK_WIDGET(l_dialog));
  }

  if (l_error)
  {
    _error_display(io_parent, l_error);
    g_clear_error(&l_error);
  }

  return l_exit;
}
/* 
vim:expandtab:softtabstop=2:tabstop=2:shiftwidth=2:nowrap:ruler 
*/
