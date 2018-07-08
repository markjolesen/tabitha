/*
  error.c

  License CC0 PUBLIC DOMAIN

  To the extent possible under law, Mark J. Olesen has waived all copyright
  and related or neighboring rights to tabitha. This work is published from:
  United States.
*/
#include "error.h"

void
_error_display(
  GtkWindow*                            i_parent,
  GError const*const                    i_error)
{
  GtkDialog*                            l_dialog;

  l_dialog= GTK_DIALOG(gtk_message_dialog_new(
    i_parent,
    GTK_DIALOG_MODAL,
    GTK_MESSAGE_ERROR,
    GTK_BUTTONS_CLOSE,
    "[%d.%d] %s",
    (*i_error).domain,
    (*i_error).code,
    (*i_error).message));

  gtk_dialog_run(l_dialog);
  gtk_widget_destroy(GTK_WIDGET(l_dialog));

  return;
}
/* 
vim:expandtab:softtabstop=2:tabstop=2:shiftwidth=2:nowrap:ruler 
*/
