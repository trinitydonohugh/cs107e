#include "assert.h"
#include "timer.h"
#include "fb.h"
#include "gl.h"
#include "console.h"
#include "printf.h"
#include "rand.h"

/* Note that to use the console, one should only have to
 * call console_init. To use the graphics library, one
 * should only have to call gl_init. If your main() requires
 * more than this, then your code will not pass tests and
 * will likely have many points deducted. Our GL tests
 * will call gl_init then invoke operations; our console
 * tests will call console_init then invoke operations.
 * To guarantee that tests will pass, make sure to also
 * run tests for each component separately.
 */

#define _WIDTH 600
#define _HEIGHT 400

#define _NROWS 20
#define _NCOLS 40

static void test_gl(void)
{
    // Double buffer mode, make sure you test single buffer too!
    gl_init(_WIDTH, _HEIGHT, GL_DOUBLEBUFFER);

    // Single buffer mode
    //gl_init(_WIDTH, _HEIGHT, GL_SINGLEBUFFER);

    // see what is being printed out for debugging
    printf("pitch: %d\n", fb_get_pitch());
    printf("width: %d\n", fb_get_width());
    printf("height: %d\n", fb_get_height());
    printf("depth: %d\n", fb_get_depth());

    // Background should be purple.
    gl_clear(gl_color(0xFF, 0, 0xFF));

    // Draw an amber pixel at an arbitrary spot.
    gl_draw_pixel(_WIDTH/3, _HEIGHT/3, GL_AMBER);
    assert(gl_read_pixel(_WIDTH/3, _HEIGHT/3) == GL_AMBER);
    //timer_delay(3);

    // Basic rectangle should be blue in center of screen
    gl_draw_rect(_WIDTH/2 - 20, _HEIGHT/2 - 20, 40, 40, GL_BLUE);
    //timer_delay(3);

    gl_draw_rect(570, 370, 40, 40, GL_BLUE);

    gl_draw_rect(_WIDTH/2 - 40, _HEIGHT/2 - 40, 40, 40, GL_BLUE);

    // Should write a single character
    gl_draw_char(60, 10, 'A', GL_BLUE);
    //timer_delay(3);

    // Should write a string of characters 
    gl_draw_string(60, 30, "ABCDE", GL_BLUE);
    //timer_delay(3);

    // Show buffer with drawn contents
    gl_swap_buffer();
    timer_delay(3);
}

static void test_double_buf_simple(void) {
    gl_init(_WIDTH, _HEIGHT, GL_DOUBLEBUFFER);

    gl_clear(GL_CYAN);
    gl_draw_string(60, 30, "Cyan", GL_BLUE);
    timer_delay(1);
    gl_swap_buffer(); //swap
    timer_delay(1);
    gl_clear(GL_MAGENTA);
    gl_draw_string(60, 30, "Magenta", GL_BLUE);
    timer_delay(1);
    gl_swap_buffer(); //swap
    gl_draw_string(60, 30, "Cyan again", GL_BLUE);
    timer_delay(1);
    gl_swap_buffer(); //swap
    timer_delay(1);
}

static void test_console(void)
{
    console_init(_NROWS, _NCOLS);

    //1: "HELLO WORLD"
    console_printf("HELLO WORLD\r");
    timer_delay(3);

    // 1: "HAPPY WORLD"
    // 2: "CODING"
    console_printf("HAPPY\nCODING\n");
    timer_delay(3);

    // Clear
    console_printf("\f");

    // 1: "GOODBYE"
    console_printf("GOODBYE\n");
}

static void draw_random_pixels(int W, int H, void(*draw_pixel)(int, int, color_t))
{
    for (int i = 0; i < 500000; i++) {
        int x = rand() % W;
        int y = rand() % H;
        int color = rand();
        gl_draw_pixel(x, y, color);
    }
}

static void draw_random_rects(int W, int H, void(*draw_rect)(int, int, int, int, color_t))
{
    for (int i = 0; i < 500; i++) {
        int x = rand() % W;
        int y = rand() % H;
        int w = rand() % 200;
        int h = rand() % 200;
        gl_draw_rect(x-w/2, y-h/2, w, h, rand());
    }
}

static void draw_chars(int W, int H, void(*draw_char)(int, int, char, color_t))
{
    int nrows = 16, ncols = 32;
    int char_w = gl_get_char_width();
    int char_h = gl_get_char_height();
    int w = ncols * char_w;
    int h = nrows * char_h;
    char ch = ' ';

    for (int row = 0; row < nrows; row++) {
        for (int col = 0; col < ncols; col++) {
            gl_draw_char(col * char_w, row * char_h, ch++, rand());
        }
    }
}

