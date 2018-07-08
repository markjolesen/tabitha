/*
  sales_print.c

  License CC0 PUBLIC DOMAIN

  To the extent possible under law, Mark J. Olesen has waived all copyright
  and related or neighboring rights to tabitha. This work is published from:
  United States.
*/
#include "sales.h"
#include "company.h"
#include "contact.h"
#include "calendar.h"
#define DECNUMDIGITS 19
#include "decNumber.h"
#include "error.h"
#include <math.h>

static char const estimate_disclaimer[]= {
  "This estimate is not a contract or a bill."
  " It is our best guess at the total price to complete the work stated above,"
  " based upon our initial inspection, but may be subject to change."
  " If prices change or additional parts and labor are required,"
  " we will inform you prior to proceeding with the work."
};

enum
{
  detail_lines_page_first= 12,
  detail_lines_page_nth= 30
};

struct print_data
{
  gchar                                 m_sales_id[size_pg_big_int];
  struct session*                       m_session;
  struct cursor                         m_cursor;
  GtkWindow*                            m_parent;
  struct company                        m_company;
  struct sales                          m_sales;
  struct contact                        m_contact;
  gchar*                                m_today;
  decContext                            m_set;
  decNumber                             m_subtotal;
  decNumber                             m_paid;
};

static gint
print_service_contact(
  PangoLayout*const                     io_layout,
  cairo_t*const                         io_cr,
  gchar const*                          i_contact,
  gint const                            i_pos_x,
  gint const                            i_pos_y,
  gint const                            i_advance)
{
  gchar                                 l_buffer[512];
  gchar*                                l_head;
  gint                                  l_line;
  gint                                  l_pos_y;
  gchar const*                          l_tail;

  l_buffer[0]= 0;
  l_head= l_buffer;
  l_line= 0;
  l_tail= i_contact;
  l_pos_y= i_pos_y + i_advance;

  do
  {

    if (0 == l_tail[0] || '\n' == l_tail[0])
    {

      l_head[0]= 0;
      g_strstrip(l_buffer);

      if (l_buffer[0])
      {
        pango_layout_set_text(io_layout, l_buffer, -1);
        cairo_move_to(io_cr, i_pos_x, l_pos_y);
        pango_cairo_show_layout (io_cr, io_layout);  
        l_pos_y+= i_advance;
        l_line++;
        if (10 == l_line)
        {
          break;
        }
      }
    
      if (0 == l_tail[0])
      {
        break;
      }

      l_buffer[0]= 0;
      l_head= l_buffer;
      l_tail++;

    }

    if ('\r' != l_tail[0])
    {
      *l_head++= *l_tail;
    }
    
    l_tail++;

  }while(1);

  if (l_line)
  {
    pango_layout_set_text(io_layout, "SERVICE:", -1);
    cairo_move_to(io_cr, i_pos_x, i_pos_y);
    pango_cairo_show_layout (io_cr, io_layout);  
  }

  return l_pos_y;
}

static gint
print_contact(
  PangoLayout*const                     io_layout,
  cairo_t*const                         io_cr,
  struct contact const*const            i_contact,
  gint const                            i_pos_y,
  gint const                            i_advance)
{
  gchar                                 l_buffer[512];
  gint                                  l_pos_y;

  l_pos_y= i_pos_y;

  pango_layout_set_text(io_layout, "TO:", -1);
  cairo_move_to(io_cr, 0, l_pos_y);
  pango_cairo_show_layout (io_cr, io_layout);  
  l_pos_y+= i_advance;

  if ((*i_contact).m_company_name[0])
  {
    pango_layout_set_text(io_layout, (*i_contact).m_company_name, -1);
    cairo_move_to(io_cr, 0, l_pos_y);
    pango_cairo_show_layout (io_cr, io_layout);  
    l_pos_y+= i_advance;
  }

  g_snprintf(
    l_buffer,
    sizeof(l_buffer),
    "%s %s", 
    (*i_contact).m_first_name,
    (*i_contact).m_last_name);

  g_strstrip(l_buffer);

  if (l_buffer[0])
  {
    pango_layout_set_text(io_layout, l_buffer, -1);
    cairo_move_to(io_cr, 0, l_pos_y);
    pango_cairo_show_layout (io_cr, io_layout);  
    l_pos_y+= i_advance;
  }

  
  if ((*i_contact).m_street1[0])
  {
    pango_layout_set_text(io_layout, (*i_contact).m_street1, -1);
    cairo_move_to(io_cr, 0, l_pos_y);
    pango_cairo_show_layout (io_cr, io_layout);  
    l_pos_y+= i_advance;
  }

  if ((*i_contact).m_street2[0])
  {
    pango_layout_set_text(io_layout, (*i_contact).m_street2, -1);
    cairo_move_to(io_cr, 0, l_pos_y);
    pango_cairo_show_layout (io_cr, io_layout);  
    l_pos_y+= i_advance;
  }

  if ((*i_contact).m_street3[0])
  {
    pango_layout_set_text(io_layout, (*i_contact).m_street3, -1);
    cairo_move_to(io_cr, 0, l_pos_y);
    pango_cairo_show_layout (io_cr, io_layout);  
    l_pos_y+= i_advance;
  }

  g_snprintf(
    l_buffer,
    sizeof(l_buffer),
    "%s, %s %s",
    (*i_contact).m_city,
    (*i_contact).m_state,
    (*i_contact).m_zipcode);

  pango_layout_set_text(io_layout, l_buffer, -1);
  cairo_move_to(io_cr, 0, l_pos_y);
  pango_cairo_show_layout (io_cr, io_layout);  
  l_pos_y+= i_advance;

  return l_pos_y;
}

