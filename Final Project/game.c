#include "LIB/neslib.h"
#include "LIB/nesdoug.h"
#include "TILESETS/beep.h"
#include "Sprites.h"


#pragma bss-name(push, "ZEROPAGE")

unsigned char pad2;


#pragma bss-name(push, "BSS")

struct Assets {
	unsigned char x;
	unsigned char y;
	unsigned char width;
	unsigned char height;
};

struct Assets GoodGuy1 = {128,64,15,15};
struct Assets BadGuy1 = {128,168,15,15};
unsigned int collided = 0;




const unsigned char palTitle[]={ 0x0f,0x03,0x15,0x30,0x0f,0x01,0x21,0x31,0x0f,0x06,0x30,0x26,0x0f,0x09,0x19,0x29 };
const unsigned char palLevel[]={
0x0f, 0x00, 0x10, 0x30, // black, gray, lt gray, white
0,0,0,0,
0,0,0,0,
0,0,0,0
}; 


const unsigned char palette_sp[]={
0x0f, 0x0f, 0x0f, 0x28, // black, black, yellow
0x0f, 0x0f, 0x0f, 0x12, // black, black, blue
0,0,0,0,
0,0,0,0
};

// Shows title screen
void show_title() {
  // Disable Rendering
  ppu_off();

  // Set pallete to our title screen pallete
  pal_bg(palTitle);
  
  // Unpack nametable into VRAM
  vram_adr(NAMETABLE_A);
  vram_unrle(beep);
  
  // Enable Rendering
  ppu_on_all();
  
  while(1) {
    ppu_wait_frame();
    
    if (pad_trigger(0)&PAD_START) break;
  }

}

void draw_bg(void){
	ppu_off(); // screen off
	pal_bg(palLevel);
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
	
	// draw 2 metasprites
	oam_meta_spr(GoodGuy1.x, GoodGuy1.y, sprPlayer);
	
	oam_meta_spr(BadGuy1.x, BadGuy1.y, sprGhost);
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
	collision = check_collision(&GoodGuy1, &BadGuy1);
		
	// change the BG color, if sprites are touching
	if (collision){
	pal_col(0,0x30);
	}
	else{
	pal_col(0,0x0f);
	}

}

void main (void) {
	
	ppu_off(); // screen off
	
	// load the palettes
	pal_bg(palTitle);
	pal_spr(palette_sp);

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
	show_title();
	
	//when show_title breaks, load level
	draw_bg();
	music_play(song+1);

	//infinite loop
	
	while (1){
		ppu_wait_nmi(); // wait till beginning of the frame
		// the sprites are pushed from a buffer to the OAM during nmi
		set_music_speed(8);
		// clear all sprites from sprite buffer
		oam_clear();

		//set everything up
		pad1 = pad_poll(0);
		draw_sprites();
		movement();
		test_collision();
		}		
	}

	
	