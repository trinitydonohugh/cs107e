#include "fb.h"

typedef unsigned int color;

const color WHITE = 0xffffffff;
const color RED = 0xffff0000;
const color GREEN = 0xff00ff00;
const color BLUE = 0xff0000ff;
const color YELLOW = 0xffffff00;
const unsigned WIDTH = 1024;
const unsigned HEIGHT = 768;
const unsigned DEPTH = 4;

void draw_pixel(int x, int y, color c)
{
    unsigned (*im)[WIDTH] = (unsigned (*)[WIDTH])fb_get_draw_buffer();
    im[y][x] = c;
}

void draw_hline(int y, color c)
{
    for( int x = 0; x < fb_get_width(); x++ )
        draw_pixel(x, y, c);
}

void draw_vline(int x, color c)
{
    for( int y = 0; y < fb_get_height(); y++ )
        draw_pixel(x, y, c);
}

void fill_pixels(int x, int y, color c) 
{
  //if (y%32 && x%32) {
    for (int u = y; u < y+16; u++) {
      for (int v = x; v < x+16; v++) {
        draw_pixel(v, u, c);
      }
    }
  //}
  // } else if (){
  //   for (int u = y; u < y+16; u++) {
  //     for (int v = x; v < x+16; v++) {
  //       draw_pixel(v, u, c);
  //     }
  //   }
  // }
}

void main(void) 
{
  fb_init(WIDTH, HEIGHT, DEPTH, FB_SINGLEBUFFER);

  for( int y = 0; y < HEIGHT; y += 16 )
      draw_hline(y, RED);

  for( int x = 0; x < WIDTH; x += 16 )
      draw_vline(x, YELLOW);

  for (int y = 0; y < fb_get_height(); y += 16) {
    for (int x = 0; x < fb_get_width(); x += 16) {
      if ((y%32 == 0 && x%32 == 0) || (y%32 && x%32)) {
        fill_pixels(x, y, WHITE);
      } 
    }
  }
}