static gint
print_page0_header2(
  PangoLayout*const                     io_layout,
  cairo_t*const                         io_cr,
  struct sales const*const              i_sales,
  gint const                            i_page_width,
  gint const                            i_pos_y,
  gint const                            i_advance)
{
  gint                                  l_boxh;
  gint                                  l_pos_y;
  gint                                  l_pos_y2;

  l_boxh= (4+i_advance);
  l_pos_y= i_pos_y;
  l_pos_y2= (i_pos_y + (2 * l_boxh));

  cairo_set_source_rgba(io_cr, 0.0, 0.0, 0.0, 1.0);
  cairo_set_line_width(io_cr, 1);
  cairo_rectangle(io_cr, 0, l_pos_y, i_page_width, l_boxh);
  cairo_stroke_preserve(io_cr);
  cairo_set_source_rgba(io_cr, 0.76, 0.76, 0.76, 1.0);
  cairo_fill(io_cr);
  cairo_stroke(io_cr);

  cairo_set_line_width(io_cr, 1);
  cairo_set_source_rgba(io_cr, 0.0, 0.0, 0.0, 1.0);
  cairo_rectangle(io_cr, 0, l_pos_y+l_boxh, i_page_width, l_boxh);
  cairo_move_to(io_cr, 200, l_pos_y);
  cairo_line_to(io_cr, 200, l_pos_y2);
  cairo_move_to(io_cr, 325, l_pos_y);
  cairo_line_to(io_cr, 325, l_pos_y2);
  cairo_move_to(io_cr, 450, l_pos_y);
  cairo_line_to(io_cr, 450, l_pos_y2);
  cairo_stroke(io_cr);

  l_pos_y+= 2;

  pango_layout_set_text(io_layout, "Purchase Order", -1);
  cairo_move_to(io_cr, 5, l_pos_y);
  pango_cairo_show_layout (io_cr, io_layout);  

  pango_layout_set_text(io_layout, "Sales Date", -1);
  cairo_move_to(io_cr, 205, l_pos_y);
  pango_cairo_show_layout (io_cr, io_layout);  

  pango_layout_set_text(io_layout, "Completed Date", -1);
  cairo_move_to(io_cr, 330, l_pos_y);
  pango_cairo_show_layout (io_cr, io_layout);  

  pango_layout_set_text(io_layout, "Due Date", -1);
  cairo_move_to(io_cr, 455, l_pos_y);
  pango_cairo_show_layout (io_cr, io_layout);  

  l_pos_y+= l_boxh;

  if ((*i_sales).m_customer_po[0])
  {
    pango_layout_set_text(io_layout, (*i_sales).m_customer_po, -1);
    cairo_move_to(io_cr, 5, l_pos_y);
    pango_cairo_show_layout (io_cr, io_layout);  
  }

  if ((*i_sales).m_sales_date[0])
  {
    pango_layout_set_text(io_layout, (*i_sales).m_sales_date, -1);
    cairo_move_to(io_cr, 205, l_pos_y);
    pango_cairo_show_layout (io_cr, io_layout);  
  }

  if ((*i_sales).m_completed_date[0])
  {
    pango_layout_set_text(io_layout, (*i_sales).m_completed_date, -1);
    cairo_move_to(io_cr, 330, l_pos_y);
    pango_cairo_show_layout (io_cr, io_layout);  
  }

  if ((*i_sales).m_due_date[0])
  {
    pango_layout_set_text(io_layout, (*i_sales).m_due_date, -1);
    cairo_move_to(io_cr, 455, l_pos_y);
    pango_cairo_show_layout (io_cr, io_layout);  
  }

  l_pos_y+= l_boxh;

  return l_pos_y;
}

