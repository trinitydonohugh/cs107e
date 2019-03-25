#include "malloc.h"
#include "printf.h"
#include "strings.h"
#include "uart.h"

/*
 * Returns a pointer to a new null-terminated string containing at most `n`
 * bytes copied from the string pointed to by `src`.
 *
 * Example: strndup("cs107e", 4) == "cs10"
 */
static char *strndup(const char *src, size_t n)
{
    char *copyStr = malloc(n);
    memcpy(copyStr, src, n);
    copyStr[n-1] = NULL;
    return copyStr;
}

static int isspace(char ch)
{
    return ch == ' ' || ch == '\t' || ch == '\n';
}

static int tokenize(const char *line, char *array[],  int max)
{
    int ntokens = 0;

    while (*line != '\0' && ntokens < max) {
        while (isspace(*line)) line++;  // skip past spaces
        if (*line == '\0') break; // no more non-white chars
        const char *start = line;
        while (*line != '\0' && !isspace(*line)) line++; // advance to next space/null
        size_t nchars = line - start;
        array[ntokens++] = strndup(start, nchars);   // make heap-copy, add to array
    }
    return ntokens;
}

void main(void)
{
    uart_init();

    char *str = "Leland Stanford Junior University Established 1891";
    int max = strlen(str); // number of tokens is at most length of string
    
    char **array = malloc(max);   // declare stack array to hold strings

    int ntokens = tokenize(str, array, max);
    for (int i = 0; i < ntokens; i++)
        printf("[%d] = %s\n", i, array[i]);
    printf("\04");

    for (int i = 0; i < max; i++) {
        free(array[i]);
    }
    free(array);
}
