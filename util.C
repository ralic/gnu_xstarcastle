//      xstarcastle
//      Copyright (c) 1993-2005 by Mark B. Hanson (mbh@panix.com).
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  
//  02110-1301, USA.
//

#include "global.h"

#include "util.h"
#include "xsc.h"

GC**                    gcs;
GC**                    bright_gcs;
namespace {

GC gc_list[GC_TOKEN_COUNT];

void
alloc_color(const char *const name, XColor *color)
{
    const int screen_number = DefaultScreen(display);
    XColor dummy;
    Colormap colormap = DefaultColormap(display, screen_number);

    if (XAllocNamedColor(display, colormap, name, color, &dummy) == 0) {
	fprintf(stderr, "%s: failed to allocate color `%s'.  ", program, name);
	fprintf(stderr, "substituting `white'.\n");
	if (XAllocNamedColor(display, colormap, "white", color,
		    &dummy) == 0) {
	    fprintf(stderr, "%s: failed to allocate color `white'.\n", program);
	}
    }
} // ::alloc_color


GC
alloc_gc(const char *const colorname, const int width)
{
    const int screen_number = DefaultScreen(display);
    XColor color;
    XGCValues gc_values;

    gc_values.line_width = width;
    gc_values.background = BlackPixel(display, screen_number);

    alloc_color(colorname, &color);
    gc_values.foreground = color.pixel;

    return XCreateGC(display, RootWindow(display, screen_number),
	(GCForeground | GCBackground | GCLineWidth), &gc_values);
} // ::alloc_gc

} // namespace


void
init_gc(bool monochrome)
{
  if (monochrome) 
    {
      gc_list[GC_BLACK] = alloc_gc("black", 2);

      gc_list[GC_BRIGHT_RED] = alloc_gc("grey40", 2);
      gc_list[GC_DULL_RED] = alloc_gc("grey30", 2);

      gc_list[GC_BRIGHT_ORANGE] = alloc_gc("grey60", 2);
      gc_list[GC_DULL_ORANGE] = alloc_gc("grey50", 2);

      gc_list[GC_BRIGHT_YELLOW] = alloc_gc("grey80", 2);
      gc_list[GC_DULL_YELLOW] = alloc_gc("grey70", 2);

      gc_list[GC_BRIGHT_GREY] = alloc_gc("grey80", 2);
      gc_list[GC_DULL_GREY] = alloc_gc("grey40", 2);

      gc_list[GC_GREEN] = alloc_gc("grey50", 2);

      gc_list[GC_BRIGHT_BLUE] = alloc_gc("grey95", 2);
      gc_list[GC_DULL_BLUE] = alloc_gc("grey90", 2);
    }
  else
    {
      gc_list[GC_BLACK] = alloc_gc("black", 2);

      gc_list[GC_BRIGHT_RED] = alloc_gc("red", 2);
      gc_list[GC_DULL_RED] = alloc_gc("red3", 2);

      gc_list[GC_BRIGHT_ORANGE] = alloc_gc("orange", 2);
      gc_list[GC_DULL_ORANGE] = alloc_gc("orange3", 2);

      gc_list[GC_BRIGHT_YELLOW] = alloc_gc("yellow", 2);
      gc_list[GC_DULL_YELLOW] = alloc_gc("yellow3", 2);

      gc_list[GC_BRIGHT_GREY] = alloc_gc("grey80", 2);
      gc_list[GC_DULL_GREY] = alloc_gc("grey40", 2);

      gc_list[GC_GREEN] = alloc_gc("green", 2);

      gc_list[GC_BRIGHT_BLUE] = alloc_gc("lightskyblue", 2);
      gc_list[GC_DULL_BLUE] = alloc_gc("dodgerblue", 2);
    }
    gcs = new GC*[display_height];
  for(int i = 0; i < display_height; ++i)
        gcs[i] = new GC[display_width];
  bright_gcs = new GC*[display_height];
  for(int i = 0; i < display_height; ++i)
        bright_gcs[i] = new GC[display_width];


} // ::init_gc