static gint
print_detail(
  PangoLayout*const                     io_layout,
  cairo_t*const                         io_cr,
  struct print_data*const               io_data,
  gint const                            i_page_width,
  gint const                            i_page_height,
  gint const                            i_pos_y,
  gint const                            i_advance,
  guint const                           i_detail_lines)
{
  gint                                  l_boxh;
  gchar                                 l_buffer[512];
  struct sales_detail*                  l_detail;
  gboolean                              l_eof;
  gboolean                              l_even;
  decNumber                             l_line_total;
  gint                                  l_pos_y;
  gint                                  l_pos_y2;
  gint                                  l_push_y;
  decNumber                             l_qty;
  gint                                  l_return_y;
  gint                                  l_row;
  gint                                  l_text_width;
  decNumber                             l_unit_price;

  l_detail= g_malloc0(sizeof(*l_detail));

  l_boxh= (4+i_advance);
  l_pos_y= i_pos_y;
  l_pos_y2= (i_pos_y + (2 * l_boxh));

  cairo_set_source_rgba(io_cr, 0.0, 0.0, 0.0, 1.0);
  cairo_set_line_width(io_cr, 1);
  cairo_rectangle(io_cr, 0, l_pos_y, i_page_width, l_boxh);
  cairo_stroke_preserve(io_cr);
  cairo_set_source_rgba(io_cr, 0.76, 0.76, 0.76, 1.0);
  cairo_fill(io_cr);
  cairo_stroke(io_cr);
  l_pos_y+= 2;

  cairo_set_line_width(io_cr, 1);
  cairo_set_source_rgba(io_cr, 0.0, 0.0, 0.0, 1.0);

  pango_layout_set_text(io_layout, "Product ID", -1);
  cairo_move_to(io_cr, 5, l_pos_y);
  pango_cairo_show_layout (io_cr, io_layout);  

  pango_layout_set_text(io_layout, "Description", -1);
  cairo_move_to(io_cr, 105, l_pos_y);
  pango_cairo_show_layout (io_cr, io_layout);  

  pango_layout_set_text(io_layout, "Qty/Hours", -1);
  cairo_move_to(io_cr, 305, l_pos_y);
  pango_cairo_show_layout (io_cr, io_layout);  

  pango_layout_set_text(io_layout, "Unit Price", -1);
  cairo_move_to(io_cr, 380, l_pos_y);
  pango_cairo_show_layout (io_cr, io_layout);  

  pango_layout_set_text(io_layout, "Line Total", -1);
  cairo_move_to(io_cr, 490, l_pos_y);
  pango_cairo_show_layout (io_cr, io_layout);  

  l_pos_y-= 2;
  l_push_y= l_pos_y;

  for (l_row= 0; i_detail_lines > l_row; l_row++)
  {

    cairo_set_source_rgba(io_cr, 0.0, 0.0, 0.0, 1.0);
    cairo_rectangle(io_cr, 0, l_pos_y+l_boxh, i_page_width, l_boxh);
    cairo_stroke_preserve(io_cr);

    l_even= (l_row % 2);

    if (l_even)
    {
      cairo_set_source_rgba(io_cr, 0.90, 0.90, 1.00, 1.0);
      cairo_fill(io_cr);
    }

    cairo_stroke(io_cr);

    cairo_set_source_rgba(io_cr, 0.0, 0.0, 0.0, 1.0);
    cairo_move_to(io_cr, 100, l_pos_y);
    cairo_line_to(io_cr, 100, l_pos_y2);
    cairo_move_to(io_cr, 300, l_pos_y);
    cairo_line_to(io_cr, 300, l_pos_y2);
    cairo_move_to(io_cr, 375, l_pos_y);
    cairo_line_to(io_cr, 375, l_pos_y2);
    cairo_move_to(io_cr, 485, l_pos_y);
    cairo_line_to(io_cr, 485, l_pos_y2);
    cairo_stroke(io_cr);
    l_pos_y+= l_boxh;
    l_pos_y2+= l_boxh;
  }

  l_return_y= (l_pos_y + l_boxh);
  l_pos_y= (2 + l_push_y + l_boxh);

  for (l_row= 0; i_detail_lines > l_row; l_row++)
  {
    l_eof= sales_detail_fetch_next(l_detail, &(*io_data).m_cursor);

    if (TRUE == l_eof)
    {
      break;
    }

    if ((*l_detail).m_product_id[0])
    {
      pango_layout_set_text(io_layout, (*l_detail).m_product_id, -1);
      cairo_move_to(io_cr, 5, l_pos_y);
      pango_cairo_show_layout (io_cr, io_layout);  
    }

    if ((*l_detail).m_description[0])
    {
      pango_layout_set_text(io_layout, (*l_detail).m_description, -1);
      cairo_move_to(io_cr, 105, l_pos_y);
      pango_cairo_show_layout (io_cr, io_layout);  
    }

    if ((*l_detail).m_quantity[0])
    {
      pango_layout_set_text(io_layout, (*l_detail).m_quantity, -1);
      pango_layout_get_size(io_layout, &l_text_width, 0);
      l_text_width/= PANGO_SCALE;
      cairo_move_to(io_cr, (373 - l_text_width), l_pos_y);
      pango_cairo_show_layout (io_cr, io_layout);  
      decNumberFromString(&l_qty, (*l_detail).m_quantity, &(*io_data).m_set); 
    }
    else
    {
      decNumberFromString(&l_qty, "1", &(*io_data).m_set); 
    }

    if ((*l_detail).m_unit_price[0])
    {
      pango_layout_set_text(io_layout, (*l_detail).m_unit_price, -1);
      pango_layout_get_size(io_layout, &l_text_width, 0);
      l_text_width/= PANGO_SCALE;
      cairo_move_to(io_cr, (483 - l_text_width), l_pos_y);
      pango_cairo_show_layout (io_cr, io_layout);  

      decNumberFromString(&l_unit_price, (*l_detail).m_unit_price, &(*io_data).m_set); 
      decNumberMultiply(&l_line_total, &l_unit_price, &l_qty, &(*io_data).m_set); 
      decNumberAdd(&(*io_data).m_subtotal, &(*io_data).m_subtotal, &l_line_total, &(*io_data).m_set);
      decNumberToString(&l_line_total, l_buffer);

      pango_layout_set_text(io_layout, l_buffer, -1);
      pango_layout_get_size(io_layout, &l_text_width, 0);
      l_text_width/= PANGO_SCALE;
      cairo_move_to(io_cr, ((i_page_width - 2) - l_text_width), l_pos_y);
      pango_cairo_show_layout (io_cr, io_layout);  
    }

    l_pos_y+= l_boxh;
  }

  g_free(l_detail);

  return l_return_y;
}