static void draw_strings(int W, int H, void(*draw_string)(int, int, char*, color_t))
{
    for (int i = 0; i < 20; i++) {
        char buf[20];
        snprintf(buf, sizeof(buf), "Number #%d", i+1);
        int x = rand() % W;
        int y = rand() % H;
        int w = strlen(buf)*gl_get_char_width();
        int h = gl_get_char_height();
        gl_draw_string(x-w/2, y-h/2, buf, rand());
    }
}

static void draw_long_string(void(*draw_string)(int, int, char*, color_t))
{
    char * long_string = "Here's some lorem ipsum, to test long lines: Lorem"
        "ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod"
        "tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim"
        "veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea"
        "commodo consequat. Duis aute irure dolor in reprehenderit in voluptate"
        "velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint"
        "occaecat cupidatat non proident, sunt in culpa qui officia deserunt"
        "mollit anim id est laborum. END.";
    gl_draw_string(0, 0, long_string, GL_BLACK);
}

void draw_double_buffer_composite(void)
{
    gl_init(_WIDTH, _HEIGHT, GL_DOUBLEBUFFER);

    // draw B
    gl_clear(GL_RED);
    gl_draw_char(0, 0, 'B', GL_BLACK);

    // display B, draw A
    gl_swap_buffer();
    timer_delay(1);
    gl_clear(GL_BLUE);
    gl_draw_char(0, 0, 'A', GL_BLACK);
   
    // display A, draw B
    gl_swap_buffer();
    timer_delay(1);
    gl_draw_string(50, 50, "ABCDE", GL_BLACK);

    // display B, draw A
    gl_swap_buffer();
    timer_delay(1);
    gl_draw_string(50, 50, "12345", GL_BLACK);

    // display A, draw B
    gl_swap_buffer();
    timer_delay(1);
    gl_draw_rect(100, 100, 20, 20, GL_WHITE);

    // display B, draw A
    gl_swap_buffer();
    timer_delay(1);
    gl_draw_rect(100, 100, 20, 20, GL_BLACK);

    // display A, blue background, 12345, black rectangle
    // draw    B, red  background, ABCDE, white rectangle
    gl_swap_buffer();
    timer_delay(1);
}


void main(void)
{
    //test draw random pixels
    // gl_init(_WIDTH, _HEIGHT, GL_SINGLEBUFFER); 
    // gl_clear(GL_WHITE);
    // draw_random_pixels(_WIDTH, _HEIGHT, &gl_draw_pixel);
    // timer_delay(1);

    //test draw random recs 
    // gl_init(_WIDTH, _HEIGHT,  GL_SINGLEBUFFER); 
    // gl_clear(GL_WHITE);
    // draw_random_rects(_WIDTH, _HEIGHT, &gl_draw_rect);
    // timer_delay(1);

    //test draw char
    // gl_init(_WIDTH, _HEIGHT, GL_SINGLEBUFFER); 
    // gl_clear(GL_WHITE);
    // draw_chars(_WIDTH, _HEIGHT, &gl_draw_char);
    // timer_delay(1);

    //test draw string
    // gl_init(_WIDTH, _HEIGHT,  GL_SINGLEBUFFER); 
    // gl_clear(GL_WHITE);
    // draw_strings(_WIDTH, _HEIGHT, &gl_draw_string);
    // timer_delay(1);

    //test draw long string
    gl_init(_WIDTH, _HEIGHT, GL_SINGLEBUFFER); 
    gl_clear(GL_YELLOW);
    gl_swap_buffer();
    gl_clear(GL_GREEN);
    draw_long_string(&gl_draw_string);
    timer_delay(1);

    //draw_double_buffer_composite();

    //test_double_buf_simple();
    //test_gl();
    //test_console();

    // console_printf("Hi, welcome to 107e. This is going to be a really long line to test whether the console will force the line to display on the next line. Let's see if this works.");

    // console_printf("hi\r");

    // console_printf("backspace\b");

    /* TODO: Add tests here to test your graphics library and console.
       For the framebuffer and graphics libraries, make sure to test
       single & double buffering and drawing/writing off the right or
       bottom edge of the frame buffer.
       For the console, make sure to test wrap-around and scrolling.
    */
}
