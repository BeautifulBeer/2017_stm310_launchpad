/*
 * main.c
 *
 *  Created on: 2017. 11. 24.
 *      Author: hw_3
 */

/*
 * flash setting
 *
 flash load "./launchpad/flashclear.axf"

 flash load "./launchpad/Debug/launchpad.axf"
 */
#include "bluetooth/config_bluetooth.h"
#include "init/config_system.h"
#include "init/config_nvic.h"
#include "init/config_exti.h"
#include "core_cm3.h"
#include "MPR121/config_mpr121.h"
#include "PWM/config_pwm.h"
#include "LCD/config_lcd.h"
#include <string.h>



#define MPR121_BUFFER_SIZE			2

/*
 * Global variables
 */

//System states
uint8_t systemStart_flag; 
uint8_t initalization_flag;

//TFT LCD Variables
int last_Xposition;
int last_Yposition;
int cur_Xposition;
int cur_Yposition;
uint8_t last_volume;
uint8_t cur_volume;
uint32_t DestAddr;

/*
	All drawings are represented as uint16_t array. Each value of array denotes position
	should be colored with black. 
*/

//16 X CONST_MUSIC_NOTE_ROW music note drawing
uint16_t MUSIC_NOTE[CONST_MUSIC_NOTE_ROW] = { 0x0000, 0x1FFE, 0x1FFE, 0x1806,
		0x1806, 0x1FFE, 0x1FFE, 0x1806, 0x1806, 0x1806, 0x1806, 0x1806, 0x1806,
		0x1806, 0x1806, 0x1806, 0x180E, 0x181F, 0x383F, 0x7C3F, 0xFC1E, 0xFC0C,
		0x7800, 0x3000 };

//(16*CONST_TREBLE_ROW) X CONST_TREBLE_COL treble drawing
uint16_t TREBLE[CONST_TREBLE_ROW][CONST_TREBLE_COL] = {
		{0x0, 0x1000},
		{0x0, 0x7000},
		{0x0, 0xf800},
		{0x1, 0xf800},
		{0x3, 0xfc00},
		{0x3, 0xfc00},
		{0x7, 0xfe00},
		{0x7, 0xfe00},
		{0x7, 0xc600},
		{0xf, 0x8700},
		{0xf, 0x700},
		{0xf, 0x700},
		{0xe, 0x700},
		{0xe, 0x700},
		{0xe, 0x700},
		{0xe, 0xf00},
		{0xe, 0xf00},
		{0xe, 0x1f00},
		{0xe, 0x1e00},
		{0xe, 0x3e00},
		{0xe, 0x7e00},
		{0xe, 0x7c00},
		{0xe, 0xfc00},
		{0x7, 0xfc00},
		{0x7, 0xf800},
		{0x7, 0xf800},
		{0x1f, 0xf000},
		{0x3f, 0xe000},
		{0x7f, 0xe000},
		{0x7f, 0xc000},
		{0xff, 0x8000},
		{0x1ff, 0x8000},
		{0x3ff, 0x8000},
		{0x7fb, 0x8000},
		{0xff3, 0x8000},
		{0xfe1, 0xc000},
		{0xfe1, 0xc000},
		{0x1fc1, 0xc000},
		{0x1f81, 0xfc00},
		{0x1f03, 0xff80},
		{0x3f07, 0xffc0},
		{0x3e0f, 0xffe0},
		{0x7e1f, 0xffe0},
		{0x7c1f, 0xfff0},
		{0x7c3f, 0xe7f0},
		{0x7c3e, 0x21f8},
		{0x7c3c, 0x70f8},
		{0x3c3c, 0x307c},
		{0x3c38, 0x307c},
		{0x3c38, 0x307c},
		{0x1c38, 0x383c},
		{0x1c1c, 0x387c},
		{0x1c1c, 0x183c},
		{0xe0e, 0x1878},
		{0x607, 0x1870},
		{0x703, 0x9cf0},
		{0x380, 0xce0},
		{0x1e0, 0xdc0},
		{0x78, 0x1f80},
		{0x3f, 0xff00},
		{0xf, 0xfe00},
		{0x0, 0x600},
		{0x0, 0x600},
		{0x0, 0x600},
		{0x0, 0x600},
		{0x0, 0x700},
		{0x8, 0x300},
		{0x3e, 0x300},
		{0x7f, 0x300},
		{0xff, 0x8300},
		{0x1ff, 0x8300},
		{0x1ff, 0x8300},
		{0x1ff, 0x8300},
		{0x1ff, 0x700},
		{0x1ff, 0x700},
		{0xfc, 0xe00},
		{0x7c, 0x3c00},
		{0x3f, 0xf800},
		{0xf, 0xe000},
		{0x3, 0x8000},
};

//(16*CONST_TREBLE_ROW) X CONST_MUSIC_NOTE_ROW speaker drawing
uint16_t SPEAKER[CONST_MUSIC_NOTE_ROW] = {
		0x0000, 0x0000, 0x001C, 0x003C, 0x00FC, 0x01FC, 0x3FFC,
		0x3FFC, 0x3FFC, 0x3FFC, 0x3FFC, 0x3FFC, 0x3FFC, 0x3FFC,
		0x3FFC, 0x3FFC, 0x3FFC, 0x3FFC, 0x3FFC, 0x01FC, 0x00FC, 0x003C,
		0x0000, 0x0000
};

