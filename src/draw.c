#include "ctr/draw.h"
#include "ctr/font.h"

#include <string.h>

void _memset_rgb3(void* ptr, const u32 rgb, size_t pixelSize)
{
	u8 r = rgb & 0xFF;
	u8 g = (rgb >> 8) & 0xFF;
	u8 b = (rgb >> 16) & 0xFF;
	
	u8* start = (u8*)ptr;
	u8* end = start + pixelSize * 3;
	while(start < end)
	{
		start[0] = r;
		start[1] = g;
		start[2] = b;
		start += 3;
	}
}

void _memset_aligned24(void* ptr, const u32 rgb, size_t blockSize)
{
	u32 tmp[6]; // 6 ints, 24 bytes, or 8 pixels
	_memset_rgb3(tmp, rgb, 8);
	
	u32* ptr32 = (u32*) ptr;
	while(blockSize--)
	{
		*ptr32++ = tmp[0];
		*ptr32++ = tmp[1];
		*ptr32++ = tmp[2];
		*ptr32++ = tmp[3];
		*ptr32++ = tmp[4];
		*ptr32++ = tmp[5];
	}
}

void draw_clear_top(u32 rgb)
{
	u32 size = TOP_WIDTH * TOP_HEIGHT * 3 / (6 * 4);
	_memset_aligned24((void*)TOP_LEFT_FRAME1, rgb, size);
	_memset_aligned24((void*)TOP_LEFT_FRAME2, rgb, size);
	_memset_aligned24((void*)TOP_RIGHT_FRAME1, rgb, size);
	_memset_aligned24((void*)TOP_RIGHT_FRAME2, rgb, size);
}

void draw_clear_sub(u32 rgb)
{
	u32 size = SUB_WIDTH * SUB_HEIGHT * 3 / (6 * 4);
	_memset_aligned24((void*)SUB_FRAME1, rgb, size);
	_memset_aligned24((void*)SUB_FRAME2, rgb, size);
}

#define SET_PIXEL(buffer, x, y, rgb)\
{\
	u32 offset = (HEIGHT * x + HEIGHT - y - 1) * 3;\
	*((u8*)buffer + offset++) = rgb & 0xFF;\
	*((u8*)buffer + offset++) = (rgb >> 8) & 0xFF;\
	*((u8*)buffer + offset++) = (rgb >> 16) & 0xFF;\
}

inline
void draw_pixel_topleft(u16 x, u16 y, u32 rgb)
{
	SET_PIXEL(TOP_LEFT_FRAME1, x, y, rgb);
	SET_PIXEL(TOP_LEFT_FRAME2, x, y, rgb);
}

inline
void draw_pixel_topright(u16 x, u16 y, u32 rgb)
{
	SET_PIXEL(TOP_RIGHT_FRAME1, x, y, rgb);
	SET_PIXEL(TOP_RIGHT_FRAME2, x, y, rgb);
}

void draw_pixel_top(u16 x, u16 y, u32 rgb)
{
	draw_pixel_topleft(x, y, rgb);
	draw_pixel_topright(x, y, rgb);
}

void draw_pixel_sub(u16 x, u16 y, u32 rgb)
{
	SET_PIXEL(SUB_FRAME1, x, y, rgb);
	SET_PIXEL(SUB_FRAME2, x, y, rgb);
}

void draw_clear_screen(u32 screen, u32 rgb)
{
	switch(screen)
	{
	case SCREEN_TOP:
	default:
		draw_clear_top(rgb);
		break;
	case SCREEN_SUB:
		draw_clear_sub(rgb);
		break;
	}
}

void draw_pixel(u32 screen, u16 x, u16 y, u32 rgb)
{
	switch(screen)
	{
	case SCREEN_TOP:
	default:
		draw_pixel_top(x, y, rgb);
		break;
	case SCREEN_SUB:
		draw_pixel_sub(x, y, rgb);
		break;
	}
}

