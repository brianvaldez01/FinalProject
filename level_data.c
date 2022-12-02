//y, room, x, type
//y = TURN_OFF end of list
// Enemies, Coins, and Star Locations
const unsigned char level_1_coins[]={
	0x70, 0, 0x00,
	0x40, 0, 0xb0,
	0x60, 1, 0x40,
	0x60, 2, 0xe0,
	TURN_OFF
};

const unsigned char level_2_coins[]={
	0x30, 0, 0x40, 
	0x60, 1, 0x20,
	0xd0, 2, 0x20,
	0x30, 2, 0x80,
	TURN_OFF
};

const unsigned char level_3_coins[]={
	0x30, 0, 0x20,
	0x30, 2, 0x20,
	0xd0, 2, 0xe0,
	TURN_OFF
};

const unsigned char * const Coins_list[]={
	level_1_coins, level_2_coins, level_3_coins
};



//y, room, x
//y = TURN_OFF end of list
const unsigned char level_1_enemies[]={
	0xd4, 1, 0x58,
	0xd4, 1, 0xb0,
	TURN_OFF
};

const unsigned char level_2_enemies[]={
	TURN_OFF
};

const unsigned char level_3_enemies[]={
	0xd4, 1, 0x10,
	TURN_OFF
};

const unsigned char * const Enemy_list[]={
	level_1_enemies, level_2_enemies, level_3_enemies
};



// Stars Things
const unsigned char level_1_stars[]={
	0xb0, 2, 0xe0,
	TURN_OFF
};

const unsigned char level_2_stars[]={
	0x30, 2, 0xc0,
	TURN_OFF
};

const unsigned char level_3_stars[]={
	0x30, 2, 0xd0,
	TURN_OFF
};

const unsigned char * const Stars_list[]={
	level_1_stars, level_2_stars, level_3_stars
};



// Metatiles Things
// 5 bytes per metatile definition, tile TL, TR, BL, BR, palette 0-3
// T means top, B means bottom, L left,R right
// 51 maximum # of metatiles = 255 bytes

const unsigned char metatiles[]={
	0, 0, 0, 0,  3,
	16, 16, 16, 16,  3,
	183, 184, 199, 200,  0,
	64, 65, 66, 67,  0,
	176, 176, 192, 192,  1,
	192, 192, 192, 192,  1,
	178, 178, 194, 194,  1,
	194, 194, 194, 194,  1,
	68, 68, 68, 68,  0,
	180, 181, 196, 197,  0,
	228, 229, 244, 245,  0,
	231, 232, 247, 248,  0,
	234, 235, 250, 251,  0,
	188, 189, 204, 205,  0,
	190, 191, 206, 207,  0,
	186, 187, 0, 0,  3
};


#define COL_DOWN 0x80
#define COL_ALL 0x40
#define COL_DEATH 0x120

// Using metatiles, define whether collidable or not
const unsigned char is_solid[]={
	0,
	COL_ALL,
	COL_ALL,
	COL_ALL,
	COL_DEATH,
	COL_DEATH,
	0,
	0,
	0,
	COL_DEATH | COL_ALL,
	COL_DEATH | COL_ALL,
	COL_DEATH | COL_ALL,
	COL_DEATH | COL_ALL,
	0,
	0,
	COL_DOWN
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