// Waiting screen for this application
uint16_t MUSIC_BACKGROUND[TFT_BACKGROUND_HEIGHT][TFT_BACKGROUND_WIDTH] = {
		{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xd000, 0x0, 0x0},
		{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0xb000, 0x0, 0x0},
		{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xd800, 0x0, 0x0},
		{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x2, 0xe800, 0x0, 0x0},
		{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xb000, 0x0, 0x0},
		{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xa800, 0x0, 0x0},
		{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x6c00, 0x0, 0x0},
		{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xb400, 0x0, 0x0},
		{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x5400, 0x0, 0x0},
		{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x7400, 0x0, 0x0},
		{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x2c00, 0x0, 0x0},
		{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x6c00, 0x0, 0x0},
		{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x3600, 0x0, 0x0},
		{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x3a00, 0x0, 0x0},
		{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x2a00, 0x0, 0x0},
		{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1700, 0x0, 0x0},
		{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x3400, 0x0, 0x0},
		{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1b00, 0x0, 0x0},
		{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1500, 0x0, 0x0},
		{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1580, 0x0, 0x0},
		{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1500, 0x0, 0x0},
		{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x5a80, 0x0, 0x0},
		{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xd40, 0x0, 0x0},
		{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xa80, 0x0, 0x0},
		{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x540, 0x0, 0x0},
		{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xaa0, 0x0, 0x0},
		{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x540, 0x0, 0x0},
		{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x6a0, 0x0, 0x0},
		{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x540, 0x0, 0x0},
		{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x550, 0x0, 0x0},
		{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x650, 0x0, 0x0},
		{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x33a0, 0x0, 0x0},
		{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x22a8, 0x0, 0x0},
		{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x11d0, 0x0, 0x0},
		{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x248, 0x0, 0x0},
		{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1b50, 0x0, 0x0},
		{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1d4, 0x0, 0x0},
		{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x150, 0x0, 0x0},
		{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xd4, 0x0, 0x0},
		{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x128, 0x0, 0x0},
		{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xa4, 0x0, 0x0},
		{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x156, 0x0, 0x0},
		{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x90, 0x0, 0x0},
		{0x0, 0x0, 0x0, 0x50, 0x0, 0x0, 0x0, 0x0, 0xd6, 0x0, 0x0},
		{0x0, 0x0, 0x1, 0x7faa, 0x0, 0x0, 0x0, 0x0, 0x49, 0x0, 0x0},
		{0x0, 0x0, 0xe, 0xb6a1, 0x8000, 0x0, 0x0, 0x0, 0xa5, 0x0, 0x0},
		{0x0, 0x0, 0x7a, 0x513e, 0x6000, 0x0, 0x0, 0x0, 0x54, 0x0, 0x0},
		{0x0, 0x0, 0x78d1, 0xe0bb, 0x1000, 0x0, 0x0, 0x0, 0x4a, 0x8000, 0x0},
		{0x0, 0x0, 0x4742, 0xc11, 0x4c00, 0x0, 0x0, 0x0, 0x52, 0x0, 0x0},
		{0x0, 0x0, 0x4584, 0x8328, 0xa200, 0x0, 0x0, 0x0, 0x4a, 0x8000, 0x0},
		{0x0, 0x0, 0x5949, 0xc6, 0x2900, 0x0, 0x0, 0x0, 0x34, 0x8000, 0x0},
		{0x0, 0x0, 0x6252, 0x161, 0x14a0, 0x0, 0x0, 0x0, 0x24, 0x8000, 0x0},
		{0x0, 0x0, 0xd220, 0x98, 0x8a40, 0x0, 0x0, 0x0, 0x35, 0x4000, 0x0},
		{0x0, 0x0, 0xc434, 0x4, 0x5da0, 0x0, 0x0, 0x0, 0x6b, 0x0, 0x0},
		{0x0, 0x1, 0xc878, 0x2, 0x3e90, 0x0, 0x0, 0x0, 0x2b, 0x4000, 0x0},
		{0x0, 0x1, 0x80b8, 0x1, 0x9158, 0x0, 0x0, 0x0, 0x12, 0x4000, 0x0},
		{0x0, 0x3, 0x90b0, 0x0, 0x4924, 0x0, 0x0, 0x0, 0x12, 0xa000, 0x0},
		{0x0, 0x4, 0xa170, 0x0, 0x4494, 0x0, 0x0, 0x0, 0x14, 0x8000, 0x0},
		{0x0, 0x9, 0x3020, 0x0, 0x244a, 0x0, 0x0, 0x0, 0x12, 0x2000, 0x0},
		{0x0, 0x8, 0xe00, 0x0, 0x1225, 0x0, 0x0, 0x0, 0x15, 0x5000, 0x0},
		{0x0, 0x12, 0x4220, 0x0, 0x911, 0x0, 0x0, 0x0, 0x9, 0x1000, 0x0},
		{0x0, 0x4, 0x220, 0x0, 0x492, 0x8000, 0x0, 0x0, 0x9, 0x4000, 0x0},
		{0x0, 0x20, 0x4580, 0x0, 0x449, 0x4000, 0x0, 0x0, 0x8, 0x5000, 0x0},
		{0x0, 0x28, 0x81c0, 0x0, 0x245, 0x6000, 0x0, 0x0, 0xa, 0x8800, 0x0},
		{0x0, 0x40, 0xb80, 0x0, 0x123, 0x8000, 0x0, 0x0, 0xa, 0xa800, 0x0},
		{0x0, 0x9, 0xb80, 0x0, 0x113, 0xb000, 0x0, 0x0, 0x4, 0x2000, 0x0},
		{0x0, 0x90, 0x180, 0x0, 0x91, 0x4800, 0x0, 0x0, 0xa, 0xa800, 0x0},
		{0x0, 0x81, 0x1380, 0x0, 0x4b, 0x2800, 0x0, 0x0, 0x2, 0x400, 0x0},
		{0x0, 0x22, 0x1100, 0x0, 0xe8, 0x8400, 0x0, 0x0, 0xd, 0xd400, 0x0},
		{0x0, 0x120, 0x80, 0x0, 0x24, 0x1400, 0x0, 0x0, 0x32, 0x7000, 0x0},
		{0x0, 0x102, 0x2000, 0x0, 0x10, 0x4800, 0x0, 0x0, 0x5, 0x3400, 0x0},
		{0x0, 0x22, 0x2100, 0x0, 0x16, 0x4a00, 0x0, 0x0, 0x12, 0x5200, 0x0},
		{0x0, 0x240, 0x100, 0x0, 0x8, 0x2500, 0x0, 0x0, 0xa, 0x3200, 0x0},
		{0x0, 0x204, 0x2100, 0x0, 0x9, 0x2500, 0x0, 0x0, 0x9, 0x2800, 0x0},
		{0x0, 0x44, 0x2000, 0x0, 0x5, 0x1080, 0x0, 0x0, 0xe, 0xa200, 0x0},
		{0x0, 0x2f40, 0x4100, 0x0, 0x4, 0x8280, 0x0, 0x0, 0x2, 0xb900, 0x0},
		{0x0, 0x28bc, 0x100, 0x0, 0x0, 0x8900, 0x0, 0x0, 0x1, 0x1d00, 0x0},
		{0x0, 0x2407, 0x4200, 0x0, 0x6, 0x940, 0x0, 0x0, 0x1, 0x5800, 0x0},
		{0x0, 0x3487, 0x4000, 0x0, 0x0, 0x4520, 0x0, 0x0, 0x1, 0xd00, 0x0},
		{0x0, 0x308d, 0x8200, 0x0, 0x3, 0x45a0, 0x0, 0x0, 0x1, 0x5080, 0x0},
		{0x0, 0x3405, 0x8200, 0x0, 0x0, 0x2320, 0x0, 0x0, 0x1, 0x4480, 0x0},
		{0x0, 0x3888, 0x4000, 0x0, 0x1, 0x2650, 0x0, 0x0, 0x0, 0xa00, 0x0},
		{0x0, 0x2b40, 0x8200, 0x0, 0x0, 0xb040, 0x0, 0x0, 0x1, 0x48c0, 0x0},
		{0x0, 0x89c, 0x200, 0x0, 0x0, 0x8328, 0x0, 0x0, 0x0, 0xa200, 0x0},
		{0x0, 0x10e, 0x8000, 0x0, 0x0, 0x5028, 0x0, 0x0, 0x0, 0x2940, 0x0},
		{0x0, 0x100f, 0x4200, 0x0, 0x0, 0x4904, 0x0, 0x0, 0x0, 0xa440, 0x0},
		{0x0, 0x90b, 0x200, 0x0, 0x0, 0x894, 0x0, 0x0, 0x0, 0x8120, 0x0},
		{0x0, 0x1116, 0x8200, 0x0, 0x0, 0x6090, 0x0, 0x0, 0x2, 0x5500, 0x0},
		{0x0, 0x8, 0x8000, 0x0, 0x0, 0x44a, 0x0, 0x0, 0x8, 0x1020, 0x0},
		{0x0, 0x1110, 0x200, 0x0, 0x0, 0x254a, 0x0, 0x0, 0x0, 0x55a0, 0x0},
		{0x0, 0x1100, 0x8200, 0x0, 0x0, 0x1341, 0x0, 0x0, 0x11, 0x7c10, 0x0},
		{0x0, 0x1110, 0x8200, 0x0, 0x0, 0x13c5, 0x4000, 0x0, 0xa, 0x4e90, 0x0},
		{0x0, 0x8, 0x0, 0x0, 0x0, 0xbed, 0x0, 0x0, 0x18, 0x1210, 0x0},
		{0x0, 0x1210, 0x8200, 0x0, 0x0, 0x824, 0x8000, 0x0, 0x10, 0x48c0, 0x0},
		{0x0, 0x1100, 0x8200, 0x0, 0x0, 0x912, 0x8000, 0x0, 0x0, 0x2210, 0x0},
		{0x0, 0x1010, 0x0, 0x0, 0x0, 0x110, 0x0, 0x0, 0x0, 0x2948, 0x0},
		{0x1, 0x5708, 0x8200, 0x0, 0x0, 0xc12, 0xc000, 0x0, 0x0, 0x908, 0x0},
		{0x1, 0xb1f0, 0x8200, 0x0, 0x0, 0x89, 0x0, 0x0, 0x0, 0x2040, 0x0},
		{0x0, 0x11c, 0x0, 0x0, 0x0, 0x488, 0x4000, 0x0, 0x0, 0x1528, 0x0},
		{0x0, 0x110f, 0x8200, 0x0, 0x0, 0x481, 0x2000, 0x0, 0x0, 0x2424, 0x0},
		{0x0, 0x100f, 0x8200, 0x0, 0x0, 0x248, 0xa000, 0x0, 0x0, 0x1124, 0x0},
		{0x0, 0x10b, 0x8000, 0x0, 0x0, 0x204, 0x8000, 0x0, 0x0, 0x1480, 0x0},
		{0x0, 0x110d, 0x8200, 0x0, 0x0, 0x44, 0x1000, 0x0, 0x0, 0xb524, 0x0},
		{0x0, 0x110b, 0x8200, 0x0, 0x0, 0x320, 0x5000, 0x0, 0x0, 0x84d2, 0x0},
		{0x0, 0x1009, 0x4100, 0x0, 0x0, 0x22, 0x4800, 0x0, 0x0, 0x9272, 0x0},
		{0x0, 0x900, 0x100, 0x0, 0x0, 0x122, 0x4800, 0x0, 0x0, 0xc8f0, 0x0},
		{0x0, 0x1088, 0x4100, 0x0, 0x0, 0x101, 0x6000, 0x0, 0x0, 0x4a32, 0x0},
		{0x0, 0x808, 0x4100, 0x0, 0x0, 0xa1, 0x6800, 0x0, 0x0, 0x4851, 0x0},
		{0x0, 0x900, 0x4000, 0x0, 0x0, 0x91, 0xe400, 0x0, 0x0, 0x6249, 0x0},
		{0x0, 0x888, 0x4100, 0x0, 0x0, 0x81, 0xd000, 0x0, 0x0, 0x6e48, 0x0},
		{0x0, 0x804, 0x100, 0x0, 0x0, 0x10, 0x9400, 0x0, 0x0, 0x941, 0x0},
		{0x0, 0x84, 0x4000, 0x0, 0x0, 0xcb, 0x8200, 0x0, 0x0, 0x139, 0x0},
		{0x0, 0x884, 0x2100, 0x0, 0x0, 0x4c, 0x1200, 0x0, 0x0, 0x938, 0x8000},
		{0x0, 0x400, 0x2080, 0x0, 0x0, 0x1d0, 0x8900, 0x0, 0x0, 0x43c, 0x8000},
		{0x0, 0x884, 0x2080, 0x0, 0x0, 0x148, 0x6800, 0x0, 0x0, 0x514, 0x0},
		{0x0, 0x444, 0x2080, 0x0, 0x0, 0x184, 0xe100, 0x0, 0x0, 0x420, 0x8000},
		{0x0, 0x45f, 0xe000, 0x0, 0x0, 0x124, 0xa500, 0x0, 0x0, 0x514, 0x8000},
		{0x0, 0xc0, 0x3c80, 0x0, 0x0, 0x1a0, 0xe480, 0x0, 0x0, 0x92, 0x4000},
		{0x0, 0x4c2, 0xe80, 0x0, 0x0, 0x104, 0xc080, 0x0, 0x0, 0x480, 0x4000},
		{0x0, 0x482, 0x1f40, 0x0, 0x0, 0x123, 0x2600, 0x0, 0x0, 0x492, 0x0},
		{0x0, 0x242, 0x1700, 0x0, 0x0, 0x53e, 0x80, 0x0, 0x0, 0x212, 0x4000},
		{0x0, 0x460, 0x1540, 0x0, 0x0, 0xd0, 0x2240, 0x0, 0x0, 0x282, 0x2000},
		{0x0, 0x262, 0x1540, 0x0, 0x0, 0x1, 0x1100, 0x0, 0x0, 0x210, 0x4000},
		{0x0, 0x221, 0x1240, 0x0, 0x0, 0x11, 0x1140, 0x0, 0x0, 0x28a, 0x2000},
		{0x0, 0x231, 0x0, 0x0, 0x0, 0x9, 0x120, 0x0, 0x0, 0x209, 0x2000},
		{0x0, 0x31, 0x1040, 0x0, 0x0, 0x8, 0x1020, 0x0, 0x0, 0x89, 0x0},
		{0x0, 0x231, 0x820, 0x0, 0x0, 0x1, 0x900, 0x0, 0x0, 0x240, 0x2000},
		{0x0, 0x21e, 0xfc20, 0x0, 0x0, 0x8, 0x88b0, 0x0, 0x0, 0x249, 0x1000},
		{0x0, 0x131, 0x7a0, 0x0, 0x0, 0x8, 0x8080, 0x0, 0x0, 0x105, 0x1000},
		{0x0, 0x100, 0x9c0, 0x0, 0x0, 0x4, 0x810, 0x0, 0x0, 0x24d, 0x1000},
		{0x0, 0x111, 0x3e0, 0x0, 0x0, 0x4, 0x84c8, 0x0, 0x0, 0x124, 0x8000},
		{0x0, 0x110, 0x89e0, 0x0, 0x0, 0x4, 0x4408, 0x0, 0x0, 0x128, 0x9000},
		{0x0, 0x10, 0x84a0, 0x0, 0x0, 0x0, 0x4048, 0x0, 0x0, 0x54, 0x0},
		{0x0, 0x100, 0x4e0, 0x0, 0x0, 0x4, 0x4440, 0x0, 0x0, 0x175, 0xd000},
		{0x0, 0x110, 0x8420, 0x0, 0x0, 0x2, 0x224, 0x0, 0x0, 0x1a4, 0xf800},
		{0x0, 0x88, 0x4010, 0x0, 0x0, 0x2, 0x4224, 0x0, 0x0, 0x0, 0x7000},
		{0x0, 0x88, 0x410, 0x0, 0x0, 0x2, 0x2004, 0x0, 0x0, 0x124, 0xa800},
		{0x0, 0x88, 0x8400, 0x0, 0x0, 0x2, 0x2222, 0x0, 0x0, 0x122, 0x4800},
		{0x0, 0x80, 0x4210, 0x0, 0x0, 0x1, 0x112, 0x0, 0x0, 0x82, 0x4000},
		{0x0, 0x88, 0x4008, 0x0, 0x0, 0x1, 0x1110, 0x0, 0x0, 0xa2, 0x800},
		{0x0, 0x8, 0x608, 0x0, 0x0, 0x1, 0x1002, 0x0, 0x0, 0x102, 0x4400},
		{0x0, 0xc0, 0x4008, 0x0, 0x0, 0x0, 0x1119, 0x0, 0x0, 0xa0, 0x400},
		{0x0, 0x8, 0x2208, 0x0, 0x0, 0x1, 0x281, 0x0, 0x0, 0x92, 0x4400},
		{0x0, 0x44, 0x4200, 0x0, 0x0, 0x0, 0x9b88, 0x0, 0x0, 0x82, 0x2000},
		{0x0, 0x44, 0x2008, 0x0, 0x0, 0x0, 0x8d89, 0x0, 0x0, 0x90, 0x4400},
		{0x0, 0x0, 0x2204, 0x0, 0x0, 0x20, 0x8f44, 0x8000, 0x0, 0x12, 0x2400},
		{0x0, 0x44, 0x104, 0x0, 0x0, 0x3e, 0x9640, 0x8000, 0x0, 0x91, 0x0},
		{0x0, 0x24, 0x2304, 0x0, 0x0, 0xe, 0xc844, 0x0, 0x0, 0x80, 0x2400},
		{0x0, 0x42, 0x2c04, 0x0, 0x0, 0x2, 0x404, 0xc000, 0x0, 0x91, 0x2200},
		{0x0, 0x20, 0x3100, 0x0, 0x0, 0x5, 0x4442, 0x0, 0x0, 0x11, 0x2200},
		{0x0, 0x22, 0x5004, 0x0, 0x0, 0x0, 0x8422, 0x4000, 0x0, 0x5c1, 0x0},
		{0x0, 0x2, 0x9104, 0x0, 0x0, 0x2, 0xd020, 0x2000, 0x0, 0x458, 0x2200},
		{0x0, 0x22, 0x2082, 0x0, 0x0, 0x0, 0x4402, 0x2000, 0x0, 0x495, 0x200},
		{0x0, 0x21, 0xb082, 0x0, 0x0, 0x0, 0x2221, 0x0, 0x0, 0x24e, 0x2200},
		{0x0, 0x12, 0x43fc, 0x0, 0x0, 0x0, 0x2211, 0x2000, 0x0, 0x416, 0x9000},
		{0x0, 0x11, 0xbcbe, 0x0, 0x0, 0x0, 0x2011, 0x1000, 0x0, 0x2ca, 0x200},
		{0x0, 0x11, 0xe83e, 0x0, 0x0, 0x0, 0x200, 0x1000, 0x0, 0x611, 0x2200},
		{0x0, 0x13, 0x80ae, 0x0, 0x0, 0x0, 0x2111, 0x8000, 0x0, 0x2d0, 0x9100},
		{0x0, 0x3, 0x82b, 0x0, 0x0, 0x0, 0x1108, 0x3000, 0x0, 0x49, 0x1000},
		{0x0, 0x14, 0x894, 0x0, 0x0, 0x0, 0x1108, 0xd800, 0x0, 0x1c, 0x200},
		{0x0, 0x9, 0x849, 0x0, 0x0, 0x0, 0x1000, 0x7800, 0x0, 0x4e, 0x9100},
		{0x0, 0x8, 0x8041, 0x0, 0x0, 0x0, 0x1108, 0x7000, 0x0, 0x4a, 0x1100},
		{0x0, 0x8, 0x800, 0x0, 0x0, 0x0, 0x84, 0x6c00, 0x0, 0x4a, 0x9000},
		{0x0, 0x8, 0x8441, 0x0, 0x0, 0x0, 0x1885, 0xc000, 0x0, 0x40, 0x8100},
		{0x0, 0x8, 0x8420, 0x8000, 0x0, 0x0, 0x76, 0x400, 0x0, 0x48, 0x9100},
		{0x0, 0x0, 0x420, 0x8000, 0x0, 0x0, 0x880, 0x2200, 0x0, 0x48, 0x1100},
		{0x0, 0x8, 0x8020, 0x0, 0x0, 0x0, 0x842, 0x2200, 0x0, 0x41, 0x0},
		{0x0, 0x4, 0x8400, 0x8000, 0x0, 0x0, 0x402, 0x200, 0x0, 0x48, 0x9100},
		{0x0, 0x4, 0x4220, 0x4000, 0x0, 0x0, 0x440, 0x2000, 0x0, 0x40, 0x8100},
		{0x0, 0x4, 0x4210, 0x4000, 0x0, 0x0, 0x422, 0x1300, 0x0, 0x110, 0x1100},
		{0x0, 0x4, 0x200, 0x4000, 0x0, 0x0, 0x421, 0x1000, 0x0, 0x49, 0x1000},
		{0x0, 0x0, 0x4220, 0x4000, 0x0, 0x0, 0x1, 0x100, 0x0, 0x40, 0x8100},
		{0x0, 0x4, 0x4010, 0x0, 0x0, 0x0, 0x620, 0x1880, 0x0, 0x48, 0x9100},
		{0x0, 0x2, 0x2210, 0x4000, 0x0, 0x0, 0x21, 0x80, 0x0, 0x88, 0x1100},
		{0x0, 0x2, 0x100, 0x2000, 0x0, 0x0, 0x210, 0x8c80, 0x0, 0x41, 0x0},
		{0x0, 0x2, 0x2110, 0x2000, 0x0, 0x0, 0x210, 0x8600, 0x0, 0x90, 0x9100},
		{0x0, 0x2, 0x2108, 0x2000, 0x0, 0x0, 0x100, 0xe40, 0x0, 0x25b, 0x1100},
		{0x0, 0x0, 0x2008, 0x0, 0x0, 0x0, 0x110, 0xcc40, 0x0, 0x841, 0x9000},
		{0x0, 0x2, 0x100, 0x2000, 0x0, 0x0, 0x108, 0xc00, 0x0, 0x10, 0xe200},
		{0x0, 0x1fd, 0x2108, 0x1000, 0x0, 0x0, 0x8, 0x5460, 0x0, 0x91, 0xe100},
		{0x0, 0xfbf, 0xf088, 0x1000, 0x0, 0x0, 0x108, 0xa200, 0x0, 0x80, 0xb000},
		{0x0, 0x3801, 0x7800, 0x1000, 0x0, 0x0, 0x8d, 0x2220, 0x0, 0x11, 0x4200},
		{0x0, 0x6000, 0x3d88, 0x1000, 0x0, 0x0, 0x8c, 0x2210, 0x0, 0x91, 0x1200},
		{0x0, 0x5001, 0x3e04, 0x0, 0x0, 0x0, 0x4c, 0x1610, 0x0, 0x80, 0x2000},
		{0x0, 0x7000, 0xbe84, 0x1000, 0x0, 0x0, 0x88, 0x1710, 0x0, 0x111, 0x200},
		{0x0, 0x1000, 0x1700, 0x800, 0x0, 0x0, 0x4a, 0x1c88, 0x0, 0x22, 0x2200},
		{0x0, 0x3800, 0xaf44, 0x800, 0x0, 0x0, 0x4a, 0xc80, 0x0, 0x120, 0x0},
		{0x0, 0x1800, 0x9542, 0x800, 0x0, 0x0, 0x10, 0x1888, 0x0, 0x702, 0x4400},
		{0x0, 0xc00, 0x1742, 0x800, 0x0, 0x0, 0x6b, 0x6804, 0x0, 0x4e2, 0x2400},
		{0x0, 0x1c00, 0x8a02, 0x0, 0x0, 0x0, 0xb, 0x8c4, 0x0, 0x628, 0x4000},
		{0x0, 0xc00, 0x4a40, 0x800, 0x0, 0x0, 0x20, 0x404, 0x0, 0x406, 0x400},
		{0x0, 0xe00, 0x4042, 0x400, 0x0, 0x0, 0x11, 0x8442, 0x0, 0x646, 0x4000},
		{0x0, 0x600, 0x4421, 0x400, 0x0, 0x0, 0x10, 0x22, 0x0, 0x647, 0x800},
		{0x0, 0x600, 0x4402, 0x0, 0x0, 0x0, 0x10, 0x8620, 0x0, 0xc07, 0x800},
		{0x0, 0x700, 0x421, 0x400, 0x0, 0x0, 0x8, 0x4012, 0x0, 0xc85, 0x8800},
		{0x0, 0x77f, 0xe020, 0x200, 0x0, 0x0, 0x8, 0x4211, 0x0, 0xa8a, 0x0},
		{0x0, 0x7a5, 0x7e21, 0x200, 0x0, 0x0, 0x0, 0x100, 0x8000, 0x4c9, 0x1000},
		{0x0, 0x200, 0xf81, 0x200, 0x0, 0x0, 0xc, 0x6110, 0x8000, 0x20, 0x9000},
		{0x0, 0x400, 0x23d0, 0x8000, 0x0, 0x0, 0x0, 0x88, 0x8000, 0x919, 0x0},
		{0x0, 0x0, 0x23e0, 0x8200, 0x0, 0x0, 0x4, 0x2084, 0x0, 0x91c, 0x1000},
		{0x0, 0x0, 0x7f0, 0x100, 0x0, 0x0, 0x2, 0x1084, 0x4000, 0x101c, 0x2000},
		{0x0, 0x0, 0x2370, 0x8100, 0x0, 0x0, 0x2, 0x1040, 0x4004, 0xa216, 0x2000},
		{0x0, 0x0, 0x12f0, 0x8100, 0x0, 0x0, 0x1, 0x80e, 0x2000, 0x522c, 0x0},
		{0x0, 0x0, 0x12b0, 0x4000, 0x0, 0x0, 0x1, 0x46e, 0x2000, 0xa424, 0x4000},
		{0x0, 0x0, 0x1170, 0x100, 0x0, 0x0, 0x0, 0x841c, 0x1000, 0xa304, 0x4000},
		{0x0, 0x0, 0x11a8, 0x4080, 0x0, 0x0, 0x0, 0x8029, 0x1000, 0xa5c4, 0x0},
		{0x0, 0x0, 0x140, 0x4080, 0x0, 0x0, 0x0, 0x4e91, 0x800, 0x2100, 0x8000},
		{0x0, 0x0, 0x1008, 0x4080, 0x0, 0x0, 0x0, 0x5a0a, 0x8800, 0x4c48, 0x8000},
		{0x0, 0x0, 0x1108, 0x2000, 0x0, 0x0, 0x0, 0x290b, 0xc400, 0x4488, 0x0},
		{0x0, 0x0, 0x880, 0x2080, 0x0, 0x0, 0x0, 0x3907, 0x4000, 0x8b01, 0x0},
		{0x0, 0x0, 0x888, 0x2040, 0x0, 0x0, 0x0, 0x894, 0x2600, 0x8410, 0x0},
		{0x0, 0x0, 0x804, 0x40, 0x0, 0x0, 0x0, 0xd46, 0x2001, 0x1102, 0x0},
		{0x0, 0x0, 0xd4, 0x2040, 0x0, 0x0, 0x0, 0x840, 0x1102, 0x2022, 0x0},
		{0x0, 0x0, 0xb80, 0x1000, 0x0, 0x0, 0x0, 0x421, 0x102, 0x220, 0x0},
		{0x0, 0x0, 0xc04, 0x1040, 0x0, 0x0, 0x0, 0x210, 0x8880, 0xc044, 0x0},
		{0x0, 0x0, 0x484, 0x1020, 0x0, 0x0, 0x0, 0x210, 0x8446, 0x8400, 0x0},
		{0x0, 0x0, 0xc42, 0x1020, 0x0, 0x0, 0x0, 0x108, 0x446f, 0x888, 0x0},
		{0x0, 0x0, 0x1802, 0x100, 0x0, 0x0, 0x0, 0x84, 0x2211, 0x1110, 0x0},
		{0x0, 0x0, 0x1440, 0x5fe0, 0x0, 0x0, 0x0, 0x43, 0x1112, 0x6020, 0x0},
		{0x0, 0x0, 0x1047, 0xfff0, 0x0, 0x0, 0x0, 0x30, 0x88cc, 0x3240, 0x0},
		{0x0, 0x0, 0x343a, 0xff0, 0x0, 0x0, 0x0, 0x8, 0x446a, 0x5880, 0x0},
		{0x0, 0x0, 0x22e1, 0xf70, 0x0, 0x0, 0x0, 0x6, 0x33b3, 0x3200, 0x0},
		{0x0, 0x0, 0x5b01, 0x5f0, 0x0, 0x0, 0x0, 0x1, 0xdf5d, 0xf400, 0x0},
		{0x0, 0x0, 0x7c61, 0x550, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
		{0x0, 0x0, 0x7201, 0x6a8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
		{0x0, 0x0, 0xe220, 0x480, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
		{0x0, 0x0, 0xc221, 0x8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
		{0x0, 0x0, 0x0, 0x8404, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
		{0x0, 0x0, 0x220, 0x8204, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
		{0x0, 0x0, 0x110, 0x8204, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
		{0x0, 0x0, 0x110, 0x8000, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
		{0x0, 0x0, 0x100, 0x204, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
		{0x0, 0x0, 0x110, 0x8102, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
		{0x0, 0x0, 0x10, 0x4102, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
		{0x0, 0x0, 0x100, 0x4000, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
		{0x0, 0x0, 0x110, 0x4103, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
		{0x0, 0x0, 0x8, 0x80, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
		{0x0, 0x0, 0x108, 0x4081, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
		{0x0, 0x0, 0x108, 0x4001, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
		{0x0, 0x0, 0x88, 0x2080, 0x8000, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
		{0x0, 0x0, 0x100, 0x40, 0x8000, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
		{0x0, 0x0, 0x88, 0x2040, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
		{0x0, 0x0, 0x88, 0x2040, 0x8000, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
		{0x0, 0x0, 0x100, 0x2000, 0x4000, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
		{0x0, 0x0, 0x88, 0x60, 0x4000, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
		{0x0, 0x0, 0x84, 0x2000, 0x4000, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
		{0x0, 0x0, 0x0, 0x1020, 0x2000, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
		{0x0, 0x0, 0x108, 0x1010, 0x2000, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
		{0x0, 0x0, 0x84, 0x1010, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
		{0x0, 0x0, 0x84, 0x1010, 0x2000, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
		{0x0, 0x0, 0x100, 0x0, 0x1000, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
		{0x0, 0x0, 0x84, 0x1010, 0x1000, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
		{0x0, 0x0, 0x104, 0x11f8, 0x1000, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
		{0x0, 0x0, 0x104, 0xbfe, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
		{0x0, 0x0, 0x100, 0xfff, 0x8820, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
		{0x0, 0x0, 0x4, 0x1f85, 0xc8fe, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
		{0x0, 0x0, 0x104, 0x1f08, 0x4781, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
		{0x0, 0x0, 0x104, 0x3e04, 0x3c00, 0x8000, 0x0, 0x0, 0x0, 0x0, 0x0},
		{0x0, 0x0, 0x200, 0x3c00, 0xe400, 0xc000, 0x0, 0x0, 0x0, 0x0, 0x0},
		{0x0, 0x0, 0x104, 0x3c07, 0x100e, 0x4000, 0x0, 0x0, 0x0, 0x0, 0x0},
		{0x0, 0x0, 0x204, 0x3c1a, 0x141f, 0x4000, 0x0, 0x0, 0x0, 0x0, 0x0},
		{0x0, 0x1, 0x4204, 0x3c60, 0x123f, 0xc000, 0x0, 0x0, 0x0, 0x0, 0x0},
		{0x0, 0x2f, 0xfa00, 0x3d82, 0x1237, 0xc000, 0x0, 0x0, 0x0, 0x0, 0x0},
		{0x0, 0x7f, 0xff08, 0x3e01, 0x182b, 0x8000, 0x0, 0x0, 0x0, 0x0, 0x0},
		{0x0, 0xf0, 0x1fc8, 0x5e03, 0x1115, 0x8000, 0x0, 0x0, 0x0, 0x0, 0x0},
		{0x0, 0xc0, 0x7fb, 0x8f86, 0x110b, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
		{0x0, 0x1e0, 0x5fc, 0xfff, 0x3080, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
		{0x0, 0xf8, 0x4ff, 0x7fc, 0x3080, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
		{0x0, 0x7e, 0x8bf, 0xc850, 0xe080, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
		{0x0, 0x3f, 0xff0f, 0xf000, 0xc000, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
		{0x0, 0x7, 0xf817, 0xfc03, 0xc040, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
		{0x0, 0x0, 0xa001, 0xffaf, 0x8040, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
		{0x0, 0x0, 0x1010, 0xfffe, 0x8020, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
		{0x0, 0x0, 0x10, 0x1ffc, 0x20, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
		{0x0, 0x0, 0x2000, 0x5a0, 0x8020, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
		{0x0, 0x0, 0x2020, 0x800, 0x4000, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
		{0x0, 0x0, 0x4020, 0x400, 0x4010, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
		{0x0, 0x0, 0x4000, 0x800, 0x10, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
		{0x0, 0x0, 0x4020, 0x0, 0x4008, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
		{0x0, 0x0, 0x8040, 0x800, 0x2008, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
		{0x0, 0x0, 0x8020, 0x800, 0x2000, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
		{0x0, 0x1, 0x40, 0x400, 0x200c, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
		{0x0, 0x1, 0x0, 0x800, 0x2000, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
		{0x0, 0x1, 0x40, 0x0, 0x4, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
		{0x0, 0x2, 0x80, 0x800, 0x2002, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
		{0x0, 0x2, 0x0, 0x800, 0x2001, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
		{0x0, 0x4, 0x80, 0x800, 0x1001, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
		{0x0, 0x4, 0x80, 0x0, 0x1001, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
};


int LCD_Refresh_TIM2_flag;
int Volume_DOWN_EXTI2_flag;
int Volume_UP_EXTI5_flag;



//I2C Variables
// uint16_t lasttouched;
// uint16_t currtouched;
// uint8_t I2C_Buffer[MPR121_BUFFER_SIZE];
// int MPR121_IRQ_EXTI0_flag;
// int I2C1_ER_flag;
// int I2C1_ER_Status;


//PWM
int cur_pixel, cur_rgb, cur_pos, cur_reset;
int cur_pattern_num;
uint8_t PIXEL_PATTERN_1[CONST_PIXEL_NUM][CONST_RGB_NUM] = {
		{ 148, 0, 211 }, {75, 0, 130 }, { 0, 0, 255 }, { 0, 255, 0 },
		{ 255, 255, 0  }, {255, 127, 0 }, { 255, 0, 0 }, { 255, 255 , 255}
};


//Bluetooth Flag
int Button_1_EXTI11_flag;
int Button_2_EXTI12_flag;
uint8_t BLUETOOTH_USART2_flag;
//The states are defined in 'bluetooth/config_bluetooth.h'
uint8_t BT_STATUS;
uint16_t receive_offset;
uint8_t receive_buffer_[BT_STR_MAX_LENGTH];
char send_buffer_[BT_STR_MAX_LENGTH];
//Data buffer for bluetooth music request 
char requestMusic[5];

//Bluetooth Music Request
void EXTI15_10_IRQHandler(void){
	if (EXTI_GetITStatus(EXTI_Line11) == SET) {
		Button_1_EXTI11_flag = 1;
		EXTI_ClearITPendingBit(EXTI_Line11);
	}else if(EXTI_GetITStatus(EXTI_Line12) == SET) {
		Button_2_EXTI12_flag = 1;
		EXTI_ClearITPendingBit(EXTI_Line12);
	}
}

//TFT LCD Volume Up
void EXTI9_5_IRQHandler(void) { //volume up status
	if (EXTI_GetITStatus(EXTI_Line5) == SET) {
		Volume_UP_EXTI5_flag = 1;
		if(cur_volume != CONST_MAX_VOLUME){
			cur_volume++;
		}
		EXTI_ClearITPendingBit(EXTI_Line5);
	}
}

//TFT LCD Volume Down
void EXTI2_IRQHandler(void) { //volume down status
	if (EXTI_GetITStatus(EXTI_Line2) == SET) {
		Volume_DOWN_EXTI2_flag = 1;
		if(cur_volume != 0){
			cur_volume--;
		}
		EXTI_ClearITPendingBit(EXTI_Line2);
	}
}

void TIM4_IRQHandler(void){
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) == SET) {

		if(cur_pattern_num==8){
			cur_pattern_num=0;
		}
		cur_pattern_num++;


		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
	}
}

//PWM
/*
	This handler is 
*/
void TIM3_IRQHandler(void) {
	if (TIM_GetITStatus(TIM3, TIM_IT_CC2) == SET) {
		if (cur_pixel != CONST_PIXEL_NUM) {
			if (cur_rgb != CONST_RGB_NUM) {
				if (cur_pos != RGB_BIT_SIZE) {
					m_send_BIT(TIM3, TIM_Channel_2,
							PIXEL_PATTERN_1[(cur_pixel + cur_pattern_num) % CONST_PIXEL_NUM][cur_rgb]
							//PIXEL_PATTERN_1[cur_pixel][cur_rgb]
									& _BV(7 - cur_pos));
					cur_pos++;
				} else {
					cur_pos = 0;
					cur_rgb++;
				}
			} else {
				cur_rgb = 0;
				cur_pixel++;
			}
		} else {
			if (cur_reset != CONST_RESET_PERIOD) {
				m_send_BIT(TIM3, TIM_Channel_2, 2);
				cur_reset++;
			} else {
				//Initialize all variables
				cur_reset = 0;
				cur_pixel = 0;
			}
		}
		TIM_ClearITPendingBit(TIM3, TIM_IT_CC2);
	}
}

//TFT LCD Refresher
void TIM2_IRQHandler(void){
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET) {
		if (cur_Yposition > CONST_TFT_Y) {
			cur_Yposition = 0;
		} else {
			cur_Yposition += 12;
		}
		LCD_Refresh_TIM2_flag = 1;
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}

void USART2_IRQHandler(void){
	if (USART_GetITStatus(USART2, USART_IT_RXNE) == SET) {
		uint8_t data = (uint8_t)USART_ReceiveData(USART2);
		if(data == BT_STATUS_SYSTEM_START){
			systemStart_flag = 1;
			initalization_flag = 0;
			BT_STATUS = BT_STATUS_SYSTEM_START;
		}
		if(data == BT_STATUS_MUSIC_PLAY){
			BT_STATUS = BT_STATUS_MUSIC_PLAY;
		}
		if(receive_offset == BT_STR_MAX_LENGTH || data == '\r'){
			BLUETOOTH_USART2_flag = 1;
		}else{
			receive_buffer_[receive_offset] = data;
			receive_offset++;
		}
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
	}
}


void USART1_IRQHandler(void){
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET) {
		uint16_t data = USART_ReceiveData(USART1);
		//m_USART_DataSend(USART1, data);
		m_USART_byteSend(USART2, data);
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
}

//void I2C1_ER_IRQHandler(void) {
//	I2C1_ER_flag = 1;
//	if (I2C_GetITStatus(I2C1, I2C_IT_BERR) != RESET) {
//		I2C1_ER_Status = 0;
//	} else if (I2C_GetITStatus(I2C1, I2C_IT_ARLO) != RESET) {
//		I2C1_ER_Status = 1;
//	} else if (I2C_GetITStatus(I2C1, I2C_IT_AF) != RESET) {
//		I2C1_ER_Status = 2;
//	} else if (I2C_GetITStatus(I2C1, I2C_IT_OVR) != RESET) {
//		I2C1_ER_Status = 3;
//	} else if (I2C_GetITStatus(I2C1, I2C_IT_PECERR) != RESET) {
//		I2C1_ER_Status = 4;
//	} else if (I2C_GetITStatus(I2C1, I2C_IT_TIMEOUT) != RESET) {
//		I2C1_ER_Status = 5;
//	} else if (I2C_GetITStatus(I2C1, I2C_IT_SMBALERT) != RESET) {
//		I2C1_ER_Status = 6;
//	}
//}

void Configure_LED() {
	GPIO_InitTypeDef GPIO_init;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

	GPIO_init.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_init.GPIO_Pin = (GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_7);
	GPIO_init.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOD, &GPIO_init);
}

int main() {
	//int delay;

	//System Configuration Initialization
	SysInit();
	SetSysClock(); // 40MHz

	//Clock Enable
	m_Init_GPIO_Clock(GPIOA);
	m_Init_GPIO_Clock(GPIOB);
	m_Init_GPIO_Clock(GPIOC);
	m_Init_GPIO_Clock(GPIOE);
	m_Init_GPIO_Clock(GPIOD);
	m_Init_AFIO_Clock(); //for timer


	//LED
	Configure_LED();

	//Bluetooth
	m_Init_USART2_Clock();
	m_Init_USART1_Clock();
	m_Init_BT_USART1_GPIOA();
	m_Init_BT_USART2_GPIOA();
	m_Init_BT_USART1();
	m_Init_BT_USART2();
	m_Init_USART2_NVIC();
	m_Init_USART1_NVIC();

	//TFT_LCD
	LCD_Init();

	//Main Loop
	while (1) {
		//Wait for android signal
		if(!systemStart_flag){
			if(!initalization_flag){
				loading_TFT(MUSIC_BACKGROUND);
				m_USART_DataSend(USART2, "AT+BTSCAN\r\n", send_buffer_);
				initalization_flag = 1;
			}
		}else{
			if(!initalization_flag){
				//I2C MPR121
				// enable I2C ER Interrupt
				//m_Init_I2C_Clock(I2C1);
				//m_Init_I2C_NVIC(I2C1, 0);
			//	m_Init_MPR121_GPIO(GPIOB, GPIO_Pin_6, 1);
			//	m_Init_MPR121_GPIO(GPIOB, GPIO_Pin_7, 1);
			//	m_Init_MPR121_GPIO(GPIOE, GPIO_Pin_0, 0);
			//	m_EXTI_GPIO_Interrupt(GPIO_PortSourceGPIOE, GPIO_PinSource0, EXTI_Line0,
			//			EXTI0_IRQn); // IRQ
			//	m_Init_MPR121_I2C(I2C1);
			//	m_Init_MPR121();
				//effect sounds
				m_Init_MPR121_GPIO(GPIOD, GPIO_Pin_11, 0); // Press Button 1
				m_Init_MPR121_GPIO(GPIOD, GPIO_Pin_12, 0); // Press Button 2
				//base sounds
				m_Init_MPR121_GPIO(GPIOC, GPIO_Pin_3, 0); // Selection Left
				m_Init_MPR121_GPIO(GPIOC, GPIO_Pin_4, 0); // Selection Right
				m_EXTI_GPIO_Interrupt(GPIO_PortSourceGPIOD, GPIO_PinSource11, EXTI_Line11,
							EXTI15_10_IRQn);
				m_EXTI_GPIO_Interrupt(GPIO_PortSourceGPIOD, GPIO_PinSource12, EXTI_Line12,
							EXTI15_10_IRQn);

				//TFT-LCD
				m_Init_LCD_GPIO(GPIOC, GPIO_PinSource2, 0);
				m_Init_LCD_GPIO(GPIOC, GPIO_PinSource5, 0);
				m_EXTI_GPIO_Interrupt(GPIO_PortSourceGPIOC, GPIO_PinSource5, EXTI_Line5,
						EXTI9_5_IRQn); // volume up
				m_EXTI_GPIO_Interrupt(GPIO_PortSourceGPIOC, GPIO_PinSource2, EXTI_Line2,
						EXTI2_IRQn); // volume down
				m_Init_TIM_Clock(TIM2);
				m_Init_TIM_NVIC(TIM2,0);
				m_Init_LCD_TIM(TIM2);

				Music_background(SPEAKER, TREBLE);
				m_Init_TIM_Clock(TIM3);
				m_Init_TIM_NVIC(TIM3,1);
				m_Init_TIM(TIM3, PERIOD, PRESCALAR);

				m_Init_PWM_GPIO(GPIOA, GPIO_Pin_6, 1);
				m_Init_PWM_GPIO(GPIOA, GPIO_Pin_7, 1);
				m_Init_PWM_GPIO(GPIOB, GPIO_Pin_0, 1);
				m_Init_PWM_GPIO(GPIOB, GPIO_Pin_1, 1);

				m_Init_PWM_TIM(TIM3, 1, 1, 1, 1);

				m_Init_TIM_Clock(TIM4);
				m_Init_TIM_NVIC(TIM4,0);
				m_Init_LED_TIM(TIM4);
				initalization_flag = 1;
			}
		}
		if(BLUETOOTH_USART2_flag){
//			if(receive_offset != BT_STR_MAX_LENGTH){
//				receive_buffer_[receive_offset] = '\0';
//			}
//			//LCD_ShowString(10,160,receive_buffer_, BLACK, WHITE);
			if(BT_STATUS == BT_STATUS_MUSIC_PLAY){
				requestMusic[0] = 'b';
				if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3)){
					requestMusic[1] = '0';
				}else{
					requestMusic[1] = '1';
				}
				if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_4)){
					requestMusic[2] = '0';
				}else{
					requestMusic[2] = '1';
				}
				requestMusic[3] = '\r';
				requestMusic[4] = '\n';
				m_USART_DataSend(USART2, requestMusic, send_buffer_);
			}
			BLUETOOTH_USART2_flag = 0;
			BT_STATUS = BT_STATUS_NONE;
			receive_offset = 0;
		}
		if (Button_1_EXTI11_flag) {
			m_USART_DataSend(USART2, "m0\r\n", send_buffer_);
			Button_1_EXTI11_flag = 0;
		}
		if (Button_2_EXTI12_flag) {
			m_USART_DataSend(USART2, "m1\r\n", send_buffer_);
			Button_2_EXTI12_flag = 0;
		}
		if (Volume_UP_EXTI5_flag) {
			printVolume(cur_volume, last_volume, 1);
			last_volume = cur_volume;
			Volume_UP_EXTI5_flag = 0;
			m_USART_DataSend(USART2,"v1\r\n", send_buffer_);
		}
		if (Volume_DOWN_EXTI2_flag) {
			printVolume(cur_volume, last_volume, 0);
			last_volume = cur_volume;
			Volume_DOWN_EXTI2_flag = 0;
			m_USART_DataSend(USART2, "v0\r\n", send_buffer_);
		}
		if (LCD_Refresh_TIM2_flag) {
			//Show_LCD_Status();
			removeObject(73 + last_Xposition, 250 - last_Yposition, MUSIC_NOTE);
			printObject(73 + cur_Xposition, 250 - cur_Yposition, MUSIC_NOTE);
			last_Yposition = cur_Yposition;
			LCD_Refresh_TIM2_flag = 0;
		}

	}
}
