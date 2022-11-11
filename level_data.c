//y, room, x, type
//y = TURN_OFF end of list
// Enemies, Coins, and Star Locations
const unsigned char level_1_coins[]={
	0x60, 0, 0x10,
	0xa0, 0, 0xe0,
	0xd0, 1, 0x10,
	0xa0, 1, 0xb0,
	0x40, 1, 0xe0,
	0x40, 2, 0x30,
	0x70, 2, 0xa0,
	0xd0, 2, 0xc0,
	TURN_OFF
};

const unsigned char level_2_coins[]={
	0x80, 0, 0x10,
	0x40, 0, 0x60,
	0x90, 0, 0x80,
	0x80, 0, 0xe0,
	0x50, 1, 0x30,
	0xa0, 1, 0xb0,
	0x60, 2, 0x10,
	0x80, 2, 0xe0,
	TURN_OFF
};

const unsigned char level_3_coins[]={
	0x40, 0, 0x10,
	0x70, 0, 0x10,
	0x70, 0, 0xe0,
	0x90, 1, 0x20,
	0x30, 1, 0x40,
	0x30, 1, 0xe0,
	TURN_OFF
};

const unsigned char * const Coins_list[]={
	level_1_coins, level_2_coins, level_3_coins
};



//y, room, x
//y = TURN_OFF end of list
const unsigned char level_1_enemies[]={
	0xd5, 0, 0xd0,
	0xd5, 1, 0x80,
	0xd5, 2, 0x70,
	TURN_OFF
};

const unsigned char level_2_enemies[]={
	0xd5, 0, 0x90,
	0xd5, 1, 0xa0,
	0xd5, 2, 0x30,
	0xd5, 2, 0xa0,
	TURN_OFF
};

const unsigned char level_3_enemies[]={
	0xd5, 0, 0xb0,
	0xd5, 2, 0x20,
	0xd5, 2, 0xa0,
	TURN_OFF
};

const unsigned char * const Enemy_list[]={
	level_1_enemies, level_2_enemies, level_3_enemies
};



// Stars Things
const unsigned char level_1_stars[]={
	0xd0, 2, 0xe0,
	TURN_OFF
};

const unsigned char level_2_stars[]={
	0x30, 2, 0xe0,
	TURN_OFF
};

const unsigned char level_3_stars[]={
	0x30, 2, 0x10,
	TURN_OFF
};

const unsigned char * const Stars_list[]={
	level_1_stars, level_2_stars, level_3_stars
};



// Metatiles Things
// 5 bytes per metatile definition, tile TL, TR, BL, BR, palette 0-3
// T means top, B means bottom, L left,R right
// 51 maximum # of metatiles = 255 bytes

const unsigned char metatiles1[]={
	0, 0, 0, 0,  1,
	16, 16, 16, 16,  1
};


#define COL_DOWN 0x80
#define COL_ALL 0x40

// Using metatiles 1, define whether collidable or not
const unsigned char is_solid[]={
	0,
	COL_ALL
};


// Map Files and Things
#include "BG/Level1.c"
#include "BG/Level2.c"
#include "BG/Level3.c"


const unsigned char * const Levels_list[]={
Level1_0,Level1_1,Level1_2,
Level2_0,Level2_1,Level2_2,
Level3_0,Level3_1,Level3_2
};

const unsigned char Level_offsets[]={
	0,3,6
};

#define MAX_ROOMS (3-1)
#define MAX_SCROLL (MAX_ROOMS*0x100)-1