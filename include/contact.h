/*
  contact.h

  License CC0 PUBLIC DOMAIN

  To the extent possible under law, Mark J. Olesen has waived all copyright
  and related or neighboring rights to tabitha. This work is published from:
  United States.
*/
#if !defined(__contact_h__)

#include "session.h"

enum contacttype
{
  residential                           = 0,
  commercial,
  vendor,
};

struct contact
{
  gchar                                 m_contact_id[size_contact_id];
  enum contacttype                      m_contact_type;
  gchar                                 m_company_name[size_company_name];
  gchar                                 m_first_name[size_first_name];
  gchar                                 m_last_name[size_last_name];
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
  gchar*                                m_notes;
};

extern int
contact_exists(
  GError**                              o_error,
  gboolean*                             o_exists,
  struct session*const                  io_session,
  gchar const                           i_contact_id[size_contact_id]);

extern int
contact_fetch(
  GError**                              o_error,
  struct contact*const                  o_contact,
  struct session*const                  io_session,
  gchar const                           i_contact_id[size_contact_id]);

extern int
contact_index_form(
  gchar                                 o_contact_id[size_contact_id],
  struct session*const                  io_session,
  GtkWindow*const                       io_parent,
  GtkBuilder*const                      io_builder);

extern int
contact_form(
  struct session*const                  io_session,
  GtkWindow*const                       io_parent,
  GtkBuilder*const                      io_builder);

extern int
contact_save(
  GError**                              o_error,
  struct session*const                  io_session,
  struct contact const*const            i_contact);

extern void
contact_trim(
  struct contact*const                  io_contact);

extern enum contacttype
contact_type_as_enum(
  gchar const*                          i_contact_type);

extern gchar const*
contact_type_as_string(
  enum contacttype const                i_contact_type);

#define __contact_h__
#endif
/* 
vim:expandtab:softtabstop=2:tabstop=2:shiftwidth=2:nowrap:ruler 
*/
