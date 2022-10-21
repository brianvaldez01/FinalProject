#include "LIB/neslib.h"
#include "LIB/nesdoug.h"
#include "TILESETS/beep.h"
#include "Sprites.h"


#pragma bss-name(push, "ZEROPAGE")


#pragma bss-name(push, "BSS")

struct Assets {
	unsigned char x;
	unsigned char y;
	unsigned char width;
	unsigned char height;
};

struct Assets GoodGuy1 = {128,64,15,15};
struct Assets BadGuy1 = {128,168,15,15};
struct Assets BadGuy2 = {120,120,15,15};
struct Assets BadGuy3 = {120,120,15,15};
struct Assets BadGuy4 = {120,120,15,15};
struct Assets BadGuy5 = {120,120,15,15};
struct Assets BadGuy6 = {120,120,15,15};
struct Assets coin = {100, 100, 7, 7};
unsigned int collided = 0;

// Max amount of enemies on our screen
#define maxEnemies 5

// List of enemies
struct Assets enemyList[maxEnemies] = {{128,168,15,15}, {120,120,15,15}, {120,120,15,15}, {120,120,15,15}, {120,120,15,15}};

// Enemy velocities
int enemy_dx[maxEnemies];
int enemy_dy[maxEnemies];


static unsigned char wait;//
static unsigned char frame_cnt;//
static int iy,dy, Goodguy_dx, Goodguy_dy;//


const unsigned char palTitle[]={ 0x0f,0x03,0x15,0x30,0x0f,0x01,0x21,0x31,0x0f,0x06,0x30,0x26,0x0f,0x09,0x19,0x29 };


// Shows title screen
void show_title() {
  // Disable Rendering
  ppu_off();
  
  // Unpack nametable into VRAM
  vram_adr(NAMETABLE_A);
  vram_unrle(beep);
  
  // Enable Rendering
  ppu_on_all();
  
iy=220<<FP_BITS;


  while(1) {
    ppu_wait_frame();
    
    if (pad_trigger(0)&PAD_START) 
	{break;}


	iy+=dy;//
	if(iy<0)//
	{//
		iy =0;//
		dy=-dy>>1;//
	}//
	if(wait)//
	{//
		--wait;//
	}//
	else//
	{//
		pal_col(10,(frame_cnt&32)?0x0f:0x20);//
		++frame_cnt;//
	}//
  }

}

void fade_out() {
  char vb;
  for ( vb =4; vb!=0; vb--) {
    // Set brightness value
    pal_bright(vb);
    // wait for 4/60 sec
    ppu_wait_frame();
    ppu_wait_frame();
    ppu_wait_frame();
    ppu_wait_frame();
  }
 
  pal_bright(0);
  set_vram_update(NULL);
}
// Function to fade in screen
void fade_in() {
  char vb;
  for(vb=0; vb<=4; vb++) {
    // Set brighness value
    pal_bright(vb);
    // wait for 4/60 sec
    ppu_wait_frame();
    ppu_wait_frame();
    ppu_wait_frame();
    ppu_wait_frame();
  }
}

void draw_bg(void){
	ppu_off(); // screen off
	
	p_maps = All_Collision_Maps[which_bg];
	// copy the collision map to c_map
	memcpy (c_map, p_maps, 240);
	
	// this sets a start position on the BG, top left of screen
	vram_adr(NAMETABLE_A);
	
	// draw the tiles
	for(temp_y = 0; temp_y < 15; ++temp_y){
		for(temp_x = 0; temp_x < 16; ++temp_x){
			temp1 = (temp_y << 4) + temp_x;

			if(c_map[temp1]){
				vram_put(0x10); // wall
				vram_put(0x10);
			}
			else{
				vram_put(0); // blank
				vram_put(0);
			}
		}
				// do twice
		for(temp_x = 0; temp_x < 16; ++temp_x){
			temp1 = (temp_y << 4) + temp_x;

			if(c_map[temp1]){
				vram_put(0x10); // wall
				vram_put(0x10);
			}
			else{
				vram_put(0); // blank
				vram_put(0);
			}
		}

	}

	ppu_on_all(); // turn on screen
}