static gint
print_totals(
  PangoLayout*const                     io_layout,
  cairo_t*const                         io_cr,
  struct print_data*const               io_data,
  gint const                            i_page_width,
  gint const                            i_pos_y,
  gint const                            i_advance)
{
  gint                                  l_boxh;
  gint                                  l_boxw;
  gchar                                 l_buffer[512];
  decNumber                             l_discount;
  decNumber                             l_due;
  gint                                  l_pos_y;
  gint                                  l_pos_y2;
  gint                                  l_text_width;
  decNumber                             l_total;

  l_pos_y= i_pos_y;
  l_boxh= (4 + i_advance);
  l_boxw= (i_page_width - 485);
  l_pos_y2= (2 + l_pos_y);

  cairo_set_source_rgba(io_cr, 0.0, 0.0, 0.0, 1.0);
  cairo_set_line_width(io_cr, 1);

  cairo_rectangle(io_cr, 485, l_pos_y, l_boxw, l_boxh);
  cairo_stroke(io_cr);
  pango_layout_set_text(io_layout, "Subtotal", -1);
  pango_layout_get_size(io_layout, &l_text_width, 0);
  l_text_width/= PANGO_SCALE;
  cairo_move_to(io_cr, (480 - l_text_width), l_pos_y2);
  pango_cairo_show_layout (io_cr, io_layout);  

  decNumberToString(&(*io_data).m_subtotal, l_buffer);
  pango_layout_set_text(io_layout, l_buffer, -1);
  pango_layout_get_size(io_layout, &l_text_width, 0);
  l_text_width/= PANGO_SCALE;
  cairo_move_to(io_cr, (i_page_width - l_text_width - 2), l_pos_y2);
  pango_cairo_show_layout (io_cr, io_layout);  

  l_pos_y+= l_boxh;
  l_pos_y2+= l_boxh;

  cairo_rectangle(io_cr, 485, l_pos_y, l_boxw, l_boxh);
  cairo_stroke(io_cr);
  pango_layout_set_text(io_layout, "Discount Amount", -1);
  pango_layout_get_size(io_layout, &l_text_width, 0);
  l_text_width/= PANGO_SCALE;
  cairo_move_to(io_cr, (480 - l_text_width), l_pos_y2);
  pango_cairo_show_layout (io_cr, io_layout);  

  decNumberFromString(&l_discount, (*io_data).m_sales.m_discount, &(*io_data).m_set);
  decNumberToString(&l_discount, l_buffer);
  pango_layout_set_text(io_layout, l_buffer, -1);
  pango_layout_get_size(io_layout, &l_text_width, 0);
  l_text_width/= PANGO_SCALE;
  cairo_move_to(io_cr, (i_page_width - l_text_width - 2), l_pos_y2);
  pango_cairo_show_layout (io_cr, io_layout);  

  l_pos_y+= l_boxh;
  l_pos_y2+= l_boxh;

  cairo_rectangle(io_cr, 485, l_pos_y, l_boxw, l_boxh);
  cairo_stroke(io_cr);
  pango_layout_set_text(
    io_layout,
    ((estimate == (*io_data).m_sales.m_sales_type) ? "Total Estimate" :  "Total"), 
    -1);
  pango_layout_get_size(io_layout, &l_text_width, 0);
  l_text_width/= PANGO_SCALE;
  cairo_move_to(io_cr, (480 - l_text_width), l_pos_y2);
  pango_cairo_show_layout (io_cr, io_layout);  

  decNumberAdd(&l_total, &(*io_data).m_subtotal, &l_discount, &(*io_data).m_set);
  decNumberToString(&l_total, l_buffer);
  pango_layout_set_text(io_layout, l_buffer, -1);
  pango_layout_get_size(io_layout, &l_text_width, 0);
  l_text_width/= PANGO_SCALE;
  cairo_move_to(io_cr, (i_page_width - l_text_width - 2), l_pos_y2);
  pango_cairo_show_layout (io_cr, io_layout);  

  l_pos_y+= l_boxh;
  l_pos_y2+= l_boxh;

  if (invoice == (*io_data).m_sales.m_sales_type)
  {
    cairo_rectangle(io_cr, 485, l_pos_y, l_boxw, l_boxh);
    cairo_stroke(io_cr);
    pango_layout_set_text(io_layout, "Paid", -1);
    pango_layout_get_size(io_layout, &l_text_width, 0);
    l_text_width/= PANGO_SCALE;
    cairo_move_to(io_cr, (480 - l_text_width), l_pos_y2);
    pango_cairo_show_layout (io_cr, io_layout);  

    decNumberToString(&(*io_data).m_paid, l_buffer);
    pango_layout_set_text(io_layout, l_buffer, -1);
    pango_layout_get_size(io_layout, &l_text_width, 0);
    l_text_width/= PANGO_SCALE;
    cairo_move_to(io_cr, (i_page_width - l_text_width - 2), l_pos_y2);
    pango_cairo_show_layout (io_cr, io_layout);  

    l_pos_y+= l_boxh;
    l_pos_y2+= l_boxh;

    cairo_rectangle(io_cr, 485, l_pos_y, l_boxw, l_boxh);
    cairo_stroke(io_cr);
    pango_layout_set_text(io_layout, "Total Due", -1);
    pango_layout_get_size(io_layout, &l_text_width, 0);
    l_text_width/= PANGO_SCALE;
    cairo_move_to(io_cr, (480 - l_text_width), l_pos_y2);
    pango_cairo_show_layout (io_cr, io_layout);  

    decNumberSubtract(&l_due, &l_total, &(*io_data).m_paid, &(*io_data).m_set);
    decNumberToString(&l_due, l_buffer);
    pango_layout_set_text(io_layout, l_buffer, -1);
    pango_layout_get_size(io_layout, &l_text_width, 0);
    l_text_width/= PANGO_SCALE;
    cairo_move_to(io_cr, (i_page_width - l_text_width - 2), l_pos_y2);
    pango_cairo_show_layout (io_cr, io_layout);  

    l_pos_y+= l_boxh;
    l_pos_y2+= l_boxh;
  }

  return l_pos_y;
}

