/*
  trim.c

  License CC0 PUBLIC DOMAIN

  To the extent possible under law, Mark J. Olesen has waived all copyright
  and related or neighboring rights to tabitha. This work is published from:
  United States.
*/
#include "trim.h"

extern void
trim_big_int(
  gchar                                 io_number[size_pg_big_int])
{
  guint64                               l_value;
  
  l_value= strtoul(io_number, 0, 10);

  memset(io_number, 0, size_pg_big_int);

  if (l_value)
  {
    g_snprintf(io_number, size_pg_big_int, "%lu", l_value);
  }

  return;
}

extern void
trim_money(
  gchar                                 io_money[size_money])
{
  gboolean                              l_bool;
  gchar                                 l_char;
  guint64                               l_digits1;
  guint                                 l_digits2;
  gchar                                 l_frac[3];
  gchar*                                l_head;
  gchar                                 l_money[size_money];
  gchar*                                l_tail;

  l_frac[0]= 0;
  l_frac[1]= 0;
  l_frac[2]= 0;
  l_head= &io_money[0];
  l_tail= &l_money[0];
  memset(l_money, 0, sizeof(l_money));

  do
  {

    l_char= l_head[0];

    if (0 == l_char)
    {
      break;
    }

    l_bool= g_ascii_isdigit(l_char);

    if (TRUE == l_bool)
    {
      break;
    }

    l_head++;

  }while(1);


  do
  {

    if (0 == l_char)
    {
      break;
    }

    l_tail[0]= l_char;
    l_tail++;

    l_head++;
    l_char= l_head[0];
    l_bool= g_ascii_isdigit(l_char);

    if (FALSE == l_bool)
    {
      if (',' != l_char)
      {
        break;
      }
    }

  }while(1);

  do
  {

    if (0 == l_char)
    {
      break;
    }

    if ('.' != l_char)
    {
      break;
    }

    l_head++;
    l_char= *l_head++;

    l_bool= g_ascii_isdigit(l_char);

    if (FALSE == l_bool)
    {
      break;
    }

    l_frac[0]= l_char;

    l_char= *l_head++;

    l_bool= g_ascii_isdigit(l_char);

    if (FALSE == l_bool)
    {
      break;
    }

    l_frac[1]= l_char;

  }while(0);

  memset(io_money, 0, size_money);
  l_digits1= strtoul(l_money, 0, 10);
  l_digits2= strtoul(l_frac, 0, 10);

  if (l_digits1)
  {
    if (l_digits2)
    {
      g_snprintf(io_money, size_money, "%lu.%02u", l_digits1, l_digits2);
    }
    else
    {
      g_snprintf(io_money, size_money, "%lu", l_digits1);
    }
  }
  else if (l_digits2)
  {
    g_snprintf(io_money, size_money, "0.%02u", l_digits2);
  }

  return;
}

extern void
trim_small_int(
  gchar                                 io_number[size_pg_small_int])
{
  guint                                 l_value;
  
  l_value= strtoul(io_number, 0, 10);

  if (G_MAXINT16 < l_value)
  {
    l_value= G_MAXINT16;
  }

  memset(io_number, 0, size_pg_small_int);

  if (l_value)
  {
    g_snprintf(io_number, size_pg_small_int, "%u", l_value);
  }

  return;
}
/* 
vim:expandtab:softtabstop=2:tabstop=2:shiftwidth=2:nowrap:ruler 
*/
