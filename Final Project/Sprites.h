unsigned char pad1;
unsigned char pad1_new;
unsigned char collision;
unsigned char collision_L;
unsigned char collision_R;
unsigned char collision_U;
unsigned char collision_D;
unsigned char which_bg;
const unsigned char * p_maps;
unsigned char coordinates;
unsigned char temp1;
unsigned char temp2;
unsigned char temp3;
unsigned char temp4;
unsigned char temp_x;
unsigned char temp_y;
unsigned char song;
unsigned char c_map[240];

#define MAX_SONGS 2
enum {SONG_GAME, SONG_PAUSE};
void change_song(void);

#include "TILESETS/beeoop.c" // called c1


const unsigned char * const All_Collision_Maps[] = {beeoop};

// a 16x16 pixel metasprite
const unsigned char sprPlayer[]={
  0,-1,0x49,0,
  8,-1,0x4a,0,
  0, 7,0x4b,0,
  8, 7,0x4c,0,
  128
};

const unsigned char sprGhost[]={
  0,-1,0x49,0,
  8,-1,0x4a,0,
  0, 7,0x4b,0,
  8, 7,0x4c,0,
  128
};

// PROTOTYPES
void draw_bg(void);
void draw_sprites(void);
void movement(void);	
void bg_collision(char * object);
void check_start(void);