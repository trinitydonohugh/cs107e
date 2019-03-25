#include "shell.h"
#include "shell_commands.h"
#include "uart.h"
#include "malloc.h"
#include "printf.h"
#include "strings.h"
#include "pi.h"

#define LINE_LEN 80

static int (*shell_printf)(const char * format, ...);

static int numCommands = 5;
static const command_t commands[] = {
    {"help",   "<cmd> prints a list of commands or description of cmd", cmd_help},
    {"echo",   "<...> echos the user input to the screen", cmd_echo},
    {"reboot", "reboot the Raspberry Pi back to the bootloader", cmd_reboot},
    {"peek", "Prints the contents (4 bytes) of memory at address", cmd_peek},
    {"poke", "Stores `value` into the memory at `address`.", cmd_poke},
};

int cmd_echo(int argc, const char *argv[]) 
{
    for (int i = 1; i < argc; ++i) 
        shell_printf("%s ", argv[i]);
    shell_printf("\n");
    return 0;
}

int cmd_help(int argc, const char *argv[]) 
{
    if (argc == 1) {
        for (int i = 0; i < numCommands; i++) {
            shell_printf("%s:%s\n", commands[i].name, commands[i].description);
        }
        return 0;
    } else if (argc == 2) {
        for (int i = 0; i < numCommands; i++) {
            if (strcmp(commands[i].name, argv[1]) == 0) {
                shell_printf("%s: %s\n", commands[i].name, commands[i].description);
                return 0;
            }
        }
        printf ("error: no such command %s\n", argv[1]);
    }
    return 1;
}

int cmd_reboot(int argc, const char* argv[]) {
    pi_reboot();
}


int cmd_peek(int argc, const char* argv[]) {
    char *addressPointer = argv[1];
    char *str = 0;
    for (int i = 0; i < 4; i++) {
        str += addressPointer[i];
    }
    const char *endptr = '\0';
    unsigned int strtonumReturn = strtonum(&str, &endptr);

    if (argc < 2) {
        shell_printf ("error: peek requires 1 argument [address]");
        return 1;
    }
    if (strtonumReturn == 0) {
        shell_printf ("error: peek cannot convert %s\n", argv[1]);
        return  1;
    } else if (((unsigned int)(*addressPointer))%4 != 0) {
        shell_printf ("error: peek address must be 4-byte aligned");
    } else {
        shell_printf ("\n%s: %d\n", argv[1], strtonumReturn);
        return 0;
    }
}

int cmd_poke(int argc, const char* argv[]) {
    char *addressPointer = argv[1];
    char *value = argv[2];
    const char *endptr = '\0';
    unsigned int strtonumReturn = strtonum(value, &endptr);

    if (argc == 2) {
        shell_printf ("error: poke requires 2 arguments [address] and [value]");
        return 1;
    } 
    else if (addressPointer[0] != '0' && addressPointer[1] != '1') {
        shell_printf ("error: poke address must be 4-byte aligned");
        return 1;
    } 
    else {
        *addressPointer = strtonumReturn;
        shell_printf ("\n%s: %08x\n", argv[1], *addressPointer);
        return 0;
    }
}

void shell_init(formatted_fn_t print_fn)
{
    shell_printf = print_fn;
}

void shell_bell(void)
{
    uart_putchar('\a');
}

void shell_readline(char buf[], int bufsize)
{
    unsigned int bufSizeCounter = 0;
    unsigned char readBuf = keyboard_read_next();
    while (readBuf != '\n' && bufSizeCounter < (bufsize - 1)) {
        if (readBuf == '\b' && bufSizeCounter == 0) {
            shell_bell();
        } else if (readBuf == '\b') {
            shell_printf("%c", readBuf);
            shell_printf(" ");
            shell_printf("%c", readBuf);
            bufSizeCounter--;
        } else {
            shell_printf("%c", readBuf);
            buf[bufSizeCounter] = readBuf;
            bufSizeCounter++;
        }
        readBuf = keyboard_read_next();
    }
    buf[bufSizeCounter] = '\0';
}

//cite: lab4/string_main.
static char *strndup(const char *src, size_t n) 
{
    int srcLength = strlen(src);
    if (srcLength >= n) {
        srcLength = n;
    }
    char *space = malloc(n+1);
    space[n] = '\0';
    return memcpy(space, src, n);
}

static int isspace(char ch)
{
    return ch == ' ' || ch == '\t' || ch == '\n';
}

static int tokenize(const char *line, const char *array[],  int max)
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

int shell_evaluate(const char *line)
{
    const char *tokenArray[strlen(line)];
    int numTokens = tokenize(line, tokenArray, strlen(line));
    
    if (numTokens == 0) {
        return -1;
    }
    for (int i = 0; i < numCommands; i++) {
        if (strcmp(commands[i].name, tokenArray[0]) == 0) {
            return commands[i].fn(numTokens, tokenArray);
        }
    }
    printf ("error: no such command %s\n", tokenArray[0]);
    return -1;
}

void shell_run(void)
{
    shell_printf("Welcome to the CS107E shell. Remember to type on your PS/2 keyboard!\n");
    while (1) 
    {
        char line[LINE_LEN];

        shell_printf("Pi> ");
        shell_readline(line, sizeof(line));
        shell_evaluate(line);
    }
}