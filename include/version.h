/*
  version.h

  License CC0 PUBLIC DOMAIN

  To the extent possible under law, Mark J. Olesen has waived all copyright
  and related or neighboring rights to tabitha. This work is published from:
  United States.
*/
#if !defined(__version_h__)

struct version_stamp
{
  char                                  m_year[1+4];
  char                                  m_month[1+2];
  char                                  m_day[1+2];
};

enum version_stage
{
  stage_alpha                           = 'A',
  stage_beta                            = 'B',
  stage_candidate                       = 'C',
  stage_released                        = 'R' 
};

struct version
{
  unsigned                              m_major;
  unsigned                              m_minor;
  enum version_stage                    m_stage;
  unsigned                              m_micro;
  struct version_stamp                  m_stamp;
};

extern char const*
version_get_string();

#define __version_h__
#endif
/* 
vim:expandtab:softtabstop=2:tabstop=2:shiftwidth=2:nowrap:ruler 
*/
