#include "LIB/neslib.h"
#include "LIB/nesdoug.h"
#include "SCREENS/title_screen.h"
#include "Sprites.h"
#include "game.h"
#include "level_data.c"

static unsigned char wait;
static unsigned char frame_cnt;
static int iy,dy;


const unsigned char palTitle[]={ 0x0f,0x03,0x15,0x30,0x0f,0x01,0x21,0x31,0x0f,0x06,0x30,0x26,0x0f,0x09,0x19,0x29 };
const unsigned char palLevel[16]={ 0x0f,0x00,0x10,0x30,0x0f,0x30,0x11,0x16,0x0f,0x30,0x11,0x16,0x0f,0x17,0x19,0x29 };
const unsigned char palSprite[16]={ 0x0f,0x00,0x10,0x30,0x0f,0x01,0x21,0x31,0x0f,0x37,0x15,0x12,0x0f,0x18,0x28,0x38 };


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

	temp_x = high_byte(PlayerGuy.x);
	if(temp_x > 0xfc) temp_x = 1;
	if(temp_x == 0) temp_x = 1;
	
	// draw player metasprite
	if (direction == LEFT)
	{
		if (running) {
			if (run_anim == 0) {
				oam_meta_spr(temp_x, high_byte(PlayerGuy.y), sprPlayerWalkLeft1);
				run_anim = 1;
			} else {
				oam_meta_spr(temp_x, high_byte(PlayerGuy.y), sprPlayerWalkLeft2);
				run_anim = 0;
			}
		} else {
			oam_meta_spr(temp_x, high_byte(PlayerGuy.y), sprPlayerStandLeft);
		}
	}

	else
	{
		if (running) {
			if (run_anim == 0) {
				oam_meta_spr(temp_x, high_byte(PlayerGuy.y), sprPlayerWalkRight1);
				run_anim = 1;
			} else {
				oam_meta_spr(temp_x, high_byte(PlayerGuy.y), sprPlayerWalkRight2);	
				run_anim = 0;
			}
		} else {
			oam_meta_spr(temp_x, high_byte(PlayerGuy.y), sprPlayerStandRight);
		}
	}


	// Draw Coins
	for(index = 0; index < MAX_COINS; ++index){ 
		temp_y = coin_y[index];

		if(temp_y == TURN_OFF) continue;

		if(get_frame_count() & 8) {
			++temp_y; // bounce the coin
		}
		if(!coin_active[index]) continue;

		temp_x = coin_x[index];

		if(temp_x > 0xf0) continue;

		if(temp_y < 0xf0) {
			oam_meta_spr(temp_x, temp_y, sprCoin);
		}
	}
	
	
	// Draw Enemies
	offset = get_frame_count() & 3;
	offset = offset << 4; // * 16, the size of the shuffle array

	for(index = 0; index < MAX_ENEMY; ++index) {
		index2 = shuffle_array[offset];
		++offset;

		temp_y = enemy_y[index];

		if(temp_y == TURN_OFF) continue;
		if(!enemy_active[index]) continue;

		temp_x = enemy_x[index];

		if(temp_x == 0) continue;
		if(temp_x > 0xf0) continue;

		if(temp_y < 0xf0) {
			if (high_byte(PlayerGuy.x) > temp_x)
				oam_meta_spr(temp_x, temp_y, sprEnemyRight);
			else
				oam_meta_spr(temp_x, temp_y, sprEnemyLeft);
		}
	}


	// Draw Stars
	for(index = 0; index < MAX_STARS; ++index) {
		temp_y = star_y[index];

		if(temp_y == TURN_OFF) continue;
		if (!star_active[index]) continue;

		temp_x = star_x[index];

		if(temp_x > 0xf0) continue;

		if(temp_y < 0xf0) {
			oam_meta_spr(temp_x, temp_y, sprStar);
		}
	}
	
	
	// Draw "coins" at the top of screen
	oam_meta_spr(16,8, sprCoinsScore);

	// Amount of coins
	temp1 = (coins / 10) + 0x10;
	temp2 = (coins % 10) + 0x10;
	oam_spr(64,8,temp1,3);
	oam_spr(72,8,temp2,3);

	// Draw "lives" at the top of screen
	oam_meta_spr(16,20, sprLivesScore);

	// Amount of lives
	temp1 = lives + 0x10;
	oam_spr(64,20,temp1,3);
}

// Shows title screen
void show_title() {
	// Set the song
	song = 0;

	// Set the game mode to title
	game_mode = MODE_TITLE;

	// Disable Rendering
	ppu_off();
	
	// Unpack nametable into VRAM
	vram_adr(NAMETABLE_A);
	vram_unrle(title_screen);
	music_play(song);
}