static void
print_notes(
  PangoLayout*const                     io_layout,
  cairo_t*const                         io_cr,
  struct print_data*const               io_data,
  gint const                            i_page_width,
  gint const                            i_page_height,
  gint const                            i_pos_y)
{
  PangoFontDescription*                 l_font;
  gdouble                               l_width;

  if ((*io_data).m_sales.m_notes)
  {
    l_font= pango_font_description_from_string("Arial 10");
    pango_layout_set_font_description(io_layout, l_font);
    pango_font_description_free(l_font);
    l_width= (PANGO_SCALE * i_page_width);
    pango_layout_set_width(io_layout, l_width);
    pango_layout_set_wrap(io_layout, PANGO_WRAP_WORD);
    pango_layout_set_markup(io_layout, (*io_data).m_sales.m_notes, -1);
    cairo_move_to(io_cr, 10, i_pos_y+20);
    pango_cairo_update_layout(io_cr, io_layout);
    pango_cairo_show_layout (io_cr, io_layout);  
  }

  return;
}

static int
print_estimate_disclaimer(
  PangoLayout*const                     io_layout,
  cairo_t*const                         io_cr,
  gint const                            i_page_width,
  gint const                            i_pos_y,
  gint const                            i_advance)
{
  PangoFontDescription*                 l_font;
  gint                                  l_pos_y;
  gint                                  l_text_height;
  gdouble                               l_width;

  l_pos_y= i_pos_y;

  l_font= pango_font_description_from_string("Arial 10");
  pango_layout_set_font_description(io_layout, l_font);
  pango_font_description_free(l_font);
  l_width= (i_page_width * PANGO_SCALE);
  pango_layout_set_width(io_layout, l_width);
  pango_layout_set_wrap(io_layout, PANGO_WRAP_WORD);
  pango_layout_set_markup(io_layout, estimate_disclaimer, -1);
  pango_layout_get_size(io_layout, 0, &l_text_height);
  cairo_move_to(io_cr, 0, i_pos_y);
  pango_cairo_update_layout(io_cr, io_layout);
  pango_cairo_show_layout (io_cr, io_layout);  

  l_pos_y+= (l_text_height / PANGO_SCALE );

  return l_pos_y;
}

static void
print_page0(
  GtkPrintOperation*const               io_operation,
  GtkPrintContext*const                 io_context,
  struct print_data*const               io_data)
{
  gint                                  l_advance;
  gchar                                 l_buffer[512];
  cairo_t*                              l_cr;
  PangoFontDescription*                 l_font;
  gdouble                               l_height;
  PangoLayout*                          l_layout;
  gint                                  l_new_y1;
  gint                                  l_new_y2;
  gint                                  l_pages;
  gint                                  l_pos_x;
  gint                                  l_pos_y;
  gint                                  l_text_height;
  gint                                  l_text_width;
  gdouble                               l_width;

  l_width= gtk_print_context_get_width(io_context);
  l_height= gtk_print_context_get_height(io_context);

  l_cr= gtk_print_context_get_cairo_context(io_context);
  l_layout= gtk_print_context_create_pango_layout(io_context);

  l_font= pango_font_description_from_string("Arial 28");
  pango_layout_set_font_description(l_layout, l_font);
  pango_font_description_free(l_font);
  pango_layout_set_width(l_layout, -1);
  pango_layout_set_text(
    l_layout, 
    ((estimate == (*io_data).m_sales.m_sales_type) ? "ESTIMATE" :  "INVOICE"), 
    -1);
  pango_layout_get_size(l_layout, &l_text_width, &l_text_height);
  l_text_width/= PANGO_SCALE;
  cairo_move_to(l_cr, (l_width - l_text_width), 0);
  cairo_set_source_rgba(l_cr, 0.90, 0.90, 0.90, 1.0);
  pango_cairo_update_layout(l_cr, l_layout);
  pango_cairo_show_layout (l_cr, l_layout);  

  cairo_set_source_rgba(l_cr, 0.0, 0.0, 0.0, 1.0);
  cairo_move_to(l_cr, 0, 0);
  l_font= pango_font_description_from_string("Arial 18");
  pango_layout_set_font_description(l_layout, l_font);
  pango_font_description_free(l_font);
  pango_layout_set_text(l_layout, (*io_data).m_company.m_company_name, -1);
  pango_layout_get_size(l_layout, &l_text_width, &l_text_height);
  l_pos_y= (l_text_height / PANGO_SCALE);
  pango_cairo_update_layout(l_cr, l_layout);
  pango_cairo_show_layout (l_cr, l_layout);  

  l_font= pango_font_description_from_string("Arial 12");
  pango_layout_set_font_description(l_layout, l_font);
  pango_font_description_free(l_font);
  pango_cairo_update_layout(l_cr, l_layout);
  pango_layout_get_size(l_layout, &l_text_width, &l_text_height);
  l_advance= (l_text_height / PANGO_SCALE);

  if ((*io_data).m_company.m_street1[0])
  {
    pango_layout_set_text(l_layout, (*io_data).m_company.m_street1, -1);
    cairo_move_to(l_cr, 0, l_pos_y);
    pango_cairo_show_layout (l_cr, l_layout);  
    l_pos_y+= l_advance;
  }

  if ((*io_data).m_company.m_street2[0])
  {
    pango_layout_set_text(l_layout, (*io_data).m_company.m_street2, -1);
    cairo_move_to(l_cr, 0, l_pos_y);
    pango_cairo_show_layout (l_cr, l_layout);  
    l_pos_y+= l_advance;
  }

  if ((*io_data).m_company.m_street3[0])
  {
    pango_layout_set_text(l_layout, (*io_data).m_company.m_street3, -1);
    cairo_move_to(l_cr, 0, l_pos_y);
    pango_cairo_show_layout (l_cr, l_layout);  
    l_pos_y+= l_advance;
  }

