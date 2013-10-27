#include <stdint.h>
#include <lib/earlycon.h>
#include <lib/stlinky.h>

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <arch/antares.h>
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"


FILE* g_early_stdin;
FILE* g_early_stdout;

static ssize_t cookie_read_function(void *_cookie, char *_buf, size_t n)
{
	size_t i = 0;
	while (i < n) { 
		*_buf++ = (char) early_getc();
	} ;
	return n;
}

static ssize_t cookie_write_function(void *_cookie, const char *_buf, size_t n)
{
	size_t i = 0;
	while (i < n) 
		early_putc(_buf[i++]);
	return n;
}


static int cookie_seek_function(void *cookie, off_t *off, int whence)
{
    errno = -1;
    return -1;
}

static int cookie_close_function(void *cookie)
{
    return 0;
}

 
#define send(text) \
	stlinky_tx(&g_stlinky_term, text, strlen(text)) 
void early_console_glue_init() {
	cookie_io_functions_t ops_read = {
		.read = cookie_read_function,
		.write = NULL, 
		.seek = cookie_seek_function,
		.close = cookie_close_function
	};
	cookie_io_functions_t ops_write = {
		.read = NULL,
		.write = cookie_write_function,
		.seek = cookie_seek_function,
		.close = cookie_close_function

	};
	g_early_stdin  = fopencookie(NULL, "r", ops_read);
	g_early_stdout = fopencookie(NULL, "w", ops_write);
	setvbuf(g_early_stdout, NULL, _IOLBF, 1);
        setvbuf(g_early_stdin,  NULL, _IOLBF, 1);
}
