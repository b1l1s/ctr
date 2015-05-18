#include "ctr/console.h"
#include "ctr/draw.h"
#include "ctr/printf.h"

typedef struct console_s
{
	u32 fg;
	u32 bg;
	u32 x;
	u32 y;
} console_s;

static console_s console;

static void console_putc(void* p, char c)
{
	if(c == '\n' || (console.x + 8) > SUB_WIDTH)
	{
		console.x = 0;
		console.y += 8;
	}

	if(console.y >= SUB_HEIGHT)
	{
		draw_shift_up(SCREEN_SUB);
		console.y -= 8;

		draw_rect(SCREEN_SUB, 0, console.y, SUB_WIDTH, 8, console.bg);
	}

	if(c == '\r' || c == '\n') return;

	draw_char(SCREEN_SUB, console.x, console.y, console.fg, c);
	console.x += 8;
}

void console_init(u32 fg, u32 bg)
{
	console.x = 0;
	console.y = 0;
	console.fg = fg;
	console.bg = bg;

	draw_clear_screen(SCREEN_SUB, bg);
	init_printf(NULL, console_putc);
}