  pango_layout_set_text(l_layout, "Date:", -1);
  l_pos_x= (l_width / 2) + 100;
  pango_layout_get_size(l_layout, &l_text_width, 0);
  l_text_width/= PANGO_SCALE;
  cairo_move_to(l_cr, (l_pos_x - l_text_width), l_pos_y);
  pango_cairo_show_layout (l_cr, l_layout);  

  pango_layout_set_text(l_layout, (*io_data).m_today, -1);
  cairo_move_to(l_cr, (l_pos_x+8), l_pos_y);
  pango_cairo_show_layout (l_cr, l_layout);  

  pango_layout_set_text(
    l_layout,
    ((estimate == (*io_data).m_sales.m_sales_type) ? "Estimate #:" :  "Invoice #:"),
    -1);
  pango_layout_get_size(l_layout, &l_text_width, 0);
  l_text_width/= PANGO_SCALE;
  cairo_move_to(l_cr, (l_pos_x - l_text_width), (l_pos_y+l_advance));
  pango_cairo_show_layout (l_cr, l_layout);  

  pango_layout_set_text(l_layout, (*io_data).m_sales.m_sales_id, -1);
  cairo_move_to(l_cr, (l_pos_x+8), (l_pos_y+l_advance));
  pango_cairo_show_layout (l_cr, l_layout);  

  g_snprintf(
    l_buffer,
    sizeof(l_buffer),
    "%s, %s %s",
    (*io_data).m_company.m_city,
    (*io_data).m_company.m_state,
    (*io_data).m_company.m_zipcode);

  pango_layout_set_text(l_layout, l_buffer, -1);
  cairo_move_to(l_cr, 0, l_pos_y);
  pango_cairo_show_layout (l_cr, l_layout);  
  l_pos_y+= l_advance;
  l_pos_x= 0;

  if ((*io_data).m_company.m_phone[0])
  {
    g_snprintf(l_buffer, sizeof(l_buffer), "Phone: %s", (*io_data).m_company.m_phone);
    pango_layout_set_text(l_layout, l_buffer, -1);
    pango_layout_get_size(l_layout, &l_text_width, 0);
    cairo_move_to(l_cr, 0, l_pos_y);
    pango_cairo_show_layout (l_cr, l_layout);  
    l_pos_x+= (l_text_width / PANGO_SCALE); 
    l_pos_x+= 25;
  }

  if ((*io_data).m_company.m_cellphone[0])
  {
    g_snprintf(l_buffer, sizeof(l_buffer), "Cell: %s", (*io_data).m_company.m_cellphone);
    pango_layout_set_text(l_layout, l_buffer, -1);
    cairo_move_to(l_cr, l_pos_x, l_pos_y);
    pango_cairo_show_layout (l_cr, l_layout);  
  }

  l_pos_y+= l_advance;

  if ((*io_data).m_company.m_email[0])
  {
    g_snprintf(l_buffer, sizeof(l_buffer), "E-mail: %s", (*io_data).m_company.m_email);
    pango_layout_set_text(l_layout, l_buffer, -1);
    cairo_move_to(l_cr, 0, l_pos_y);
    pango_cairo_show_layout (l_cr, l_layout);  
    l_pos_y+= l_advance;
  }

  if ((*io_data).m_company.m_website[0])
  {
    pango_layout_set_text(l_layout, (*io_data).m_company.m_website, -1);
    cairo_move_to(l_cr, 0, l_pos_y);
    pango_cairo_show_layout (l_cr, l_layout);  
    l_pos_y+= l_advance;
  }

  l_pos_y+= l_advance;
  l_pos_y+= l_advance;

  l_new_y1= print_contact(
    l_layout,
    l_cr,
    &(*io_data).m_contact,
    l_pos_y,
    l_advance);

  l_new_y2= print_service_contact(
    l_layout,
    l_cr,
    (*io_data).m_sales.m_service_contact,
    (l_width / 2),
    l_pos_y,
    l_advance);

  if (l_new_y1 > l_new_y2)
  {
    l_pos_y= l_new_y1;
  }
  else
  {
    l_pos_y= l_new_y2;
  }

  l_pos_y+= l_advance;
  l_pos_y+= l_advance;

  if (invoice == (*io_data).m_sales.m_sales_type)
  {
    l_pos_y= print_page0_header2(
      l_layout,
      l_cr,
      &(*io_data).m_sales,
      l_width,
      l_pos_y,
      l_advance);
  }
  else
  {
    l_pos_y= print_estimate_disclaimer(l_layout, l_cr, l_width, l_pos_y, l_advance);
  }

  l_pos_y+= l_advance;
  l_pos_y+= l_advance;

  l_pos_y= print_detail(
    l_layout,
    l_cr,
    io_data,
    l_width,
    l_height,
    l_pos_y,
    l_advance,
    detail_lines_page_first);

  l_pages= gtk_print_operation_get_n_pages_to_print(io_operation);

