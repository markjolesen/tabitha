/*
  trim.h

  License CC0 PUBLIC DOMAIN

  To the extent possible under law, Mark J. Olesen has waived all copyright
  and related or neighboring rights to tabitha. This work is published from:
  United States.
*/
#if !defined(__trim_h__)

#include "session.h"

extern void
trim_big_int(
  gchar                                 io_number[size_pg_big_int]);

extern void
trim_money(
  gchar                                 io_money[size_money]);

extern void
trim_small_int(
  gchar                                 io_number[size_pg_small_int]);

#define __trim_h__
#endif
/* 
vim:expandtab:softtabstop=2:tabstop=2:shiftwidth=2:nowrap:ruler 
*/
