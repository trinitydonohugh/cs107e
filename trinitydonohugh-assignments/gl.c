#include "gl.h"
#include "string.h"

void gl_init(unsigned int width, unsigned int height, unsigned int mode)
{
    fb_init(width, height, 4, mode);
}

void gl_swap_buffer(void)
{
    fb_swap_buffer();
}

unsigned int gl_get_width(void)
{
    unsigned int glwidth = fb_get_width();
    return glwidth;
}

unsigned int gl_get_height(void)
{
    unsigned int glheight = fb_get_height();
    return glheight;
}

unsigned int gl_get_pitch(void)
{
    unsigned int glpitch = fb_get_pitch();
    return glpitch;
}

color_t gl_color(unsigned char r, unsigned char g, unsigned char b)
{
    color_t returnColor = 0xFF000000 | r << 16 | g << 8 | b;
    return returnColor;
}

void gl_draw_pixel(int x, int y, color_t c)
{
    unsigned (*im)[gl_get_pitch()/4] = (unsigned (*)[gl_get_pitch()/4])fb_get_draw_buffer();   
    //check bounds in pixel 
    if (y < gl_get_height() && y > 0) {
        if (x < gl_get_width() && x > 0) {
            im[y][x] = c;
        }
    } 
}

void gl_clear(color_t c)
{
    for (int y = 0; y < gl_get_height(); y++) {
      for (int x = 0; x < gl_get_width(); x++) {
        gl_draw_pixel(x, y, c);
      }
    }
}

color_t gl_read_pixel(int x, int y)
{
    unsigned (*im)[gl_get_pitch()/4] = (unsigned (*)[gl_get_pitch()/4])fb_get_draw_buffer();
    color_t color = im[y][x];
    return color;
}

void gl_draw_rect(int x, int y, int w, int h, color_t c)
{   
    for (int u = y; u < y+h; u++) {
        for (int v = x; v < x+w; v++) {
            gl_draw_pixel(v, u, c);
        }
    }

}

void gl_draw_char(int x, int y, int ch, color_t c)
{
    unsigned char buf[font_get_size()];
    unsigned int bufIndex = 0;
    if (font_get_char(ch, buf, font_get_size())){
        for (int u = y; u < y + font_get_height(); u++) {
            for (int v = x; v < x + font_get_width(); v++) {
                if (buf[bufIndex] == 0xFF) {
                    gl_draw_pixel(v, u, c);
                }
                bufIndex++;
            }
        }
    }
}

void gl_draw_string(int x, int y, const char* str, color_t c)
{   
    for (int i = 0; i < strlen(str); i++) {
        gl_draw_char(x, y, str[i], c);
        x += font_get_width();
    }
}

unsigned int gl_get_char_height(void)
{
    return font_get_height();
}

unsigned int gl_get_char_width(void)
{
    unsigned int fontWidth = font_get_width();
    return fontWidth;
}