  if (1 == l_pages)
  {
    l_pos_y= print_totals(l_layout, l_cr, io_data, l_width, l_pos_y, l_advance);
  }
  else
  {
    l_font= pango_font_description_from_string("Arial 8");
    pango_layout_set_font_description(l_layout, l_font);
    pango_font_description_free(l_font);
    pango_layout_set_text(l_layout, "(continued on next page)", -1);
    pango_layout_get_size(l_layout, &l_text_width, 0);
    l_pos_x= ((l_width / 2) - ((l_text_width / 2) / PANGO_SCALE));
    cairo_move_to(l_cr, l_pos_x, l_pos_y);
    pango_cairo_update_layout(l_cr, l_layout);
    pango_cairo_show_layout (l_cr, l_layout);  
    l_pos_y+= l_advance;

    g_snprintf(l_buffer, sizeof(l_buffer), "Page 1 of %u", l_pages);
    pango_layout_set_text(l_layout, l_buffer, -1);
    pango_layout_get_size(l_layout, &l_text_width, &l_text_height);
    l_pos_x= l_width - (l_text_width / PANGO_SCALE);
    l_new_y2= l_height - (l_text_height / PANGO_SCALE);
    cairo_move_to(l_cr, l_pos_x, l_new_y2);
    pango_cairo_update_layout(l_cr, l_layout);
    pango_cairo_show_layout (l_cr, l_layout);  
  }

  print_notes(l_layout, l_cr, io_data, l_width, l_height, l_pos_y);

  g_object_unref(l_layout);

  return;
}

static void
print_pageN(
  GtkPrintOperation*const               io_operation,
  GtkPrintContext*const                 io_context,
  struct print_data*const               io_data,
  gint const                            i_page_number)
{
  gint                                  l_advance;
  gchar                                 l_buffer[512];
  cairo_t*                              l_cr;
  PangoFontDescription*                 l_font;
  gdouble                               l_height;
  PangoLayout*                          l_layout;
  gint                                  l_new_y1;
  gint                                  l_new_y2;
  gint                                  l_pages;
  gint                                  l_pos_x;
  gint                                  l_pos_y;
  gint                                  l_text_height;
  gint                                  l_text_width;
  gdouble                               l_width;

  l_width= gtk_print_context_get_width(io_context);
  l_height= gtk_print_context_get_height(io_context);

  l_cr= gtk_print_context_get_cairo_context(io_context);
  l_layout= gtk_print_context_create_pango_layout(io_context);

  l_font= pango_font_description_from_string("Arial 28");
  pango_layout_set_font_description(l_layout, l_font);
  pango_font_description_free(l_font);
  pango_layout_set_width(l_layout, -1);
  pango_layout_set_text(
    l_layout, 
    ((estimate == (*io_data).m_sales.m_sales_type) ? "ESTIMATE" :  "INVOICE"), 
    -1);
  pango_layout_get_size(l_layout, &l_text_width, &l_text_height);
  l_text_width/= PANGO_SCALE;
  cairo_move_to(l_cr, (l_width - l_text_width), 0);
  cairo_set_source_rgba(l_cr, 0.90, 0.90, 0.90, 1.0);
  pango_cairo_update_layout(l_cr, l_layout);
  pango_cairo_show_layout (l_cr, l_layout);  

  cairo_set_source_rgba(l_cr, 0.0, 0.0, 0.0, 1.0);
  cairo_move_to(l_cr, 0, 0);
  l_font= pango_font_description_from_string("Arial 18");
  pango_layout_set_font_description(l_layout, l_font);
  pango_font_description_free(l_font);
  pango_layout_set_text(l_layout, (*io_data).m_company.m_company_name, -1);
  pango_layout_get_size(l_layout, &l_text_width, &l_text_height);
  l_pos_y= (l_text_height / PANGO_SCALE);
  pango_cairo_update_layout(l_cr, l_layout);
  pango_cairo_show_layout (l_cr, l_layout);  

  l_font= pango_font_description_from_string("Arial 12");
  pango_layout_set_font_description(l_layout, l_font);
  pango_font_description_free(l_font);
  pango_cairo_update_layout(l_cr, l_layout);
  pango_layout_get_size(l_layout, &l_text_width, &l_text_height);
  l_advance= (l_text_height / PANGO_SCALE);

  l_pos_y+= l_advance;
  l_pos_y+= l_advance;

  l_pos_y= print_detail(
    l_layout,
    l_cr,
    io_data,
    l_width,
    l_height,
    l_pos_y,
    l_advance,
    detail_lines_page_nth);

  l_pages= gtk_print_operation_get_n_pages_to_print(io_operation);

  if (i_page_number == l_pages)
  {
    print_totals(l_layout, l_cr, io_data, l_width, l_pos_y, l_advance);
  }
  else
  {
    l_font= pango_font_description_from_string("Arial 8");
    pango_layout_set_font_description(l_layout, l_font);
    pango_font_description_free(l_font);
    pango_layout_set_text(l_layout, "(continued on next page)", -1);
    pango_layout_get_size(l_layout, &l_text_width, 0);
    l_pos_x= ((l_width / 2) - ((l_text_width / 2) / PANGO_SCALE));
    cairo_move_to(l_cr, l_pos_x, l_pos_y);
    pango_cairo_update_layout(l_cr, l_layout);
    pango_cairo_show_layout (l_cr, l_layout);  
  }

  g_snprintf(l_buffer, sizeof(l_buffer), "Page %u of %u", i_page_number, l_pages);
  l_font= pango_font_description_from_string("Arial 8");
  pango_layout_set_font_description(l_layout, l_font);
  pango_font_description_free(l_font);
  pango_layout_set_text(l_layout, l_buffer, -1);
  pango_layout_get_size(l_layout, &l_text_width, &l_text_height);
  l_pos_x= l_width - (l_text_width / PANGO_SCALE);
  l_pos_y= l_height - (l_text_height / PANGO_SCALE);
  cairo_move_to(l_cr, l_pos_x, l_pos_y);
  pango_cairo_update_layout(l_cr, l_layout);
  pango_cairo_show_layout (l_cr, l_layout);  

  g_object_unref(l_layout);

  return;
}

