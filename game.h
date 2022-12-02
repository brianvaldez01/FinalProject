#define SPEED 0x180
#define MAX_RIGHT 0xb000
#define JUMP_VEL -0x600
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
unsigned int temp7;
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
unsigned char index2;
unsigned char room;
unsigned char map;
unsigned int scroll_x;
unsigned int scroll_y;
unsigned char scroll_count; 
unsigned int pseudo_scroll_x; 
//unsigned int pseudo_scroll_y;
unsigned char L_R_switch;
unsigned int old_x;
//unsigned int old_y;
unsigned char temp_x;
unsigned char temp_y;


// Game Mode Things
unsigned char game_mode;
enum {MODE_TITLE, MODE_GAME, MODE_END, MODE_GAME_OVER, MODE_SWITCH};

// Gameplay Things
unsigned char coins = 0;
const unsigned char * pointer;
unsigned char lives = 3;
unsigned char level;
unsigned char offset;
unsigned char level_up;
unsigned char bright;
unsigned char bright_count;
unsigned char death;
unsigned char timer;
unsigned char running;
unsigned char run_anim = 0;
unsigned char enemy_frames; //in case of skipped frames
unsigned char map_loaded; //only load it once
unsigned char short_jump_count;
unsigned int old_jump_x;
unsigned int old_jump_y;

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
struct Base Generic2;

struct Player {
  unsigned int x;
  unsigned int y;
  signed int vel_x;
  signed int vel_y;
};

struct Player PlayerGuy; // starting position
// the width and height should be 1 less than the dimensions (14x14)
// note, I'm using the top left as the 0,0 on x,y

#define HERO_WIDTH 5
#define HERO_HEIGHT 13


// Enemy Things
#define MAX_ENEMY 16
#define ENEMY_WIDTH 14
#define ENEMY_HEIGHT 10
unsigned char enemy_x[MAX_ENEMY];
unsigned char enemy_y[MAX_ENEMY];
unsigned char enemy_active[MAX_ENEMY];
unsigned char enemy_room[MAX_ENEMY];
unsigned char enemy_actual_x[MAX_ENEMY];

//for shuffling 16 enemies
const unsigned char shuffle_array[]={
0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0,
0,2,4,6,8,10,12,14,1,3,5,7,9,11,13,15,
15,13,11,9,7,5,3,1,14,12,10,8,6,4,2,0	
};


// Coin Things
#define MAX_COINS 16
#define COIN_WIDTH 7
#define COIN_HEIGHT 11
#define TURN_OFF 0xff
unsigned char coin_x[MAX_COINS];
unsigned char coin_y[MAX_COINS];
unsigned char coin_active[MAX_COINS];
unsigned char coin_room[MAX_COINS];
unsigned char coin_actual_x[MAX_COINS];

// Star Things
#define MAX_STARS 2
#define STAR_WIDTH 13
#define STAR_HEIGHT 13
#define TURN_OFF 0xff
unsigned char star_x[MAX_STARS];
unsigned char star_y[MAX_STARS];
unsigned char star_active[MAX_STARS];
unsigned char star_room[MAX_STARS];
unsigned char star_actual_x[MAX_STARS];


// Map Things
unsigned char c_map[240];
unsigned char c_map2[240];


// Music Things
#define MAX_SONGS 2
enum {SONG_GAME, SONG_PAUSE};
enum {SFX_JUMP, SFX_COIN, SFX_NOISE, SFX_STAR};
void change_song(void);

const unsigned char text[]="COINS:00  LIVES:0  LEVEL:0";

// PROTOTYPES
void show_title(void);
void fade_in(void);
void fade_out(void);
void load_room(void);
void draw_sprites(void);
void movement(void);	
void bg_collision(void);
void bg_check_low(void);
void bg_collision_sub(void);
void bg_collision_fast(void);
void draw_screen_R(void);
void new_cmap(void);
void bg_low(void);
char get_position(void);
void enemy_moves(void);
void sprite_collisions(void);
void check_spr_objects(void);
void sprite_obj_init(void);
void set_sprite_zero(void);