#ifndef __HID_H
#define __HID_H

#include "ctr/types.h"

// The register value seems to be flipped
#define REG_HID		((volatile u32*)0x10146000)

#define HID_A		((u32)0x001)
#define HID_B		((u32)0x002)
#define HID_SEL		((u32)0x004)
#define HID_START	((u32)0x008)
#define HID_RIGHT	((u32)0x010)
#define HID_LEFT	((u32)0x020)
#define HID_UP		((u32)0x040)
#define HID_DOWN	((u32)0x080)
#define HID_RT		((u32)0x100)
#define HID_LT		((u32)0x200)

u32 input_wait();

#endif // __HID_H