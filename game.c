#include "LIB/neslib.h"
#include "LIB/nesdoug.h"
#include "TILESETS/beep.h"
#include "Sprites.h"
#include "game.h"

static unsigned char wait;
static unsigned char frame_cnt;
static int iy,dy;


const unsigned char palTitle[]={ 0x0f,0x03,0x15,0x30,0x0f,0x01,0x21,0x31,0x0f,0x06,0x30,0x26,0x0f,0x09,0x19,0x29 };

// Function to fade out screen
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

//draw the player and enemy sprite
void draw_sprites(void) {
	// clear all sprites from sprite buffer
	oam_clear();
	
	// draw player metasprite
	// WOULD LIKE TO EDIT THIS LATER, BY CHANING THE PLAYER SPRITE SO THAT IT LOOKS LEFT WHEN HE MOVES LEFT, AND RIGHT WHEN HE MOVES RIGHT
	// LIKE HAVE sprPlayerLeft and sprPlayerRight
	if (direction == LEFT)
		oam_meta_spr(high_byte(PlayerGuy.x), high_byte(PlayerGuy.y), sprPlayer);
	else
		oam_meta_spr(high_byte(PlayerGuy.x), high_byte(PlayerGuy.y), sprPlayer);
}

// Shows title screen
void show_title() {
	song = 0;
  // Disable Rendering
  ppu_off();
  
  // Unpack nametable into VRAM
  vram_adr(NAMETABLE_A);
  vram_unrle(beep);
  music_play(song);

  
  // Enable Rendering
  ppu_on_all();
  
iy=220<<FP_BITS;


  while(1) {
    ppu_wait_frame();
    
    if (pad_trigger(0)&PAD_START) 
	{ 
	
		break;}


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

//loads the room
void load_room(void) {
	set_data_pointer(Rooms[0]);
	set_mt_pointer(metatiles1);
	for(y=0; ;y+=0x20){
		for(x=0; ;x+=0x20){
			address = get_ppu_addr(0, x, y);
			index = (y & 0xf0) + (x >> 4);
			buffer_4_mt(address, index); // ppu_address, index to the data
			flush_vram_update2();
			if (x == 0xe0) break;
		}
		if (y == 0xe0) break;
	}
	
	// a little bit in the next room
	set_data_pointer(Rooms[1]);
	for(y=0; ;y+=0x20){
		x = 0;
		address = get_ppu_addr(1, x, y);
		index = (y & 0xf0);
		buffer_4_mt(address, index); // ppu_address, index to the data
		flush_vram_update2();
		if (y == 0xe0) break;
	}
	
	// copy the room to the collision map
	// the second one should auto-load with the scrolling code
	memcpy (c_map, room1, 240);
}

void movement(void) {
	
	// handle x
	old_x = PlayerGuy.x;
	
	if(pad1 & PAD_LEFT){
		direction = LEFT;
		if(PlayerGuy.x <= 0x100) {
			PlayerGuy.vel_x = 0;
			PlayerGuy.x = 0x100;
		}
		else if(PlayerGuy.x < 0x400) { // don't want to wrap around to the other side
			PlayerGuy.vel_x = -0x100;
		}
		else {
			PlayerGuy.vel_x = -SPEED;
		}
	}

	else if (pad1 & PAD_RIGHT){
		
		direction = RIGHT;

		PlayerGuy.vel_x = SPEED;
	}
	
	else { // nothing pressed
		PlayerGuy.vel_x = 0;
	}
	
	PlayerGuy.x += PlayerGuy.vel_x;
	
	if((PlayerGuy.x < 0x100)||(PlayerGuy.x > 0xf800)) { // make sure no wrap around to the other side
		PlayerGuy.x = 0x100;
	} 
	
	L_R_switch = 1; // shinks the y values in bg_coll, less problems with head / feet collisions
	
	Generic.x = high_byte(PlayerGuy.x); // this is much faster than passing a pointer to PlayerGuy
	Generic.y = high_byte(PlayerGuy.y);

	Generic.width = HERO_WIDTH;
	Generic.height = HERO_HEIGHT;

	bg_collision();

	if(collision_R && collision_L){ // if both true, probably half stuck in a wall
		PlayerGuy.x = old_x;
	}
	
	else if(collision_L) {
		high_byte(PlayerGuy.x) = high_byte(PlayerGuy.x) - eject_L;
		
	}
	
	else if(collision_R) {
		high_byte(PlayerGuy.x) = high_byte(PlayerGuy.x) - eject_R;
	} 

	// handle y
	if(PlayerGuy.vel_y < 0x300){
		PlayerGuy.vel_y += GRAVITY;
	}
	else{
		PlayerGuy.vel_y = 0x300; 
	}
	PlayerGuy.y += PlayerGuy.vel_y;
	
	L_R_switch = 0;
	Generic.x = high_byte(PlayerGuy.x); 
	Generic.y = high_byte(PlayerGuy.y);
	bg_collision();
	
	if(collision_U) {
		high_byte(PlayerGuy.y) = high_byte(PlayerGuy.y) - eject_U;
		PlayerGuy.vel_y = 0;
	}
	else if(collision_D) {
		high_byte(PlayerGuy.y) = high_byte(PlayerGuy.y) - eject_D;
		PlayerGuy.y &= 0xff00;
		if(PlayerGuy.vel_y > 0) {
			PlayerGuy.vel_y = 0;
		}
	}
	//jumping section
	Generic.y = high_byte(PlayerGuy.y);

		if(pad1 & PAD_A) {
			PlayerGuy.vel_y = JUMP_VEL;
		}
		
	
	// do we need to load a new collision map? (scrolled into a new room)
	if((scroll_x & 0xff) < 4){
		new_cmap();
	}
	
	// do we need to load a new collision map? (scrolled into a new room)
	if((scroll_x & 0xff) < 4){
		new_cmap(); //
	}
	
	// scroll
	temp5 = PlayerGuy.x;
	if (PlayerGuy.x > MAX_RIGHT){
		temp1 = (PlayerGuy.x - MAX_RIGHT) >> 8;
		scroll_x += temp1;
		high_byte(PlayerGuy.x) = high_byte(PlayerGuy.x) - temp1;
	}

	if(scroll_x >= MAX_SCROLL) {
		scroll_x = MAX_SCROLL; // stop scrolling right, end of level
		PlayerGuy.x = temp5; // but allow the x position to go all the way right
		if(high_byte(PlayerGuy.x) >= 0xf1) {
			PlayerGuy.x = 0xf100;
		}
	}
}	

void draw_screen_R(void){
	// scrolling to the right, draw metatiles as we go
	pseudo_scroll_x = scroll_x + 0x120;
	
	temp1 = pseudo_scroll_x >> 8;
	
	set_data_pointer(Rooms[temp1]);
	nt = temp1 & 1;
	x = pseudo_scroll_x & 0xff;
	
	// important that the main loop clears the vram_buffer
	
	switch(scroll_count){
		case 0:
			address = get_ppu_addr(nt, x, 0);
			index = 0 + (x >> 4);
			buffer_4_mt(address, index); // ppu_address, index to the data
			
			address = get_ppu_addr(nt, x, 0x20);
			index = 0x20 + (x >> 4);
			buffer_4_mt(address, index); // ppu_address, index to the data
			break;
			
		case 1:
			address = get_ppu_addr(nt, x, 0x40);
			index = 0x40 + (x >> 4);
			buffer_4_mt(address, index); // ppu_address, index to the data
			
			address = get_ppu_addr(nt, x, 0x60);
			index = 0x60 + (x >> 4);
			buffer_4_mt(address, index); // ppu_address, index to the data
			break;
			
		case 2:
			address = get_ppu_addr(nt, x, 0x80);
			index = 0x80 + (x >> 4);
			buffer_4_mt(address, index); // ppu_address, index to the data
			
			address = get_ppu_addr(nt, x, 0xa0);
			index = 0xa0 + (x >> 4);
			buffer_4_mt(address, index); // ppu_address, index to the data
			break;
			
		default:
			address = get_ppu_addr(nt, x, 0xc0);
			index = 0xc0 + (x >> 4);
			buffer_4_mt(address, index); // ppu_address, index to the data
			
			address = get_ppu_addr(nt, x, 0xe0);
			index = 0xe0 + (x >> 4);
			buffer_4_mt(address, index); // ppu_address, index to the data
	}

	++scroll_count;
	scroll_count &= 3; // mask off top bits, keep it 0-3
}

void bg_collision_sub(void){
	coordinates = (temp1 >> 4) + (temp3 & 0xf0);
	
	map = temp2&1; // high byte

	if(!map){
		collision = c_map[coordinates];
	}

	else{
		collision = c_map2[coordinates];
	}
}

void bg_collision(void) {
	// note, !0 = collision
	// sprite collision with backgrounds
	// load the object's x,y,width,height to Generic, then call this
	collision_L = 0;
	collision_R = 0;
	collision_U = 0;
	collision_D = 0;
	
	if(Generic.y >= 0xf0) return;
	
	temp6 = temp5 = Generic.x + scroll_x; // upper left (temp6 = save for reuse)
	temp1 = temp5 & 0xff; // low byte x
	temp2 = temp5 >> 8; // high byte x
	
	eject_L = temp1 | 0xf0;
	
	temp3 = Generic.y; // y top
	
	eject_U = temp3 | 0xf0;
	
	if(L_R_switch) temp3 += 2; // fix bug, walking through walls
	
	bg_collision_sub();
	
	if(collision){ // find a corner in the collision map
		++collision_L;
		++collision_U;
	}
	
	// upper right
	temp5 += Generic.width;
	temp1 = temp5 & 0xff; // low byte x
	temp2 = temp5 >> 8; // high byte x
	
	eject_R = (temp1 + 1) & 0x0f;
	
	// temp3 is unchanged
	bg_collision_sub();
	
	if(collision){ // find a corner in the collision map
		++collision_R;
		++collision_U;
	}
	
	
	// again, lower
	
	// bottom right, x hasn't changed

	temp3 = Generic.y + Generic.height; //y bottom
	if(L_R_switch) temp3 -= 2; // fix bug, walking through walls
	eject_D = (temp3 + 1) & 0x0f;
	if(temp3 >= 0xf0) return;
	
	bg_collision_sub();
	
	if(collision){ // find a corner in the collision map
		++collision_R;
		++collision_D;
	}
	
	// bottom left
	temp1 = temp6 & 0xff; // low byte x
	temp2 = temp6 >> 8; // high byte x
	
	//temp3, y is unchanged

	bg_collision_sub();
	
	if(collision){ // find a corner in the collision map
		++collision_L;
		++collision_D;
	}
}

// copy a new collision map to one of the 2 c_map arrays
void new_cmap(void){
	// copy a new collision map to one of the 2 c_map arrays
	room = ((scroll_x >> 8) +1); //high byte = room, one to the right
	
	map = room & 1; //even or odd?
	if(!map){
		memcpy (c_map, Rooms[room], 240);
	}
	else{
		memcpy (c_map2, Rooms[room], 240);
	}
}

void main (void) {
	
	 // load the pallets
	 // use the second set of tiles for sprites
	// both bg and sprite are set to 0 by default
	pal_bg(palTitle);
	pal_spr(palTitle);
	
	bank_spr(1);
	set_vram_buffer();
	
	//show title;
	show_title();
	//turn on
	ppu_on_all();
	
	//fade out and turn off
	fade_out();
	ppu_off();
	fade_in();
	
	bank_spr(1);
	set_vram_buffer();

	// Load the level
	load_room();

	// Shift thigs one pixel down
	scroll_y = 0xff;
	set_scroll_y(scroll_y); // shift the bg down 1 pixel

	// Turn screen on again
	
	ppu_on_all();
	

	// Player level music
	music_play(song+1);

	while (1) 
	{
		// Wait till beginning of the frame
		ppu_wait_nmi();

		// the sprites are pushed from a buffer to the OAM during nmi
		set_music_speed(8);

		// Read first controller
		pad1 = pad_poll(0);


		movement();
		// set scroll
		set_scroll_x(scroll_x);
		set_scroll_y(scroll_y);
		draw_screen_R();
		draw_sprites();
	}	
}