void draw_sprites(void){
	// clear all sprites from sprite buffer
	oam_clear();
	
	// draw first 2 metasprites
	oam_meta_spr(GoodGuy1.x, GoodGuy1.y, sprPlayer);
	oam_meta_spr(enemyList[0].x, enemyList[0].y, sprGhost);

	// draw rest of enemies
	for(i = 1; i < numEnemies; i++) {
		oam_meta_spr(enemyList[i].x, enemyList[i].y, sprGhost);
	}

	// draw coin
	oam_meta_spr(coin.x, coin.y, sprCoin);
}
	
	
void movement(void){
	if(pad1 & PAD_LEFT){
		GoodGuy1.x -= 1;
	}
	else if (pad1 & PAD_RIGHT){
		GoodGuy1.x += 1;
	}
	
	bg_collision((char *)&GoodGuy1);
	if(collision_R) GoodGuy1.x -= 1;
	if(collision_L) GoodGuy1.x += 1;
	
	if(pad1 & PAD_UP){
		GoodGuy1.y -= 1;
	}
	else if (pad1 & PAD_DOWN){
		GoodGuy1.y += 1;
	}
	
	bg_collision((char *)&GoodGuy1);
	if(collision_D) GoodGuy1.y -= 1;
	if(collision_U) GoodGuy1.y += 1;

	for(i = 0; i < numEnemies; i++) {
		bg_collision((char *)&enemyList[i]);
		if(collision_R) enemyList[i].x -= 1;
		if(collision_L) enemyList[i].x += 1;
		bg_collision((char *)&enemyList[i]);
		if(collision_D) enemyList[i].y -= 1;
		if(collision_U) enemyList[i].y += 1;
	}

	Goodguy_dx = 0;//
	Goodguy_dy = 0;//
	GoodGuy1.x += Goodguy_dx;//
	GoodGuy1.y += Goodguy_dy;//
	enemy_dx[0] = (GoodGuy1.x - enemyList[0].x);
	enemy_dy[0] = (GoodGuy1.y - enemyList[0].y);
	enemyList[0].x += enemy_dx[0]/20;
	enemyList[0].y += enemy_dy[0]/20;

	for(i = 1; i < numEnemies; i++) {
		enemy_dx[i] = (enemyList[i - 1].x - enemyList[i].x);
		enemy_dy[i] = (enemyList[i - 1].y - enemyList[i].y);
		enemyList[i].x += enemy_dx[i]/20;
		enemyList[i].y += enemy_dy[i]/20;
	}
}	



void bg_collision(char * object){
	// sprite collision with backgrounds
	// object expected to have first 4 bytes as x,y,width,height
	// casting to char* so this function could work for any sized structs
	collision_L = 0;
	collision_R = 0;
	collision_U = 0;
	collision_D = 0;
	
	temp1 = object[0]; // left side
	temp2 = temp1 + object[2]; // right side
	temp3 = object[1]; // top side
	temp4 = temp3 + object[3]; // bottom side
	
	if(temp3 >= 0xf0) return;
	// y out of range
	
	coordinates = (temp1 >> 4) + (temp3 & 0xf0); // upper left
	if(c_map[coordinates]){ // find a corner in the collision map
		++collision_L;
		++collision_U;
	}
	
	coordinates = (temp2 >> 4) + (temp3 & 0xf0); // upper right
	if(c_map[coordinates]){
		++collision_R;
		++collision_U;
	}
	
	if(temp4 >= 0xf0) return;
	// y out of range
	
	coordinates = (temp1 >> 4) + (temp4 & 0xf0); // bottom left
	if(c_map[coordinates]){
		++collision_L;
		++collision_D;
	}
	
	coordinates = (temp2 >> 4) + (temp4 & 0xf0); // bottom right
	if(c_map[coordinates]){
		++collision_R;
		++collision_D;
	}
}