void
free_all_gcs(void)
{
    for(int i = 0; i < display_height; ++i)
      delete gcs[i];
    delete gcs;
    for(int i = 0; i < display_height; ++i)
      delete bright_gcs[i];
    delete bright_gcs;

    XFreeGC(display, gc_list[GC_BLACK]);

    XFreeGC(display, gc_list[GC_BRIGHT_RED]);
    XFreeGC(display, gc_list[GC_DULL_RED]);

    XFreeGC(display, gc_list[GC_BRIGHT_ORANGE]);
    XFreeGC(display, gc_list[GC_DULL_ORANGE]);

    XFreeGC(display, gc_list[GC_BRIGHT_YELLOW]);
    XFreeGC(display, gc_list[GC_DULL_YELLOW]);

    XFreeGC(display, gc_list[GC_BRIGHT_GREY]);
    XFreeGC(display, gc_list[GC_DULL_GREY]);

    XFreeGC(display, gc_list[GC_BRIGHT_BLUE]);
    XFreeGC(display, gc_list[GC_DULL_BLUE]);

    XFreeGC(display, gc_list[GC_GREEN]);
} // ::free_all_gcs


GC
fetch_gc(const gc_token t)
{
    return gc_list[t];
} // ::fetch_gc


int wrap(int pos, int max)
{
  while (pos < 0)
    pos+=max;
  while (pos >= max)
    pos-=max;
  return pos;
}

void drawvertline(Window w, int x, int y1, int y2, GC c, bool bright)
{
  int tmp, dy;

  if (y1 > y2)
  {
    tmp = y1;
    y1 = y2;
    y2 = tmp;
  }

  for (dy = y1; dy <= y2; dy++)
  {
    int ndy = wrap(dy, int(gwheight));
    int nx = wrap(x, int(wwidth));
    if (c == gc_list[GC_BLACK])
      XDrawPoint (display, w, c, nx, ndy);
    else
      {
        if (bright)
          XDrawPoint (display, w, bright_gcs[ndy][nx], nx, ndy);
        else
          XDrawPoint (display, w, gcs[ndy][nx], nx, ndy);
      }
  }
}

void __XDrawLine(Display * display, Window window, GC thisgc,
                int nx0, int ny0, int nx1, int ny1)
{
  XDrawLine(display, window, thisgc, nx0, ny0, nx1, ny1);
}

void _XFillArc(Display *display, Window window, GC thisgc, int x, int y,
               int xsize, int ysize, int b, int d, bool bright)
{
  if (y < 0)
    y+=gwheight;
  if (x < 0)
    x+=wwidth;
  if (y > gwheight)
    y-=gwheight;
  if (x > wwidth)
    x-=wwidth;
  if (x + xsize > wwidth)
    return;
  if (y + ysize > gwheight)
    return;

    if (thisgc == gc_list[GC_BLACK])
      XFillArc(display, window, thisgc, x, y, xsize, ysize, b, d);
    else
      {
      XFillArc(display, window, gcs[y][x], x, y, xsize, ysize, b, d);
      }
}

void _XDrawLine(Display * display, Window window, GC thisgc,
                int nx0, int ny0, int nx1, int ny1, bool bright)
{
  int dx, dy;
  float m, b;

  dx = nx1 - nx0;
  dy = ny1 - ny0;

  if (dx != 0)
    {
      m = ((float) dy) / ((float) dx);
      b = ny0 - m * nx0;

      if (nx1 >= nx0)
        dx = 1;
      else
        dx = -1;

      while (nx0 != nx1)
        {
          ny0 = (int) (m * nx0 + b);
          ny1 = (int) (m * (nx0 + dx) + b);

          drawvertline(window, nx0, ny0, ny1, thisgc, bright);

          nx0 = nx0 + dx;
        }
    }
  else
    {
      drawvertline(window, nx0, ny0, ny1, thisgc, bright);
    }
}

void
fill_circle(GC **g, int r, GC c)
{
  int ox = wwidth2, oy = gwheight2; // origin

  for (int x = -r; x < r ; x++)
    {
          int h= (int)sqrt(r * r - x * x);
              for (int y = -h; y < h; y++)
                g[y+oy][x+ox] = c;
    }
}

void
fill_gcs()
{
  for (int i = 0; i < gwheight; i++)
    for (int j = 0; j < wwidth; j++)
      gcs[i][j] = fetch_gc(GC_DULL_BLUE);

  fill_circle (gcs, int(max(gwheight, wwidth) * 0.1493589),
               fetch_gc(GC_DULL_RED));

  fill_circle (gcs, int(max(gwheight, wwidth) * 0.1223589),
               fetch_gc(GC_DULL_YELLOW));

  for (int i = 0; i < gwheight; i++)
    for (int j = 0; j < wwidth; j++)
      bright_gcs[i][j] = fetch_gc(GC_BRIGHT_BLUE);

  fill_circle (bright_gcs, int(max(gwheight, wwidth) * 0.1493589),
               fetch_gc(GC_BRIGHT_RED));

  fill_circle (bright_gcs, int(max(gwheight, wwidth) * 0.1223589),
               fetch_gc(GC_BRIGHT_YELLOW));
}
