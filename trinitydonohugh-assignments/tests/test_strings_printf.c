#include "assert.h"
#include "printf.h"
#include <stddef.h>
#include "strings.h"
#include "uart.h"

static void test_memset(void)
{
    int numA = 0xefefefef;
    int numB = 2;

    memset(&numB, 0xef, sizeof(int));
    assert(numA == numB);
}

static void test_memcpy(void)
{
    int numA = 0x12345678;
    int numB = 2;

    memcpy(&numB, &numA, sizeof(int));
    assert(numA == numB);
}

static void test_strlen(void)
{
    assert(strlen("green") == 5);
}

static void test_strcmp(void)
{
    assert(strcmp("apples", "apples") == 0);
}

static void test_strlcat(void)
{
    char buf[20];
    memset(buf, 0x77, sizeof(buf)); // init contents with known value

    buf[0] = '\0'; // start with empty string
    strlcat(buf, "CS", sizeof(buf));
    strlcat(buf, "107e", sizeof(buf));
    assert(strcmp(buf, "CS107e") == 0);

    buf[0] = 'a';
    buf[1] = 'b';
    buf[2] = 'c';
    strlcat(buf, "CS", sizeof(buf));
    strlcat(buf, "107e", sizeof(buf));
    assert(strcmp(buf, "abcCS107e") == 0);
}

static void test_strtonum(void)
{
    const char *input = "013", *rest = NULL;

    int val = strtonum(input, &rest);
    assert(val == 13 && rest == (input + strlen(input)));

    input = "0xabc";
    rest = NULL;
    val = strtonum(input, &rest);
    assert(val == 2748 && rest == (input + strlen(input)));
}

// These aren't part of printf public interface, we must declare them here to
// be able to use them in this test file.
int unsigned_to_base(char *buf, size_t bufsize, unsigned int val, int base, int min_width);
int signed_to_base(char *buf, size_t bufsize, int val, int base, int min_width);

static void test_to_base(void)
{
    char buf[5];
    memset(buf, 0x77, sizeof(buf)); // init contents with known value

    int n = unsigned_to_base(buf, 20, 33, 16, 3);
    assert(strcmp(buf, "021") == 0);
    assert(n == 3);

    n = unsigned_to_base(buf, 20, 94305, 10, 0);
    assert(strcmp(buf, "94305") == 0);
    assert(n == 5);

    n = unsigned_to_base(buf, 20, 233, 16, 0);
    assert(strcmp(buf, "e9") == 0);
    assert(n == 2);

    n = unsigned_to_base(buf, 20, 420, 16, 4);
    assert(strcmp(buf, "01a4") == 0);
    assert(n == 4);

    n = signed_to_base(buf, 5, -9999, 10, 6);
    assert(strcmp(buf, "-099") == 0);
    assert(n == 6);

    n = signed_to_base(buf, 5, 0, 10, 6);
    assert(strcmp(buf, "0") == 0);
    assert(n == 1);

    n = signed_to_base(buf, 20, 0x1A4, 16, 5);
    assert(strcmp(buf, "001a4") == 0);
    assert(n == 5);
}

static void test_snprintf(void)
{
    char buf[100];
    memset(buf, 0x77, sizeof(buf)); // init contents with known value

    int bufsize = sizeof(buf);

    // Start off simple...
    snprintf(buf, bufsize, "Hello, world!");
    int test = strcmp(buf, "Hello, world!");
    assert(test == 0);

    // Decimal
    snprintf(buf, bufsize, "%d", 45);
    assert(strcmp(buf, "45") == 0);

    // Hexadecimal
    snprintf(buf, bufsize, "%04x", 0xef);
    assert(strcmp(buf, "00ef") == 0);

    // Pointer
    snprintf(buf, bufsize, "%p", (void *) 0x20200004);
    assert(strcmp(buf, "0x20200004") == 0);

    // Character
    snprintf(buf, bufsize, "%c", 'A');
    assert(strcmp(buf, "A") == 0);

    snprintf(buf, bufsize, "%c", 'B');
    assert(strcmp(buf, "B") == 0);

    // String
    snprintf(buf, bufsize, "C%sS", "binky");
    assert(strcmp(buf, "CbinkyS") == 0);

    // Format string with intermixed codes
    snprintf(buf, bufsize, "CS%d%c!", 107, 'e');
    assert(strcmp(buf, "CS107e!") == 0);

    snprintf(buf, bufsize, "A%03d%c", 40, 'B');
    assert(strcmp(buf, "A040B") == 0);

    snprintf(buf, bufsize, "A%04x%c", 0xef, 'B');
    assert(strcmp(buf, "A00efB") == 0);

    snprintf(buf, bufsize, "A%03x%04d", 0xef, 30);
    assert(strcmp(buf, "A0ef0030") == 0);

    // Test return value
    assert(snprintf(buf, bufsize, "Hello") == 5);
    assert(snprintf(buf, 2, "Hello") == 5);
}


void main(void)
{
    uart_init();
    test_memset();
    test_memcpy();
    test_strlen();
    test_strcmp();
    test_strlcat();
    test_strtonum();
    test_to_base();
    test_snprintf();
}
