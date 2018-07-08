/*
  edit.h

  License CC0 PUBLIC DOMAIN

  To the extent possible under law, Mark J. Olesen has waived all copyright
  and related or neighboring rights to tabitha. This work is published from:
  United States.
*/
#if !defined(__edit_h__)

#include <gtk/gtk.h>

extern int
edit_form(
  GtkTextView*const                     io_textview,
  GtkWindow*const                       io_parent,
  GtkBuilder*const                      io_builder);

#define __edit_h__
#endif