void test_collision(void){

	collision1 = check_collision(&GoodGuy1, &enemyList[0]);
	collision2 = check_collision(&GoodGuy1, &enemyList[1]);
	collision3 = check_collision(&GoodGuy1, &enemyList[2]);
	collision4 = check_collision(&GoodGuy1, &enemyList[3]);
	collision5 = check_collision(&GoodGuy1, &enemyList[4]);

	if (numEnemies == 1) {
		if (collision1) playerDead = 1;
		else playerDead = 0;
	} else if (numEnemies == 2) {
		if (collision1 || collision2) playerDead = 1;
		else playerDead = 0;
	} else if (numEnemies == 3) {
		if (collision1 || collision2 || collision3) playerDead = 1;
		else playerDead = 0;
	} else if (numEnemies == 4) {
		if (collision1 || collision2 || collision3 || collision4) playerDead = 1;
		else playerDead = 0;
	} else {
		if (collision1 || collision2 || collision3 || collision4 || collision5) playerDead = 1;
		else playerDead = 0;
	}
}

void coin_pickup(void) {
	coin_collision = check_collision(&GoodGuy1, &coin);

	if (coin_collision) {
		// If player is in left side of screen
		if (GoodGuy1.x < 100) {
			// If player is on top half of screen
				if (GoodGuy1.y < 100) {
					coin.x = 50;
					coin.y = 175;
				}
			// Else bottom half of screen
				else {
					coin.x = 200;
					coin.y = 175;
				}
		} 
		
		// Else player is on the right side of screen
		else {
			// If player is on top half of screen
			if (GoodGuy1.y < 100) {
				coin.x = 50;
				coin.y = 40;
			}
			// Else bottom half of screen
			else {
				coin.x = 200;
				coin.y = 40;
			}
		}

		// If we are not at max enemies
		if (numEnemies != maxEnemies) {
			// Add enemies
			numEnemies++;
		}
		
	} else {
		// Do nothing, keep coin at the same spot
		pal_col(0,0x0f);
	}
}

void main (void) {

	while(1) {
	
		ppu_off(); // screen off
		
		// load the palettes
		pal_bg(palTitle);
		pal_spr(palTitle);

		// use the second set of tiles for sprites
		// both bg and sprite are set to 0 by default
		bank_spr(1);
		set_vram_buffer();
	
		song = 0;
		music_play(song);
		
		// turn on screen
		ppu_on_all(); 
		//music_play(song);

		
		//load title screen
		fade_in();
		show_title();
		fade_out();
		
		
		//when show_title breaks, load level
		draw_bg();
		fade_in();
		music_play(song+1);

		// Initialize position of player, 1st enemy, and 1st coin
		GoodGuy1.x = 128;
		GoodGuy1.y = 64;
		enemyList[0].x = 128;
		enemyList[0].y = 168;
		for(i = 1; i < maxEnemies; i++) {
			enemyList[i].x = 120;
			enemyList[i].y = 120;
		}
		coin.x = 100;
		coin.y = 100;


		// Initialize all enemy velocities to 0
		for(i = 0; i < maxEnemies; i++) {
			enemy_dx[i] = 0;
			enemy_dy[i] = 0;
		}

		// Loop until player dies
		
		while (playerDead == 0){
			ppu_wait_nmi(); // wait till beginning of the frame
			// the sprites are pushed from a buffer to the OAM during nmi
			set_music_speed(8);
			// clear all sprites from sprite buffer
			oam_clear();

			//set everything up
			pad1 = pad_poll(0);
			draw_sprites();
			movement();
			coin_pickup();
			test_collision();
			}	

			ppu_wait_frame();

			// Once player dies, go back to title screen
			fade_out();

			playerDead = 0;

			// Turn off Screen
			ppu_off();

			// Clear the sprites
			oam_clear();

			// Set numEnemies back to 1
			numEnemies = 1;

			// Fade out of screen
			fade_out();
		}
	}