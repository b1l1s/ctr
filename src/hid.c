#include "ctr/hid.h"

#define HID_ALL_RELEASED 0xFFF

u32 input_wait()
{
	u32 prev = *REG_HID;
	u32 key;
	do
	{
		// Wait for state change
		while((key = *REG_HID) == prev);
		
		// Ignore key releases
		if(key > prev)
		{
			prev = key;
			continue;
		}

		// Simple debounce
		u32 deb = 0x7FFF;
		while(--deb)
		{
			if(key != *REG_HID)
			{
				// State changed, redo
				key = prev;
				break;
			}
		}
	} while(key == prev);
	
	// Flip the bits
	return key ^ HID_ALL_RELEASED;
}