static void
print_page (
  GtkPrintOperation*const               io_operation,
  GtkPrintContext*const                 io_context,
  gint                                  i_page_number,
  gpointer                              io_user_data)
{
  struct print_data*                    l_data;

  l_data= (struct print_data*)io_user_data;

  if (0 == i_page_number)
  {
    print_page0(io_operation, io_context, l_data);
  }
  else
  {
    print_pageN(io_operation, io_context, l_data, (1+i_page_number));
  }

  return;
}

static void
print_begin (
  GtkPrintOperation*const               io_operation,
  GtkPrintContext*const                 io_context,
  gpointer                              io_user_data)
{
  struct print_data*                    l_data;
  GError*                               l_error;
  int                                   l_exit;
  gsize                                 l_pages;

  l_data= (struct print_data*)io_user_data;
  l_error= 0;
  l_exit= 0;

  do
  {

    l_exit= company_fetch(&l_error, &(*l_data).m_company, (*l_data).m_session);

    if (l_exit)
    {
      break;
    }

    l_exit= sales_fetch(
      &l_error,
      &(*l_data).m_sales,
      (*l_data).m_session,
      (*l_data).m_sales_id);

    if (l_exit)
    {
      break;
    }

    l_exit= sales_detail_fetch_query(
      &l_error,
      &(*l_data).m_cursor,
      (*l_data).m_session,
      (*l_data).m_sales_id);

    if (l_exit)
    {
      break;
    }

    if ((*l_data).m_sales.m_billing_id[0])
    {
      contact_fetch(
        &l_error,
        &(*l_data).m_contact,
        (*l_data).m_session, 
        (*l_data).m_sales.m_billing_id);
      if (l_error)
      {
        g_clear_error(&l_error);
      }
    }

    if (detail_lines_page_first >= (*l_data).m_cursor.m_tuples)
    {
      l_pages= 1;
    }
    else
    {
      l_pages= (detail_lines_page_nth - 1);
      l_pages+= ((*l_data).m_cursor.m_tuples - detail_lines_page_first);
      l_pages= ceil(l_pages / detail_lines_page_nth);
      l_pages++;
    }

    gtk_print_operation_set_n_pages(io_operation, l_pages);

  }while(0);

  if (l_error)
  {
    _error_display((*l_data).m_parent, l_error);
    g_clear_error(&l_error);
  }

  if (l_exit)
  {
    gtk_print_operation_cancel(io_operation);
  }

  return;
}

static void
print_end (
  GtkPrintOperation*const               io_operation,
  GtkPrintContext*const                 io_context,
  gpointer                              io_user_data)
{
  struct print_data*                    l_data;

  l_data= (struct print_data*)io_user_data;

  cursor_discharge(&(*l_data).m_cursor);
  g_free((*l_data).m_today);
  g_free(l_data);

  return;
}

extern int
sales_print(
  struct session*const                  io_session,
  GtkWindow*const                       io_parent,
  gchar const                           i_sales_id[size_pg_big_int])
{
  struct print_data*                    l_data;
  GError*                               l_error;
  int                                   l_exit;
  GtkPrintOperation*                    l_operation;
  GtkPrintSettings*                     l_settings;
  decNumber                             l_two;

  l_data= (struct print_data*)g_malloc0(sizeof(*l_data));

  l_error= 0;
  l_exit= 0;
  l_operation= 0;
  l_settings= 0;

  g_strlcpy((*l_data).m_sales_id, i_sales_id, sizeof((*l_data).m_sales_id));
  cursor_assign(&(*l_data).m_cursor);
  (*l_data).m_session= io_session;
  (*l_data).m_parent= io_parent;
  (*l_data).m_today= calendar_format_now();
  decContextDefault(&(*l_data).m_set, DEC_INIT_BASE);
  (*l_data).m_set.traps= 0;
  (*l_data).m_set.digits= DECNUMDIGITS;
  decNumberFromInt32(&(*l_data).m_subtotal, 0);
  decNumberFromInt32(&(*l_data).m_paid, 0);
  decNumberFromString(&l_two, "-2", &(*l_data).m_set);
  decNumberRescale(&(*l_data).m_subtotal, &(*l_data).m_subtotal, &l_two, &(*l_data).m_set);
  decNumberRescale(&(*l_data).m_paid, &(*l_data).m_paid, &l_two, &(*l_data).m_set);

  l_operation= gtk_print_operation_new();

  g_signal_connect(G_OBJECT(l_operation), "begin-print", G_CALLBACK(print_begin), l_data);
  g_signal_connect(G_OBJECT(l_operation), "draw-page", G_CALLBACK(print_page), l_data);
  g_signal_connect(G_OBJECT(l_operation), "end-print", G_CALLBACK(print_end), l_data);

  gtk_print_operation_set_use_full_page(l_operation, FALSE);
  gtk_print_operation_set_unit(l_operation, GTK_UNIT_POINTS);
  gtk_print_operation_set_embed_page_setup(l_operation, TRUE);

  l_settings= gtk_print_settings_new();

  gtk_print_operation_set_print_settings(l_operation, l_settings);

  gtk_print_operation_run(
    l_operation,
    GTK_PRINT_OPERATION_ACTION_PRINT_DIALOG,
    io_parent,
    &l_error);

  if (l_error)
  {
    _error_display(io_parent, l_error);
    g_clear_error(&l_error);
  }

  g_object_unref(l_operation);
  g_object_unref(l_settings);


  return l_exit;
}
/* 
vim:expandtab:softtabstop=2:tabstop=2:shiftwidth=2:nowrap:ruler 
*/