//loads the room
void load_room(void) { 
	clear_vram_buffer();
	offset = Level_offsets[level];

	set_data_pointer(Levels_list[offset]);
	set_mt_pointer(metatiles);
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

	++offset;
	
	// a little bit in the next room
	set_data_pointer(Levels_list[offset]);
	for(y=0; ;y+=0x20){
		x = 0;
		address = get_ppu_addr(1, x, y);
		index = (y & 0xf0);
		buffer_4_mt(address, index); // ppu_address, index to the data
		flush_vram_update2();
		if (y == 0xe0) break;
	}

	--offset;
	
	// copy the room to the collision map
	// the second one should auto-load with the scrolling code
	memcpy (c_map, Levels_list[offset], 240);
	

	sprite_obj_init();
	PlayerGuy.x = 0x3000;
	PlayerGuy.y = 0xc400;
	PlayerGuy.vel_x = 0;
	PlayerGuy.vel_y = 0;

	map_loaded = 0;
}

void movement(void) {
	
	// handle x
	old_x = PlayerGuy.x;
	
	if(pad1 & PAD_LEFT){
		direction = LEFT;
		running = 1;
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
		running = 1;
		PlayerGuy.vel_x = SPEED;
	}
	
	else { // nothing pressed
		PlayerGuy.vel_x = 0;
		running = 0;
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

	// check collision down a little lower than hero
	Generic.y = high_byte(PlayerGuy.y); // the rest should be the same
	bg_check_low();
	if(collision_D) {
		if(pad1_new & PAD_A) {
			PlayerGuy.vel_y = JUMP_VEL; // JUMP
			sfx_play(SFX_JUMP, 0);
			short_jump_count = 1;
		}
	}


	// Allows shorter jumps
	if(short_jump_count){
		if (collision_D) {
			old_jump_x = PlayerGuy.x;
			old_jump_y = PlayerGuy.y;
		}

		++short_jump_count;
		if(short_jump_count > 30) short_jump_count = 0;
	}

	if((short_jump_count) && ((pad1 & PAD_A) == 0) && (PlayerGuy.vel_y < -0x200)){
		if (collision_D) {
			old_jump_x = PlayerGuy.x;
			old_jump_y = PlayerGuy.y;
		}

		PlayerGuy.vel_y = -0x200;
		short_jump_count = 0;
	}
		
	
	// do we need to load a new collision map? (scrolled into a new room)
	if((scroll_x & 0xff) < 4){
		if (!map_loaded) {
			new_cmap();
			map_loaded = 1;
		}
	} else {
		map_loaded = 0;
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

void enemy_moves(void) {

	//for bg collisions
	Generic.x = enemy_x[index];
	Generic.y = enemy_y[index] + 6; // mid point
	Generic.width = 13;
	

	if(enemy_active[index]) {
		if(enemy_x[index] > Generic2.x) {
			Generic.x -= 1;
			bg_collision_fast();
			if(collision_L) return;
			if(enemy_actual_x[index] == 0) --enemy_room[index];
			--enemy_actual_x[index];
		}

		else if(enemy_x[index] < Generic2.x){
			Generic.x += 1;
			bg_collision_fast();
			if(collision_R) return;
			++enemy_actual_x[index];
			if(enemy_actual_x[index] == 0) ++enemy_room[index];
		}
	}
	
}

void draw_screen_R(void){
	// scrolling to the right, draw metatiles as we go
	pseudo_scroll_x = scroll_x + 0x120;
	
	temp1 = pseudo_scroll_x >> 8;

	offset = Level_offsets[level];
	offset += temp1;
	
	set_data_pointer(Levels_list[offset]);
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

	collision = is_solid[collision];
}

void bg_collision_fast(void){
	// rewrote this for enemies, bg_collision was too slow
	collision_L = 0;
	collision_R = 0;
	
	if(Generic.y >= 0xf0) return;
	
	temp6 = temp5 = Generic.x + scroll_x; // upper left (temp6 = save for reuse)
	temp1 = temp5 & 0xff; // low byte x
	temp2 = temp5 >> 8; // high byte x
	
	temp3 = Generic.y; // y top
	
	bg_collision_sub();
	
	if(collision & COL_ALL){ // find a corner in the collision map
		++collision_L;
	}
	
	// upper right
	temp5 += Generic.width;
	temp1 = temp5 & 0xff; // low byte x
	temp2 = temp5 >> 8; // high byte x
	
	// temp3 is unchanged
	bg_collision_sub();
	
	if(collision & COL_ALL){ // find a corner in the collision map
		++collision_R;
	}
}

void bg_check_low(void) {
	// floor collisions
	collision_D = 0;
	
	temp5 = Generic.x + scroll_x;    //left
	temp1 = temp5 & 0xff; //low byte
	temp2 = temp5 >> 8; //high byte
	
	temp3 = Generic.y + Generic.height + 1; // bottom
	
	if(temp3 >= 0xf0) return;
	
	bg_collision_sub();
	
	if(collision & (COL_DOWN|COL_ALL)){ // find a corner in the collision map
		++collision_D;
	}
	
	
	//temp5 = right
	temp5 += Generic.width;
	temp1 = temp5 & 0xff; //low byte
	temp2 = temp5 >> 8; //high byte
	
	//temp3 is unchanged
	bg_collision_sub();
	
	if(collision & (COL_DOWN|COL_ALL)){ // find a corner in the collision map
		++collision_D;
	}
	
	if((temp3 & 0x0f) > 3) collision_D = 0; // for platforms, only collide with the top 3 pixels
}

void bg_collision(void) {
	// note, uses bits in the metatile data to determine collision
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
	
	if(collision & COL_ALL){ // find a corner in the collision map
		++collision_L;
		++collision_U;
	}

	if(collision & COL_DEATH) {
		sfx_play(SFX_NOISE, 0);
		++death;
	}
	
	// upper right
	temp5 += Generic.width;
	temp1 = temp5 & 0xff; // low byte x
	temp2 = temp5 >> 8; // high byte x
	
	eject_R = (temp1 + 1) & 0x0f;
	
	// temp3 is unchanged
	bg_collision_sub();
	
	if(collision & COL_ALL){ // find a corner in the collision map
		++collision_R;
		++collision_U;
	}
	
	if(collision & COL_DEATH) {
		sfx_play(SFX_NOISE, 0);
		++death;
	}
	
	// again, lower
	
	// bottom right, x hasn't changed
	
	temp3 = Generic.y + Generic.height; //y bottom
	if(L_R_switch) temp3 -= 2; // fix bug, walking through walls
	eject_D = (temp3 + 1) & 0x0f;
	if(temp3 >= 0xf0) return;
	
	bg_collision_sub();
	
	if(collision & COL_ALL){ // find a corner in the collision map
		++collision_R;
	}
	if(collision & (COL_DOWN|COL_ALL)){ // find a corner in the collision map
		++collision_D;
	}

	if(collision & COL_DEATH) {
		sfx_play(SFX_NOISE, 0);
		++death;
	}
	
	// bottom left
	temp1 = temp6 & 0xff; // low byte x
	temp2 = temp6 >> 8; // high byte x
	
	//temp3, y is unchanged

	bg_collision_sub();
	
	if(collision & COL_ALL){ // find a corner in the collision map
		++collision_L;
	}
	if(collision & (COL_DOWN|COL_ALL)){ // find a corner in the collision map
		++collision_D;
	}

	if(collision & COL_DEATH) {
		sfx_play(SFX_NOISE, 0);
		++death;
	}

	if((temp3 & 0x0f) > 3) collision_D = 0; // for platforms, only collide with the top 3 pixels
}

// copy a new collision map to one of the 2 c_map arrays
void new_cmap(void){
	// copy a new collision map to one of the 2 c_map arrays
	room = ((scroll_x >> 8) +1); //high byte = room, one to the right
	offset = Level_offsets[level];
	offset += room;
	
	map = room & 1; //even or odd?
	if(!map){
		memcpy (c_map, Levels_list[offset], 240);
	}
	else{
		memcpy (c_map2, Levels_list[offset], 240);
	}
}

void sprite_collisions(void) {

	Generic.x = high_byte(PlayerGuy.x);
	Generic.y = high_byte(PlayerGuy.y);
	Generic.width = HERO_WIDTH;
	Generic.height = HERO_HEIGHT;
	
	Generic2.width = COIN_WIDTH;
	Generic2.height = COIN_HEIGHT;
	
	for(index = 0; index < MAX_COINS; ++index){
		if(coin_active[index]){
			Generic2.x = coin_x[index];
			Generic2.y = coin_y[index];
			if(check_collision(&Generic, &Generic2)) {
				coin_y[index] = TURN_OFF;
				sfx_play(SFX_COIN, 0);
				++coins;
			}
		}
	}

	Generic2.width = STAR_WIDTH;
	Generic2.height = STAR_HEIGHT;

	for(index = 0; index < MAX_STARS; ++index){
		if(star_active[index]){
			Generic2.x = star_x[index];
			Generic2.y = star_y[index];
			if(check_collision(&Generic, &Generic2)) {
				star_y[index] = TURN_OFF;
				sfx_play(SFX_STAR, 0);
				++level_up;
			}
		}
	}

	Generic2.width = ENEMY_WIDTH;
	Generic2.height = ENEMY_HEIGHT;
	
	for(index = 0; index < MAX_ENEMY; ++index){
		if(enemy_active[index]){
			Generic2.x = enemy_x[index];
			Generic2.y = enemy_y[index];
			if(check_collision(&Generic, &Generic2)){
				enemy_y[index] = TURN_OFF;
				sfx_play(SFX_NOISE, 0);
				if(coins) {
					coins = coins - 3;
					if (coins > 0x80) coins = 0;
				} 

				
				
				if (lives) {
					lives = lives - 1;
					if (lives > 0x80) lives = 0;
				} else {
					++death;
				}
			}
		}
	}
}

void check_spr_objects(void) {
	Generic2.x = high_byte(PlayerGuy.x);

	// mark each object "active" if they are, and get the screen x
	
	for(index = 0; index < MAX_COINS; ++index){
		coin_active[index] = 0; //default to zero
		if(coin_y[index] != TURN_OFF){
			high_byte(temp5) = coin_room[index];
			low_byte(temp5) = coin_actual_x[index];
			coin_active[index] = get_position();
			coin_x[index] = temp_x; // screen x coords
		}

	}


	for(index = 0; index < MAX_STARS; ++index){
		star_active[index] = 0; //default to zero
		if(star_y[index] != TURN_OFF){
			high_byte(temp5) = star_room[index];
			low_byte(temp5) = star_actual_x[index];
			star_active[index] = get_position();
			star_x[index] = temp_x; // screen x coords
		}

	}
	

	for(index = 0; index < MAX_ENEMY; ++index){
		enemy_active[index] = 0; //default to zero
		if(enemy_y[index] != TURN_OFF){
			high_byte(temp5) = enemy_room[index];
			low_byte(temp5) = enemy_actual_x[index];
			temp1 = enemy_active[index] = get_position();
			if(temp1 == 0) continue;
			enemy_x[index] = temp_x; // screen x coords
			
			enemy_moves(); // if active, do it's moves now
		}

	}
}

char get_position(void){
	// is it in range ? return 1 if yes
	
	temp5 -= scroll_x;
	temp_x = temp5 & 0xff;
	if(high_byte(temp5)) return 0;
	return 1;
}

void sprite_obj_init(void){

	pointer = Coins_list[level];
	for(index = 0,index2 = 0;index < MAX_COINS; ++index){
		
		coin_x[index] = 0;

		temp1 = pointer[index2]; // get a byte of data
		coin_y[index] = temp1;
		
		if(temp1 == TURN_OFF) break;

		++index2;
		
		coin_active[index] = 0;

		
		temp1 = pointer[index2]; // get a byte of data
		coin_room[index] = temp1;
		
		++index2;
		
		temp1 = pointer[index2]; // get a byte of data
		coin_actual_x[index] = temp1;
		
		++index2;
	}
	
	for(++index;index < MAX_COINS; ++index){
		coin_y[index] = TURN_OFF;
	}


	pointer = Stars_list[level];
	for(index = 0,index2 = 0;index < MAX_STARS; ++index){
		
		star_x[index] = 0;

		temp1 = pointer[index2]; // get a byte of data
		star_y[index] = temp1;
		
		if(temp1 == TURN_OFF) break;

		++index2;
		
		star_active[index] = 0;

		
		temp1 = pointer[index2]; // get a byte of data
		star_room[index] = temp1;
		
		++index2;
		
		temp1 = pointer[index2]; // get a byte of data
		star_actual_x[index] = temp1;
		
		++index2;
	}
	
	for(++index;index < MAX_STARS; ++index){
		star_y[index] = TURN_OFF;
	}
	
	pointer = Enemy_list[level];
	for(index = 0,index2 = 0;index < MAX_ENEMY; ++index){
		
		enemy_x[index] = 0;

		temp1 = pointer[index2]; // get a byte of data
		enemy_y[index] = temp1;
		
		if(temp1 == TURN_OFF) break;

		++index2;
		
		enemy_active[index] = 0;
		
		temp1 = pointer[index2]; // get a byte of data
		enemy_room[index] = temp1;
		
		++index2;
		
		temp1 = pointer[index2]; // get a byte of data
		enemy_actual_x[index] = temp1;
		
		++index2;
	}
	
	for(++index;index < MAX_ENEMY; ++index){
		enemy_y[index] = TURN_OFF;
	}
}

void main (void) {
	
	// load the pallets
	// use the second set of tiles for sprites
	// both bg and sprite are set to 0 by default
	pal_bg(palTitle);
	pal_spr(palSprite);
	bank_spr(1);

	set_vram_buffer();
	
	// Show title;
	show_title();

	// Turn on
	ppu_on_all();

	// Set scroll
	scroll_x = 0;
	set_scroll_x(scroll_x);

	// For title text flash
	iy = 220 << FP_BITS;

	while(1) {
		// When we enter the title screen
		while(game_mode == MODE_TITLE) {

			while(1) {
				ppu_wait_frame();

				// Read first controller
				pad1 = pad_poll(0);
				pad1_new = get_pad_new(0);
				
				// If the player presses start
				if (pad1_new & PAD_START) 
				{ 
					pal_fade_to(4, 0);
					ppu_off();
					pal_bg(palLevel);
					load_room();
					game_mode = MODE_GAME;
					music_play(song+1);
					scroll_x = 0;
					set_scroll_x(scroll_x);
					ppu_on_all();
					pal_bright(4);
					break;
				}


				iy+=dy;

				if(iy<0)
				{
					iy =0;
					dy=-dy>>1;
				}

				if(wait)
				{
					--wait;
				}

				else
				{
					pal_col(10,(frame_cnt&32)?0x0f:0x20);
					++frame_cnt;
				}
			}
		}

		// Player is alive
		while (game_mode == MODE_GAME) 
		{
			// Wait till beginning of the frame
			ppu_wait_nmi();

			// the sprites are pushed from a buffer to the OAM during nmi
			set_music_speed(8);

			// Read first controller
			pad1 = pad_poll(0);
			pad1_new = get_pad_new(0);

			// Do these things first before drawing to screen
			movement();
			check_spr_objects();
			sprite_collisions();

			// set scroll
			set_scroll_x(scroll_x);
			draw_screen_R();
			draw_sprites();

			// Go to next level or player death
			if (level_up) {
				game_mode = MODE_SWITCH;
				level_up = 0;
				bright = 4;
				bright_count = 0;
				++level;
			} else if (death) {
				death = 0;
				bright = 0;
				scroll_x = 0;
				set_scroll_x(scroll_x);
				ppu_off();

				delay(5);

				// Clear screen and go to game over
				oam_clear();
				game_mode = MODE_GAME_OVER;
				vram_adr(NAMETABLE_A);
				vram_fill(0,1024);
				ppu_on_all();
				pal_bright(4);
			}
		}

		// Switch levels
		while (game_mode == MODE_SWITCH) {
			ppu_wait_nmi();
			++bright_count;

			if(bright_count >= 0x10) { // fade out
				bright_count = 0;
				--bright;
				if(bright != 0xff) pal_bright(bright); // fade out
			}

			set_scroll_x(scroll_x);
			
			if(bright == 0xff) { // now switch rooms
				ppu_off();
				oam_clear();
				scroll_x = 0;
				set_scroll_x(scroll_x);

				if(level < 3) {
					load_room();
					game_mode = MODE_GAME;
					ppu_on_all();
					pal_bright(4); // back to normal brighness
				}

				else { // set end of game. Did we win?
					game_mode = MODE_END;
					vram_adr(NAMETABLE_A);
					vram_fill(0,1024);
					ppu_on_all();
					pal_bright(4);
				}
			}
		}

		// Player wins
		while (game_mode == MODE_END) {	
			ppu_wait_nmi();

			temp1 = (coins / 10) + 0x10;
			temp2 = (coins % 10) + 0x10;

			// Draw "you win" and coins score
			oam_meta_spr(90, 100, youText);
			oam_meta_spr(120, 100, winText);
			oam_meta_spr(85, 150, sprCoinsScore);
			oam_spr(131,150,temp1,3);
			oam_spr(139,150,temp2,3);

			set_scroll_x(0);

			music_stop();
		}

		// Player loses
		while (game_mode == MODE_GAME_OVER) {
			ppu_wait_nmi();

			// Draw "you lose"
			oam_meta_spr(90, 100, youText);
			oam_meta_spr(120, 100, loseText);	
			
			set_scroll_x(0);
			
			music_stop();
		}
	}
}