void draw_char(u32 screen, u16 x, u16 y, u32 rgb, char c)
{
	void(*draw_pixel_fun)(u16, u16, u32) = draw_pixel_top;
	if(screen == SCREEN_SUB) draw_pixel_fun = draw_pixel_sub;

	// We only include characters from 32 until 126
	//if(c > 126) c = 32;
	//c -= 32;

	u32 _x, _y, _c = c * 8;
	for(_y = 0; _y < 8; _y++)
	{
		u8 mask = 0b10000000;
		u8 row = font[_y + _c];
		for(_x = 0; _x < 8; _x++, mask >>= 1)
		{
			if(row & mask)
				draw_pixel_fun(x + _x, y + _y, rgb);
		}
	}
}

void draw_str(u32 screen, u16 x, u16 y, u32 rgb, const char* str)
{
	u32 width = TOP_WIDTH;
	if(screen == SCREEN_SUB) width = SUB_WIDTH;

	size_t len = strlen(str);

	u32 _x = x;
	u32 line = 0;

	int i;
	for(i = 0; i < len; ++i)
	{
		if(str[i] == '\n' || ((_x + 8) > width))
		{
			//_x = x;
			//line++;
			break;
		}

		if(str[i] == '\r' || str[i] == '\n') continue;

		draw_char(screen, _x, y + line * 8, rgb, str[i]);
		_x += 8;
	}
}

void memcpy32(void* dst, const void* src, size_t size)
{
	u32* dst32 = (u32*)dst;
	const u32* src32 = (const u32*)src;
	while(size--)
	{
		*dst32++ = *src32++;
	}
}

void draw_shift_up(u32 screen)
{
	// Buffer is bottom to top, left to right
	u32 x, y;
	for(x = 0; x < SUB_WIDTH; x++)
	{
		for(y = HEIGHT * 3 - 24; y != 0; y-=24) // y dim here is bytes
		{
			void* col = (u8*)SUB_FRAME1 + x * HEIGHT * 3 + y;
			memcpy32(col, col - 6 * 4, 6);
			
			col = (u8*)SUB_FRAME2 + x * HEIGHT * 3 + y;
			memcpy32(col, col - 6 * 4, 6);
		}
	}
}

void draw_shift_down(u32 screen)
{
	// Buffer is bottom to top, left to right
	// Size in ints - 6 ints
	const u32 copySize = HEIGHT / 4 * 3 - 6;
	
	u32 x;
	for(x = 0; x < SUB_WIDTH; x++)
	{
		// Copy from lower 8 pixels(6 ints)
		void* col = (u8*)SUB_FRAME1 + x * HEIGHT * 3;
		memcpy32(col, col + 6 * 4, copySize);
		
		col = (u8*)SUB_FRAME2 + x * HEIGHT * 3;
		memcpy32(col, col + 6 * 4, copySize);
	}
}

void _draw_rect(u8* fb, u16 x, u16 y, u16 width, u16 height, u32 rgb)
{
	y = HEIGHT - (y + height);

	u32 colStart = x * HEIGHT + y;
	u32 colEnd = x * HEIGHT + y + height;
	
	u32 prepend = ((colStart + 7) & ~7) - colStart;
	u32 postpend = colEnd & 7;
	u32 mid = height - (prepend + postpend);
	
	u32 mid_8 = mid / 8;

	int _x;
	for(_x = 0; _x < width; ++_x)
	{
		u8* ptr = fb + ((x + _x) * HEIGHT + y) * 3;
		_memset_rgb3(ptr, rgb, prepend);
		
		ptr += prepend * 3;
		_memset_aligned24(ptr, rgb, mid_8);
		
		ptr += mid * 3;
		_memset_rgb3(ptr, rgb, postpend);
	}
}

void draw_rect(u32 screen, u16 x, u16 y, u16 width, u16 height, u32 rgb)
{
	switch(screen)
	{
	case SCREEN_TOP:
	default:
		_draw_rect((u8*)TOP_LEFT_FRAME1, x, y, width, height, rgb);
		_draw_rect((u8*)TOP_LEFT_FRAME2, x, y, width, height, rgb);
		_draw_rect((u8*)TOP_RIGHT_FRAME1, x, y, width, height, rgb);
		_draw_rect((u8*)TOP_RIGHT_FRAME2, x, y, width, height, rgb);
		break;
	case SCREEN_SUB:
		_draw_rect((u8*)SUB_FRAME1, x, y, width, height, rgb);
		_draw_rect((u8*)SUB_FRAME2, x, y, width, height, rgb);
		break;
	}
}
