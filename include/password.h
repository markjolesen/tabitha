/*
  password.h

  License CC0 PUBLIC DOMAIN

  To the extent possible under law, Mark J. Olesen has waived all copyright
  and related or neighboring rights to tabitha. This work is published from:
  United States.
*/
#if !defined(__password_h__)

#include <session.h>

extern gchar*
password_decrypt(
  struct session*const                  io_session,
  gchar const*                          i_encrypted);

extern gchar*
password_encrypt(
  struct session*const                  io_session,
  gchar const*                          i_plaintext);

#define __password_h__
#endif
/* 
vim:expandtab:softtabstop=2:tabstop=2:shiftwidth=2:nowrap:ruler 
*/
