#ifndef __ENDIAN_H
#define __ENDIAN_H

#include <stdint.h>

// Inplace swaps
#define BSWAP16(x) {\
	x = ((x & 0xFF) << 8) | ((x & 0xFF00) >> 8);\
};

#define BSWAP32(x) {\
	asm\
	(\
		"eor r1, %1, %1, ror #16\n\t"\
		"bic r1, r1, #0xFF0000\n\t"\
		"mov %0, %1, ror #8\n\t"\
		"eor %0, %0, r1, lsr #8\n\t"\
		:"=r"(x)\
		:"0"(x)\
		:"r1"\
	);\
};

inline uint16_t bswap16(uint16_t val)
{
	uint16_t res = val;
	BSWAP16(res);
	return res;
}

inline uint32_t bswap32(uint32_t val)
{
	uint32_t res = val;
	BSWAP32(res);
	return res;
}

#endif /*__ENDIAN_H*/