/*
  edit_form.c

  License CC0 PUBLIC DOMAIN

  To the extent possible under law, Mark J. Olesen has waived all copyright
  and related or neighboring rights to tabitha. This work is published from:
  United States.
*/
#include "edit.h"
#include "error.h"
#include <gmodule.h>

struct property
{
  GtkTextView*                          m_textview;
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

    (*o_property).m_textview=
      GTK_TEXT_VIEW(gtk_builder_get_object(io_builder, "edit_textview"));

    if (0 == (*o_property).m_textview)
    {
      break;
    }

    l_exit= 0;

  }while(0);

  return l_exit;
}

static void
set(
  struct property*const                 i_property,
  gchar const*                          i_text)
{
  GtkTextBuffer*                        l_buffer;

  l_buffer= gtk_text_view_get_buffer((*i_property).m_textview);
  gtk_text_buffer_set_text(l_buffer, i_text, -1);

  return;
}

static void
copy(
  gchar**                               io_text,
  struct property const*const           i_property)
{
  GtkTextBuffer*                        l_buffer;
  GtkTextIter                           l_end;
  GtkTextIter                           l_start;

  g_free((*io_text));
  l_buffer= gtk_text_view_get_buffer((*i_property).m_textview);
  gtk_text_buffer_get_start_iter(l_buffer, &l_start);
  gtk_text_buffer_get_end_iter(l_buffer, &l_end);
  (*io_text)= gtk_text_buffer_get_text(l_buffer, &l_start, &l_end, FALSE);

  return;
}

G_MODULE_EXPORT void
on_edit_paste_clicked(
  GtkToolButton*const                   io_toolbutton,
  gpointer                              io_user_data)
{
  return;
}

static int
internal_form(
  gchar**                               io_text,
  GtkWindow*const                       io_parent,
  GtkBuilder*const                      io_builder)
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

    l_dialog= GTK_DIALOG(gtk_builder_get_object(io_builder, "dialog_edit"));

    if (0 == l_dialog)
    {
      l_error= g_error_new(
        domain_general,
        error_generic,
        "Unable to find dialog: '%s'",
        "dialog_edit");
      break;
    }

    gtk_window_set_transient_for(GTK_WINDOW(l_dialog), io_parent);

    l_exit= bind(l_property, io_builder);

    if (l_exit)
    {
      l_error= g_error_new(
        domain_general,
        error_generic,
        "Unable to load dialog: '%s'",
        "dialog_edit");
      break;
    }

    set(l_property, (*io_text));

    g_object_set_data(G_OBJECT(l_dialog), "builder", io_builder);
    g_object_set_data(G_OBJECT(l_dialog), "property", &l_property);

    gtk_widget_show_all(GTK_WIDGET(l_dialog));
    gtk_window_set_modal(GTK_WINDOW(l_dialog), 1);

    l_exit= gtk_dialog_run(l_dialog);

    if (GTK_RESPONSE_OK != l_exit)
    {
      break;
    }

    copy(io_text, l_property);
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

extern int
edit_form(
  GtkTextView*const                     io_textview,
  GtkWindow*const                       io_parent,
  GtkBuilder*const                      io_builder)
{
  GtkTextBuffer*                        l_buffer;
  GtkTextIter                           l_end;
  int                                   l_exit;
  GtkTextIter                           l_start;
  gchar*                                l_text;

  l_buffer= gtk_text_view_get_buffer(io_textview);
  gtk_text_buffer_get_start_iter(l_buffer, &l_start);
  gtk_text_buffer_get_end_iter(l_buffer, &l_end);
  l_text= gtk_text_buffer_get_text(l_buffer, &l_start, &l_end, FALSE);

  l_exit= internal_form(&l_text, io_parent, io_builder);

  if (0 == l_exit)
  {
    gtk_text_buffer_set_text(l_buffer, l_text, -1);
  }

  g_free(l_text);

  return l_exit;
}
/* 
vim:expandtab:softtabstop=2:tabstop=2:shiftwidth=2:nowrap:ruler 
*/
