/*
  company.h

  License CC0 PUBLIC DOMAIN

  To the extent possible under law, Mark J. Olesen has waived all copyright
  and related or neighboring rights to tabitha. This work is published from:
  United States.
*/
#if !defined(__company_h__)

#include "session.h"

struct company
{
  gchar                                 m_company_name[size_company_name];
  gchar                                 m_street1[size_street1];
  gchar                                 m_street2[size_street2];
  gchar                                 m_street3[size_street3];
  gchar                                 m_city[size_city];
  gchar                                 m_state[size_state];
  gchar                                 m_zipcode[size_zipcode];
  gchar                                 m_phone[size_phone];
  gchar                                 m_cellphone[size_cellphone];
  gchar                                 m_fax[size_fax];
  gchar                                 m_email[size_email];
  gchar                                 m_website[size_website];
  gchar                                 m_smtp_server[size_smtp_server];
  gchar                                 m_smtp_username[size_smtp_username];
  gchar                                 m_smtp_password[size_smtp_password];
  gchar                                 m_smtp_port[size_pg_small_int];
};

extern int
company_fetch(
  GError**                              o_error,
  struct company*const                  o_company,
  struct session*const                  io_session);

extern int
company_form(
  struct session*const                  io_session,
  GtkWindow*const                       io_parent,
  GtkBuilder*const                      io_builder);

extern void
company_trim(
  struct company*const                  io_company);

extern int
company_update(
  GError**                              o_error,
  struct session*const                  io_session,
  struct company const*const            i_company);

#define __company_h__
#endif
/* 
vim:expandtab:softtabstop=2:tabstop=2:shiftwidth=2:nowrap:ruler 
*/
