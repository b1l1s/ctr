#ifndef __DRAW_H
#define __DRAW_H

#include "ctr/types.h"

#define TOP_WIDTH			400
#define TOP_HEIGHT			240
#define SUB_WIDTH			320
#define SUB_HEIGHT			240

#define HEIGHT				240

typedef enum
{
	SCREEN_TOP = 0,
	SCREEN_SUB = 1,
} draw_screen;

typedef struct draw_s
{
	void* top_left;
	void* top_right;
	void* sub;
} draw_s;

void draw_init(const draw_s* _draw);
void draw_clear_screen(u32 screen, u32 rgb);
void draw_pixel(u32 screen, u16 x, u16 y, u32 rgb);
void draw_rect(u32 screen, u16 x, u16 y, u16 width, u16 height, u32 rgb);
void draw_char(u32 screen, u16 x, u16 y, u32 rgb, char c);
void draw_str(u32 screen, u16 x, u16 y, u32 rgb, const char* str);

void draw_shift_up(u32 screen);
void draw_shift_down(u32 screen);

#endif /*__DRAW_H*/
