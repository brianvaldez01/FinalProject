#define SPEED 0x180
#define MAX_RIGHT 0xb000
#define JUMP_VEL -0x300
#define GRAVITY 0x50


#pragma bss-name(push, "ZEROPAGE")

// Global Variables
unsigned char pad1;
unsigned char pad1_new;
unsigned char collision;
unsigned char collision_L;
unsigned char collision_R;
unsigned char collision_U;
unsigned char collision_D;
unsigned char coordinates;
unsigned char temp1;
unsigned char temp2;
unsigned char temp3;
unsigned char temp4;
unsigned int temp5;
unsigned int temp6;
unsigned char eject_L;
unsigned char eject_R;
unsigned char eject_U;
unsigned char eject_D;
unsigned char direction;
#define LEFT 0
#define RIGHT 1
unsigned char song;

// Stuff for scrolling
int address;
unsigned char x; // room loader code
unsigned char y;
unsigned char nt;
unsigned char index;
unsigned char room;
unsigned char map;
unsigned int scroll_x;
unsigned int scroll_y;
unsigned char scroll_count; 
unsigned int pseudo_scroll_x; 
//unsigned int pseudo_scroll_y;
unsigned char L_R_switch;
unsigned int old_x;
unsigned int old_y;


#pragma bss-name(push, "BSS")


#define FP_BITS 4;

// Player Things
struct Base {
  unsigned char x;
  unsigned char y;
  unsigned char width;
  unsigned char height;
};

struct Base Generic;

struct Player {
  unsigned int x;
  unsigned int y;
  signed int vel_x;
  signed int vel_y;
};

struct Player PlayerGuy = {0x1000,0xc200}; // starting position
// the width and height should be 1 less than the dimensions (14x14)
// note, I'm using the top left as the 0,0 on x,y

#define HERO_WIDTH 13
#define HERO_HEIGHT 13


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


// Map Things
unsigned char c_map[240];
unsigned char c_map2[240];
unsigned char which_bg;
const unsigned char * p_maps;


// Music Things
#define MAX_SONGS 2
enum {SONG_GAME, SONG_PAUSE};
void change_song(void);


// Map Files and Things
#include "BG/room1.c"
#include "BG/room2.c"
#include "BG/room3.c"

#define MAX_ROOMS (3-1)
#define MAX_SCROLL (MAX_ROOMS*0x100)-1

const unsigned char * const Rooms[] = {room1, room2, room3};


// PROTOTYPES
void show_title(void);
void fade_in(void);
void fade_out(void);
void load_room(void);
void draw_sprites(void);
void movement(void);	
void bg_collision(void);
void bg_collision_sub(void);
void draw_screen_R(void);
void new_cmap(void);
void bg_low(void);