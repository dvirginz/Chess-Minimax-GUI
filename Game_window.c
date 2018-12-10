/*
 * Game_window.c
 *
 *  Created on: 9 Sep 2017
 *      Author: GalL
 */
#include "Game_window.h"
#include "Button.h"
#include <stdio.h>
#include <stdlib.h>
#include "XML.h"
#include "Tools.h"
#include <unistd.h>
#define GW_WIDGET_NUM 11
#define GW_WIDGET_DRAW 6
#define GW_CHESS_PIECES 13 // taking empty entry into consideartion
#define BLACK_RGB_R 	  205
#define BLACK_RGB_G 	  133
#define BLACK_RGB_B 	  63
#define WHITE_RGB_R 	  255
#define WHITE_RGB_G 	  255
#define WHITE_RGB_B       255
#define PIECE_SIZE_BMP    45
#define PIECE_SIZE_PX     47
#define UL_BOARD_CORNER_X 300
#define UL_BOARD_CORNER_Y 50
#define MAX_FILES 5
#define CATCHABLE_RGB_R   255
#define CATCHABLE_RGB_G   0
#define CATCHABLE_RGB_B   0
#define OPPONENT_RGB_R    0
#define OPPONENT_RGB_G    255
#define OPPONENT_RGB_B    0
#define NEUTRAL_RGB_R     0
#define NEUTRAL_RGB_G     0
#define NEUTRAL_RGB_B     255
#define CASTLE_RGB_R   60
#define CASTLE_RGB_G   60
#define CASTLE_RGB_B   60

//rectangle representing the board are of the window:
static SDL_Rect board_rect = {.x = UL_BOARD_CORNER_X,
		.y=UL_BOARD_CORNER_Y,.w = GAME_BOARD_SIZE*PIECE_SIZE_PX ,.h= GAME_BOARD_SIZE*PIECE_SIZE_PX};
static SDL_Rect board_rect_borders = {.x = UL_BOARD_CORNER_X -5 ,
		.y=UL_BOARD_CORNER_Y -5 ,.w = GAME_BOARD_SIZE*PIECE_SIZE_PX +10  ,.h= GAME_BOARD_SIZE*PIECE_SIZE_PX +10 }; // a slightly larger rect for the borders.
Widget** create_gw_widgets(SDL_Renderer* r){
	/** the following indices assigned are considered constants in the scope of this file.
	 * for code readability reasons they are not replaced with constants/enum values.
	 */
	Widget** res = (Widget**)malloc(GW_WIDGET_NUM*sizeof(Widget*));
	if(!res){return NULL;}
	SDL_Rect restart_loc = {.x= 70,.y=50,.w=180,.h=40};
	//restart,save and load are always active:
	res[0]  = create_button(r,restart_loc,"./img/restart_active.bmp","./img/restart_active.bmp",gw_cb_restart);
	SDL_Rect save_loc = {.x= 70,.y=150,.w=180,.h=40};
	res[1]  = create_button(r,save_loc,"./img/save_active.bmp","./img/save_active.bmp",gw_cb_save);
	SDL_Rect load_loc = {.x= 70,.y=250,.w=180,.h=40};
	res[2]  = create_button(r,load_loc,"./img/load_game_active.bmp","./img/load_game_active.bmp",gw_cb_load);
	SDL_Rect undo_loc = {.x= 70,.y=350,.w=180,.h=40};
	res[5]  = create_button(r,undo_loc,"./img/undo_active.bmp","./img/undo_inactive.bmp",gw_cb_undo);
	SDL_Rect main_loc = {.x= 70,.y=450,.w=180,.h=40};
	res[4]  = create_button(r,main_loc,"./img/main_active.bmp","./img/main_active.bmp",gw_cb_back_to_main);
	SDL_Rect exit_loc = {.x= 70,.y=550,.w=180,.h=40};
	res[3]  = create_button(r,exit_loc,"./img/exit.bmp","./img/exit.bmp",gw_cb_exit);
	SDL_Rect state_loc = {.x= 300,.y=550,.w=350,.h=60};
	res[6]  = create_button(r,state_loc,"./img/white_check.bmp","./img/white_check.bmp",NULL);
	res[7]  = create_button(r,state_loc,"./img/black_check.bmp","./img/black_check.bmp",NULL);
	res[8]  = create_button(r,state_loc,"./img/white_mate.bmp","./img/white_mate.bmp",NULL);
	res[9] 	= create_button(r,state_loc,"./img/black_mate.bmp","./img/black_mate.bmp",NULL);
	res[10] = create_button(r,state_loc,"./img/tie.bmp","./img/tie.bmp",NULL);
	for(int i = 0 ; i < GW_WIDGET_NUM ; i++){
		if(!res[i] ){
			//destroy all widgets
			destroy_widgets_list(res,GW_WIDGET_NUM);
			return NULL;
		}
	}
	return res;
}

GUI_MESSAGE update_gw_status(game_window* gw){
	if(!gw){return GUI_INVALID_ARGUMENT;}
	//setting the priority to HIGH in order to improve UX.
	//SDL_SetThreadPriority(SDL_THREAD_PRIORITY_HIGH);
	Widget** widgets = gw->widgets;
	//make all buttons stop handling events.
	for(int i = 0 ; i< gw->widget_num ; i++){
		(((Button*)widgets[i]->data))->is_active = false;
	}
	//clear the rendering target
	if(SDL_RenderClear(gw->renderer) != 0){return GUI_SDL_FAILURE;}
	//draw the buttons the buttons:
	GUI_MESSAGE msg = GUI_SUCCESS;
	for(int i=0 ; i < GW_WIDGET_DRAW -1;i++){
		msg = (((Button*)widgets[i]->data)->set_active(widgets[i],true) != GUI_SUCCESS) ? GUI_SDL_FAILURE : msg;
	}
	//undo is active iff board->curr_hist_len > 1* && board->user_mode == SINGLE_PLAYER_GAME
	// *  - this to prevent the rare case where undo is pressed before the computer made its move
	bool undo_active = (gw->board->curr_hist_len > 1 && gw->board->user_mode == SINGLE_PLAYER_GAME );
	msg = (((Button*)widgets[5]->data)->set_active(widgets[5],undo_active) != GUI_SUCCESS) ? GUI_SDL_FAILURE : msg;
	//update the grid
	msg = (update_gw_grid(gw) < 0 )? GUI_SDL_FAILURE : msg;
	update_game_state( gw); // upate state of game
	gw->render_needed = true;
	return msg;
}
Window* create_game_window(SDL_Window* window,SDL_Renderer* r,game_board* board){
	if(!window||!r){return NULL;}
	Window* res = (Window*)malloc(sizeof(Window));
	//alloc space for the dialog window  itself
	game_window* data = (game_window*)malloc(sizeof(game_window));
	//create the window.
	//SDL_Window* window = SDL_CreateWindow("Game",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,800,800,SDL_WINDOW_BORDERLESS);
	//SDL_Renderer* r = SDL_CreateRenderer(window,-1,SDL_RENDERER_TARGETTEXTURE || SDL_RENDERER_SOFTWARE);
	SDL_Texture** pieces_tx = create_gw_textures(r);
	Widget** widgets = create_gw_widgets(r);
	if(!res || !data || !widgets||!pieces_tx||!board){
		fprintf(stderr,"ERROR: Could not create game window. %s",SDL_GetError());
		free(data);
		free(res);
		destroy_game(board);
		destroy_widgets_list(widgets,GW_WIDGET_NUM);
		//SDL_DestroyWindow(window);
		//SDL_DestroyRenderer(r);
		free(pieces_tx);
		return NULL;
	}
	board->game_mode = GUI; // make sure it's set to GUI.
	data->board_locked = (game_state(board) != CHESS_STATUS_NORMAL);
	data->game_waiting = false;
	data->widget_num = GW_WIDGET_NUM;
	data->widgets = widgets;
	data->renderer = r;
	data->window = window;
	data->board = board;
	data->initial_player = board->curr_player;
	data->initial_p_type = board->p_type;
	data->pieces_tx = pieces_tx;
	data->save_needed = false;
	data->drag_drop_state = RELEASED;
	data->render_needed = true;
	res->data = data;
	res->destroy = destroy_gw;
	res->handle_event = handle_gw_event;
	res->draw = draw_gw;
	return res;
}
void handle_gw_event(Window* w  ,SDL_Event* e){
	if(!w||!e){return;}
	game_window* gw = (game_window*)(w->data);
	//if the board is waiting - events are not treated.
	if(gw->game_waiting){
		return;
	}
	//notice we are not clearing the rendering target. this is done only if necessary, by a call to update_gw_status from the event handlers.
	//each widget will handle the event
	for(int i = 0 ; i < gw->widget_num;i++){
		gw->widgets[i]->handle_event(gw->widgets[i],e,gw);
	}
	//update drag&drop:
	 update_drag(gw ,e);
	 //highlight moves feature. called only if necessary:
	 if(e->type == SDL_MOUSEBUTTONDOWN && e->button.button == SDL_BUTTON_RIGHT &&
			 is_point_valid_self(gw,e->button.x,e->button.y)){
		 update_gw_with_event_firing(gw);
		 highlight_moves(gw,e);
	 }
	//re-render so changes will take place.
	 if(gw->render_needed){
		 SDL_RenderPresent(gw->renderer);
		 gw->render_needed = false;
	 }
}
SDL_Texture* create_tx_from_bmp(SDL_Renderer* r,const char* image,Uint8 bg_r,Uint8 bg_g,Uint8 bg_b){
	if(!r || !image){
		return NULL;
	}
	SDL_Surface* surface = SDL_LoadBMP(image);
	//convert to desired RGB format.
	surface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_ARGB8888, 0);
	//clean the surface and apply background
	clean_surface_by_threshold(surface, bg_r, bg_g, bg_b);
	if(!surface){
		return NULL;
	}
	//the color of the background in our photos. we will use it as the colorkey for the background.
	//Uint32 color_key = SDL_MapRGB(surface->format, 100, 100, 100);
	//int colorkey_res = SDL_SetColorKey(surface, SDL_TRUE, color_key);
	SDL_Texture* tx  = SDL_CreateTextureFromSurface(r, surface);
	if(!surface || !tx){
		fprintf(stderr,"ERROR: could not create Textures. %s\n",SDL_GetError());
		SDL_FreeSurface(surface);
		SDL_DestroyTexture(tx);
		return NULL;
	}
	SDL_FreeSurface(surface);
	return tx;
}
SDL_Texture** create_gw_textures(SDL_Renderer* r){
	/** the following indices assigned are considered constants in the scope of this file.
	 * for code readability reasons they are not replaced with constants/enum values.
	 */
	//there is a redundant slot in the beginning, this is so the indices comply with our piece-coding convention.this is why we use CALLOC
	SDL_Texture** res = (SDL_Texture**)calloc(1,2*GW_CHESS_PIECES*sizeof(SDL_Texture*));
	if(!res|| !r){
		free(res);//NULL safe
		return NULL;
	}
	//FOR A PIECE WITH CODE n in {1...13} - the "white" bg is in 2n -1, black in 2n.
	res[1]  = create_tx_from_bmp(r,"./img/pieces/white_pawn.bmp"	,WHITE_RGB_R,WHITE_RGB_G,WHITE_RGB_B);//white
	res[2]  = create_tx_from_bmp(r,"./img/pieces/white_pawn.bmp"	,BLACK_RGB_R,BLACK_RGB_G,BLACK_RGB_B);//"black"
	res[3]  = create_tx_from_bmp(r,"./img/pieces/white_king.bmp"	,WHITE_RGB_R,WHITE_RGB_G,WHITE_RGB_B);
	res[4]  = create_tx_from_bmp(r,"./img/pieces/white_king.bmp"	,BLACK_RGB_R,BLACK_RGB_G,BLACK_RGB_B);
	res[5]  = create_tx_from_bmp(r,"./img/pieces/white_queen.bmp"	,WHITE_RGB_R,WHITE_RGB_G,WHITE_RGB_B);
	res[6]  = create_tx_from_bmp(r,"./img/pieces/white_queen.bmp"	,BLACK_RGB_R,BLACK_RGB_G,BLACK_RGB_B);
	res[7]  = create_tx_from_bmp(r,"./img/pieces/white_bishop.bmp"	,WHITE_RGB_R,WHITE_RGB_G,WHITE_RGB_B);
	res[8]  = create_tx_from_bmp(r,"./img/pieces/white_bishop.bmp"	,BLACK_RGB_R,BLACK_RGB_G,BLACK_RGB_B);
	res[9]  = create_tx_from_bmp(r,"./img/pieces/white_rook.bmp"	,WHITE_RGB_R,WHITE_RGB_G,WHITE_RGB_B);
	res[10] = create_tx_from_bmp(r,"./img/pieces/white_rook.bmp"	,BLACK_RGB_R,BLACK_RGB_G,BLACK_RGB_B);
	res[11] = create_tx_from_bmp(r,"./img/pieces/white_knight.bmp"	,WHITE_RGB_R,WHITE_RGB_G,WHITE_RGB_B);
	res[12] = create_tx_from_bmp(r,"./img/pieces/white_knight.bmp"	,BLACK_RGB_R,BLACK_RGB_G,BLACK_RGB_B);
	res[13] = create_tx_from_bmp(r,"./img/pieces/black_pawn.bmp"	,WHITE_RGB_R,WHITE_RGB_G,WHITE_RGB_B);
	res[14] = create_tx_from_bmp(r,"./img/pieces/black_pawn.bmp"	,BLACK_RGB_R,BLACK_RGB_G,BLACK_RGB_B);
	res[15] = create_tx_from_bmp(r,"./img/pieces/black_king.bmp"	,WHITE_RGB_R,WHITE_RGB_G,WHITE_RGB_B);
	res[16] = create_tx_from_bmp(r,"./img/pieces/black_king.bmp"	,BLACK_RGB_R,BLACK_RGB_G,BLACK_RGB_B);
	res[17] = create_tx_from_bmp(r,"./img/pieces/black_queen.bmp"	,WHITE_RGB_R,WHITE_RGB_G,WHITE_RGB_B);
	res[18] = create_tx_from_bmp(r,"./img/pieces/black_queen.bmp"	,BLACK_RGB_R,BLACK_RGB_G,BLACK_RGB_B);
	res[19] = create_tx_from_bmp(r,"./img/pieces/black_bishop.bmp"	,WHITE_RGB_R,WHITE_RGB_G,WHITE_RGB_B);
	res[20] = create_tx_from_bmp(r,"./img/pieces/black_bishop.bmp"	,BLACK_RGB_R,BLACK_RGB_G,BLACK_RGB_B);
	res[21] = create_tx_from_bmp(r,"./img/pieces/black_rook.bmp"	,WHITE_RGB_R,WHITE_RGB_G,WHITE_RGB_B);
	res[22] = create_tx_from_bmp(r,"./img/pieces/black_rook.bmp"	,BLACK_RGB_R,BLACK_RGB_G,BLACK_RGB_B);
	res[23] = create_tx_from_bmp(r,"./img/pieces/black_knight.bmp"	,WHITE_RGB_R,WHITE_RGB_G,WHITE_RGB_B);
	res[24] = create_tx_from_bmp(r,"./img/pieces/black_knight.bmp"	,BLACK_RGB_R,BLACK_RGB_G,BLACK_RGB_B);
	for(int i = 1 ; i <= GW_CHESS_PIECES ; i++){
		if(!res[i]){//clean all
			for(int j = 1 ; j <= GW_CHESS_PIECES ; j++){
				if(res[i]){
					SDL_DestroyTexture(res[i]); // SDL_DestroyTexture is NOT NULL SAFE.
				}
			}
			free(res);
			return NULL;
		}
	}
	return res;
}
GUI_MESSAGE draw_lines_on_square(SDL_Renderer* r,int row,int column,Sint32 R,Sint32 G,Sint32 B){
	if(!r){return GUI_INVALID_ARGUMENT;}
	int SPACING = 5;
	int start_x = UL_BOARD_CORNER_X + column * PIECE_SIZE_PX; // the x and y positions of the upper left corner of the corresponding sqaure in the board.
	int end_x = start_x + PIECE_SIZE_PX;
	int start_y = UL_BOARD_CORNER_Y + (GAME_BOARD_SIZE - row -1)*PIECE_SIZE_PX;
	int end_y = start_y + PIECE_SIZE_PX;
	int fail_num = SDL_SetRenderDrawColor(r,R,G,B,SDL_BLENDMODE_ADD);
	for(int k = start_x ; k < end_x ; k += SPACING){ // drawing VRETICAL lines
		fail_num += SDL_RenderDrawLine(r,k,start_y,k,end_y);
	}
	for(int k = start_y ; k < end_y ; k += SPACING){ // drawing HORIZONTAL lines
		fail_num += SDL_RenderDrawLine(r,start_x,k,end_x,k);
	}
	fail_num += SDL_SetRenderDrawColor(r,BACKGROUND_RGB_R,BACKGROUND_RGB_G,BACKGROUND_RGB_B,SDL_BLENDMODE_ADD);
	if(fail_num != 0){
		return GUI_SDL_FAILURE;
	}
	return GUI_SUCCESS;

}
GUI_MESSAGE draw_gw(Window* w ){
	if(!w){return GUI_INVALID_ARGUMENT;}
	game_window* gw = (game_window*)w->data;
	int fail_num = 0;
	//set the render draw color, clean the renderer, update status.
	fail_num += SDL_SetRenderDrawColor(gw->renderer,BACKGROUND_RGB_R,BACKGROUND_RGB_G,BACKGROUND_RGB_B,SDL_BLENDMODE_ADD);
	fail_num += SDL_RenderClear(gw->renderer);
	GUI_MESSAGE msg = update_gw_status(gw);
	if(gw->board->p_type == PC ){
		msg = (init_move(gw) == GUI_SUCCESS)?msg:GUI_FAILURE;
		msg = (update_gw_status(gw) == GUI_SUCCESS)?msg:GUI_FAILURE;
	}
	SDL_RenderPresent(gw->renderer);
	if(fail_num!=0 || msg != GUI_SUCCESS){
		fprintf(stderr,"ERROR: Could not draw Game Window! %s\n",SDL_GetError());
		return GUI_SDL_FAILURE;
	}
	return GUI_SUCCESS;
}
int update_gw_grid(game_window* gw){
	int k;
	SDL_Rect loc = {.x=UL_BOARD_CORNER_X,.y=UL_BOARD_CORNER_Y,.w=PIECE_SIZE_BMP,.h=PIECE_SIZE_BMP};
	int fail_num = 0;
	fail_num += SDL_RenderFillRect(gw->renderer,&board_rect_borders);//borders of the board.
	for(int i = 0 ; i < GAME_BOARD_SIZE ; i++){
		//x coordinate
		loc.x = UL_BOARD_CORNER_X + i*PIECE_SIZE_PX;
		for(int j=0; j <  GAME_BOARD_SIZE ; j++){
			//y coordinate, adjust colors and fill the rect
			loc.y = UL_BOARD_CORNER_Y + (GAME_BOARD_SIZE - j -1 )*PIECE_SIZE_PX;
			//k is used to hold the modulo. if k ==0 than this is a black slot, o.w white.
			k = (i+j)%2;
			if(k == 0){
				//black - in our case case PERU BROWN
				fail_num += SDL_SetRenderDrawColor(gw->renderer,BLACK_RGB_R,BLACK_RGB_G,BLACK_RGB_B,SDL_BLENDMODE_ADD);
			}else{
				//white
				fail_num += SDL_SetRenderDrawColor(gw->renderer,WHITE_RGB_R,WHITE_RGB_G,WHITE_RGB_B,SDL_BLENDMODE_ADD);
			}
			fail_num += SDL_RenderFillRect(gw->renderer,&loc);//fill the background color
			int code = get_piece_code(gw->board->board[j][i]);
			if(code != 0){
				//the way we set the textures in pieces_tx allows us to get the correct one easily.
				fail_num += SDL_RenderCopy(gw->renderer,gw->pieces_tx[2*code -k],NULL,&loc);//the piece itself - IF AND ONLY IF IT IS NOT AN EMPTY ENTRY.
			}
		}
	}
	if(gw->board->curr_player == WHITE_PLAYER){//set  the borders white
		fail_num += SDL_SetRenderDrawColor(gw->renderer,WHITE_RGB_R,WHITE_RGB_G,WHITE_RGB_B,SDL_BLENDMODE_ADD);
	}else{//set  the borders black
		fail_num += SDL_SetRenderDrawColor(gw->renderer,BLACK_RGB_R,BLACK_RGB_G,BLACK_RGB_B,SDL_BLENDMODE_ADD);
	}
	//return to normal color.
	fail_num += SDL_SetRenderDrawColor(gw->renderer,BACKGROUND_RGB_R,BACKGROUND_RGB_G,BACKGROUND_RGB_B,SDL_BLENDMODE_ADD);
	if(fail_num < 0){
		fprintf(stderr,"ERROR:could not generate grid %s", SDL_GetError());
	}
	return fail_num;
}
void clean_surface_by_threshold(SDL_Surface* s,Uint8 bg_r,Uint8 bg_g,Uint8 bg_b){
	if(!s){return ;}
	//configure these variables to control the thresholds.
	Uint8 white_threshold = 220;
	Uint8 black_threshold = 65;
	//not using any locks - they are not needed in this case - this code runs on SINGLE THREAD.
	Uint32* pixels = (Uint32*) s->pixels;
	Uint8 r,g,b;
	int h = s->h;
	int w = s->w;
	for(int i = 0 ; i < h ; i++){
		for(int j =0 ; j < w ; j++){
			//get the pixel in (i,j) and work on it:
			Uint32 curr_pix = pixels[j*w +i];
			//get the rgb values:
			SDL_GetRGB(curr_pix,s->format,&r,&g,&b);
			//convert to gray level, formula taken from a Stack overflow post.
			Uint8 gray_level =  0.212671f * r + 0.715160f * g + 0.072169f * b;
			if(gray_level >= white_threshold ){
				//turn it to complete white
				curr_pix = SDL_MapRGB(s->format,255,255,255);
			}else if (gray_level <= black_threshold){
				//turn it to complete black:
				curr_pix = SDL_MapRGB(s->format,0,0,0);
			}else{//put the color used in the color key:
				curr_pix = SDL_MapRGB(s->format,bg_r,bg_g,bg_b);
			}
			pixels[j*w +i] = curr_pix;
		}
	}
}
void destroy_gw(Window* w){
	if(!w){return;}
	game_window* gw = (game_window*)w->data;
	destroy_widgets_list(gw->widgets,gw->widget_num);
	for(int i = 1 ; i < 2*GW_CHESS_PIECES ; i++){
		if(gw->pieces_tx[i] != NULL){
			SDL_DestroyTexture(gw->pieces_tx[i]);
		}
	}
	free(gw->pieces_tx);
	destroy_game(gw->board);
	free(gw);
	free(w);
}
void gw_cb_restart(void* gw){
	if(!gw){return;}
	game_window* game = (game_window*)gw;
	game_board* new_board = create_game(game->board->difficulty);
	if(!new_board){
		fprintf(stderr,"ERROR: UNABLE TO RESTART GAME\n");
		return; //no need to crash the program.
	}
	new_board->curr_player = game->initial_player;
	new_board->p_type = game->initial_p_type;
	new_board->user_mode = game->board->user_mode;
	new_board->game_mode = GUI;
	destroy_game(game->board);//get the rid of the current board
	game->board = new_board;
	game->board_locked = (game_state(game->board) != CHESS_STATUS_NORMAL);
	GUI_MESSAGE msg = GUI_SUCCESS;
	if(game->board->p_type == PC ){
		msg = (init_move(game) == GUI_SUCCESS)?msg:GUI_FAILURE;
		msg = (update_gw_status(game) == GUI_SUCCESS)?msg:GUI_FAILURE;
	}
	if(msg != GUI_SUCCESS){
		fire_user_event(FAILURE,NULL,NULL);
	}
}
void gw_cb_save(void* gw){
	if(!gw){return;}
	if(gw_save_current(((game_window*)gw)->board)!=GUI_SUCCESS){
		fprintf(stderr,"ERROR: SAVE FAILED!\n"); // no need to crash.
	}else{
		((game_window*)gw)->save_needed = false;
	}
}
void gw_cb_load(void* gw){
	if(!gw){return;}
	fire_user_event(LOAD_REQUESTED,NULL,NULL);
}
void gw_cb_undo(void* gw){
	if(!gw){return;}
	//no need to check the undo len and game mode. the button is clickable iff the conditions are met.
	((game_window*)gw)->save_needed = true; // a save is needed since the board changed.
	undo_move(((game_window*)gw)->board);
	undo_move(((game_window*)gw)->board);
	update_gw_with_event_firing(gw);
}
void gw_cb_back_to_main(void* gw){
	if(!gw){return;}
	if(!((game_window*)(gw))->save_needed){
		fire_user_event(BACK_TO_MAIN,NULL,NULL);
		return;
	}
	int choice;
	GUI_MESSAGE msg ;
	int ret = gw_show_save_dialog(&choice,((game_window*)(gw))->window);
	if(ret != 0){
		fire_user_event(FAILURE,NULL,NULL);
	}
	switch(choice){
	case CONTINUE_SAVE:
		msg = gw_save_current(((game_window*)(gw))->board);
		if(msg != GUI_SUCCESS){
			fprintf(stderr,"ERROR: encountered a problem while saving, please try again later\n");
		}else{
			fire_user_event(BACK_TO_MAIN,NULL,NULL);
		}
		break;
	case CANCEL:
		break;
	case CONTINUE_NO_SAVE:
		fire_user_event(BACK_TO_MAIN,NULL,NULL);
		break;
	}
}
void gw_cb_exit(void* gw){
	if(!gw){return;}
	int choice;
	SDL_Event quit_event = {.type = SDL_QUIT};
	if(((game_window*)(gw))->save_needed == false){
		SDL_PushEvent(&quit_event);
		return;
	}
	int ret = gw_show_save_dialog(&choice,((game_window*)(gw))->window);
	if(ret != 0){
		fire_user_event(FAILURE,NULL,NULL);
	}
	switch(choice){
	case CONTINUE_SAVE:
		if(gw_save_current(((game_window*)gw)->board)!=GUI_SUCCESS){
			fprintf(stderr,"ERROR: SAVE FAILED!\n"); // no need to crash.
		}else{
			((game_window*)gw)->save_needed = false;
			SDL_PushEvent(&quit_event);
		}
		break;
	case CANCEL:
		break;
	case CONTINUE_NO_SAVE:
		SDL_PushEvent(&quit_event);
		break;
	}
}
int gw_show_save_dialog(int* choice,SDL_Window* w){
	if(!w){return -1;}
	 SDL_MessageBoxButtonData msg_choices[] = {
	        { SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, CONTINUE_SAVE , "Save" },//if the user hits ENTER, this option gets selected.
	        { SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, CANCEL , "Cancel" }, // if the users hits escape, this option gets selected.
			{ 0, CONTINUE_NO_SAVE, "Don't Save" }
	    };
	 SDL_MessageBoxData msg_data =
			{SDL_MESSAGEBOX_WARNING,
			w,
			"Warning",
			"You have unsaved changes",
			SDL_arraysize(msg_choices),
			msg_choices,
			NULL //use default system color scheme
			};
	 return SDL_ShowMessageBox(&msg_data,choice);//show the messagebox
}
void update_drag(game_window* gw ,SDL_Event* e){
	if(!e||!gw|| gw->board_locked){return;}//exit on null inputs, not updating anything if the board is locked.
	switch(e->type){
	case SDL_MOUSEBUTTONDOWN://when the left button gets pressed - we interpret it as starting the drop
		if(e->button.button == SDL_BUTTON_LEFT){
			//record the start position and the
			if(gw->drag_drop_state == RELEASED){
				gw->drag_drop_state = DRAGGING;
				gw->drag_start_x = e->button.x;
				gw->drag_start_y = e->button.y;
			}
		}
		break;
	case SDL_MOUSEMOTION:
		if(gw->drag_drop_state == DRAGGING){
			update_drag_drop_grid(gw,e->button.x,e->button.y);
		}
		break;
	case SDL_MOUSEBUTTONUP:
		if(gw->drag_drop_state == DRAGGING && e->button.button == SDL_BUTTON_LEFT ){
			//the drag ended.
			//finalize the drag event:
			Sint32 end_x = e->button.x;
			Sint32 end_y = e->button.y;
			 finalize_drag_drop(gw,gw->drag_start_x,gw->drag_start_y,end_x,end_y);
		}
		gw->drag_drop_state = RELEASED;
		break;
	}
}
//this function handles the highlight moves feature
void highlight_moves(game_window* gw,SDL_Event* e){
	if(!e||!gw|| gw->board_locked){return;}//exit on null inputs, not updating anything if the board is locked.
	if(gw->board->user_mode == SINGLE_PLAYER_GAME && gw->board->difficulty >2){return;} // cases that match this dont need this option available.
	if(e->type == SDL_MOUSEBUTTONDOWN && e->button.button == SDL_BUTTON_RIGHT){//if the right button got pressed - check for point validity.
		//SDL_Point p = {.x = e->button.x,.y=e->button.y};
		int fail_num = 0;
		if(is_point_valid_self(gw,e->button.x,e->button.y)){
			int column  = (e->button.x - UL_BOARD_CORNER_X)/PIECE_SIZE_PX;//the column
			int row  = GAME_BOARD_SIZE -1 - (e->button.y - UL_BOARD_CORNER_Y)/PIECE_SIZE_PX;//the row
			position pos = {.row = row,.column=column};
			legit_moves* moves = valid_moves_array(gw->board,pos,get_validator(gw->board->board[row][column]));//getting all the possible moves
			if(!moves){
				fire_user_event(FAILURE,NULL,NULL);
				return;
			}
			int R,G,B;
			tool_pos curr_pos = {gw->board->board[row][column],{row,column}};
			for(int i=0; i < moves->moves_num ; i++){
				tool_pos dest_pos = moves->valid_pos[i];
				int dest_row = dest_pos.pos.row, dest_col = dest_pos.pos.column;
				if(gw->board->board[dest_pos.pos.row][dest_pos.pos.column] == EMPTY_ENTRY){//EMPTY ENTRY. THE COLOR MAY CHANGE IF IT IS CATCHABLE.
					R = NEUTRAL_RGB_R;
					G = NEUTRAL_RGB_G;
					B = NEUTRAL_RGB_B;
				}else{ // OPPONENT'S SQUARE
					R = OPPONENT_RGB_R;
					G = OPPONENT_RGB_G;
					B = OPPONENT_RGB_B;
				}
				if(is_catchable_after_move(gw->board,pos,dest_pos.pos)){// CATCHABLE COLOR
					R = CATCHABLE_RGB_R;
					G = CATCHABLE_RGB_G;
					B = CATCHABLE_RGB_B;
				}
				char castle_buffer[MAX_LINE_LENGTH];
				CASTLE_TYPE castle_type = handle_castle_moves(curr_pos.pos, castle_buffer,
										castle_buffer, gw->board, dest_pos.pos);
				if(castle_type != CASTLE_UNDEF && (dest_pos.pos.row == 0||dest_pos.pos.row == GAME_BOARD_SIZE -1))
                    fail_num += (draw_lines_on_square(gw->renderer,dest_pos.pos.row,4,
							CASTLE_RGB_R,CASTLE_RGB_G,CASTLE_RGB_B) != GUI_SUCCESS)?-1:0;
				if(gw->board->board[curr_pos.pos.row][curr_pos.pos.column] == WHITE_KING|| gw->board->board[curr_pos.pos.row][curr_pos.pos.column] == BLACK_KING){
					valid_move move_status = is_valid_king(gw->board,curr_pos.pos,dest_pos.pos);
					if(move_status == CASTLE_MOVE){
						dest_col = (dest_pos.pos.column > curr_pos.pos.column)? GAME_BOARD_SIZE -1 : 0; //which sqaure will get highlighted.
						R = CASTLE_RGB_R,G = CASTLE_RGB_G , B = CASTLE_RGB_B;
					}
				}
				fail_num += (draw_lines_on_square(gw->renderer,dest_row,dest_col,R,G,B) != GUI_SUCCESS)?-1:0;
			}
			destroy_legit_moves(moves);
		}
		if(fail_num != 0) {//something went wrong
			fire_user_event(FAILURE,NULL,NULL);
		}
	 }
}
//this function is called when the drag ended.
void finalize_drag_drop(game_window* gw,Sint32 start_x,Sint32 start_y,Sint32 end_x,Sint32 end_y){
	CHESS_STATUS board_stat;
	if(is_point_valid_self(gw,start_x,start_y)&&is_point_valid_opponent(gw,end_x,end_y)){
		int start_column  = (start_x - UL_BOARD_CORNER_X)/PIECE_SIZE_PX;//the column where the drag began
		int start_row  = GAME_BOARD_SIZE -1 - (start_y - UL_BOARD_CORNER_Y)/PIECE_SIZE_PX;//the row where the drag began
		int end_column  = (end_x - UL_BOARD_CORNER_X)/PIECE_SIZE_PX;//the column where the drag ENDED
		int end_row  = GAME_BOARD_SIZE -1 - (end_y - UL_BOARD_CORNER_Y)/PIECE_SIZE_PX;//the row where the drag ENDED
		tool_pos t_start = {gw->board->board[start_row][start_column],{.row=start_row,.column=start_column}};
		tool_pos t_end = {gw->board->board[end_row][end_column],{.row=end_row,.column=end_column}};
		//perform the move itself, if allowed:
		valid_move move_type = valid_move_for_tool(gw->board,t_start.pos,t_end.pos,get_validator(t_start.tool_type));
		if((move_type == VALID_MOVE || move_type == CASTLE_MOVE)&& !gw->board_locked){
			if((t_start.tool_type == WHITE_PAWN && start_row == GAME_BOARD_SIZE -2 && end_row == GAME_BOARD_SIZE -1 && gw->board->curr_player == WHITE_PLAYER) ||
					(t_start.tool_type == BLACK_PAWN &&start_row == 1 && end_row == 0 && gw->board->curr_player == BLACK_PLAYER)){
				//this is a pawn promotion
				t_start.tool_type = get_pawn_promotion(gw->window,gw->board->curr_player);
			}
			set_move(gw->board,t_start,t_end);
			update_gw_with_event_firing(gw);
			SDL_RenderPresent(gw->renderer);
			board_stat = game_state(gw->board);
			if(gw->board->user_mode == SINGLE_PLAYER_GAME){// if this is single user mode, perform move suggested by minimax.
				gw->board_locked = (board_stat != CHESS_STATUS_NORMAL);
				if(!gw->board_locked){
					//get the computer's move from minimax.
					gw->game_waiting = true; //prevent the board from handling events.
					gw->board->p_type = PC;
					MOVE_COMMAND move = suggest_move(gw->board,gw->board->difficulty);

					notify_promotion(gw,move);//if this is a pawn promotion, the user will be notified,
					set_move(gw->board,move.from,move.to);

					gw->board->p_type = HUMAN;
					gw->game_waiting = false; //allow the board to handle events.
				}
				update_gw_with_event_firing(gw);
			}
			gw->save_needed = true;//moves were done, need to save.
		}else{
			//put everything back where it was:
			update_gw_with_event_firing(gw);
		}
	}else{
		update_gw_with_event_firing(gw);
	}
}
GUI_MESSAGE init_move(game_window* gw){
	if(!gw){return GUI_INVALID_ARGUMENT;}
	if(gw->board->p_type ==PC && !gw->board_locked){
		gw->game_waiting = true; //prevent the board from handling events.
		MOVE_COMMAND move = suggest_move(gw->board,(gw->board->difficulty == 5)? 4:gw->board->difficulty);
		GUI_MESSAGE msg =  (set_move(gw->board,move.from,move.to) == CHESS_STATUS_SUCCESS) ? GUI_SUCCESS:GUI_FAILURE;
		gw->board->p_type = HUMAN;
		gw->game_waiting = false; //allow the board to handle events.
		return msg;
	}
	return GUI_SUCCESS;
}
//DEPRECATED
void notify_finish(CHESS_STATUS board_stat,bool computer_move){
	if(board_stat == CHESS_STATUS_TIE){
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "TIE",
				"GAME OVER - IT'S A TIE!", NULL );
		return;
	}
	if(computer_move){
		//IF WE GOT HERE THEN THIS IS A MATE - SO THE COMPUTER WON.
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "MATE",
				"GAME OVER - COMPUTER WINS!!", NULL );
	}else{
		//THIS IS A MATE BY THE USER - SO THE USER WON.
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "MATE",
				"GAME OVER - YOU WIN!!", NULL );
	}
}
void update_drag_drop_grid(game_window* gw,Sint32 x,Sint32 y){
	if(!gw){return;}
	//determine the actual row and column of where the drag BEGAN -
	if(is_point_valid_self(gw,gw->drag_start_x ,gw->drag_start_y)){
		int column  = (gw->drag_start_x - UL_BOARD_CORNER_X)/PIECE_SIZE_PX;//the column where the drag began
		int row  = GAME_BOARD_SIZE -1 - (gw->drag_start_y - UL_BOARD_CORNER_Y)/PIECE_SIZE_PX;//the row where the drag began
		SDL_Rect orig_loc = {.x=UL_BOARD_CORNER_X + column*PIECE_SIZE_PX,.y=UL_BOARD_CORNER_Y + (GAME_BOARD_SIZE -1 -row)*PIECE_SIZE_PX,.w=PIECE_SIZE_BMP,.h=PIECE_SIZE_BMP};
		//the drag began inside the board - update accordingly, make the original slot vacant,start drawing
		int fail_num = (update_gw_status(gw) == GUI_SUCCESS) ?0:-1;//the number of failures. SDL returns negative number on failure, so
		//this variable will never be zero "by luck" (i,e - two error codes summing to zero)
		int k = (row+column)%2;
		if(k == 0){
			//black - in our case case PERU BROWN
			fail_num += SDL_SetRenderDrawColor(gw->renderer,BLACK_RGB_R,BLACK_RGB_G,BLACK_RGB_B,SDL_BLENDMODE_ADD);
		}else{
			//white
			fail_num += SDL_SetRenderDrawColor(gw->renderer,WHITE_RGB_R,WHITE_RGB_G,WHITE_RGB_G,SDL_BLENDMODE_ADD);
		}
		fail_num += SDL_RenderFillRect(gw->renderer,&orig_loc);//fill the background color
		int code = get_piece_code(gw->board->board[row][column]);
		SDL_Rect mouse_loc = {.x=x-22,.y=y-22,.w=PIECE_SIZE_BMP,.h=PIECE_SIZE_BMP};
		//back to normal - this is to avoid changing the color of the entire window.
		fail_num += SDL_SetRenderDrawColor(gw->renderer,BACKGROUND_RGB_R,BACKGROUND_RGB_G,BACKGROUND_RGB_B,SDL_BLENDMODE_ADD);
		if(code != 0){
			//the way we set the textures in pieces_tx allows us to get the correct one easily.
			fail_num += SDL_RenderCopy(gw->renderer,gw->pieces_tx[2*code -k],NULL,&mouse_loc);//the piece itself - IF AND ONLY IF IT IS NOT AN EMPTY ENTRY.
		}
		gw->render_needed = true;
		if(fail_num != 0){
			//push event with error
			fprintf(stderr,"ERROR: A PROBLEM OCCURED WHILE HANDLING EVENT %s \n",SDL_GetError());
			fire_user_event(FAILURE,NULL,NULL);
		}
	}
}
//returns true if and only if (x,y) is inside the board itself, and the tool in the corresponding place belongs to the CURRENT player..
bool is_point_valid_self(game_window* gw,Sint32 x,Sint32 y){
	if(!gw){return false;}
	SDL_Point p = {.x=x,.y=y};
	if(SDL_PointInRect(&p,&board_rect)){//make sure the point is in the rect
		//get the row&column
		int column  = (x - UL_BOARD_CORNER_X)/PIECE_SIZE_PX;//the column
		int row  = GAME_BOARD_SIZE -1 - (y - UL_BOARD_CORNER_Y)/PIECE_SIZE_PX;//the row
		int piece_code  = get_piece_code(gw->board->board[row][column]);
		player_color curr_player = gw->board->curr_player;
		return (piece_code >= 1 && piece_code <=6 && curr_player == WHITE_PLAYER) ||
				(piece_code >= 7 && piece_code <=12 && curr_player == BLACK_PLAYER) ;
	}
	return false;
}
//returns true if and only if (x,y) is inside the board itself, and the tool in the corresponding place belongs to the opponent player OR is an empty entry
bool is_point_valid_opponent(game_window* gw,Sint32 x,Sint32 y){
	if(!gw){return false;}
	SDL_Point p = {.x=x,.y=y};
	if(SDL_PointInRect(&p,&board_rect)){//make sure the point is in the rect
		//get the row&column
		int column  = (x - UL_BOARD_CORNER_X)/PIECE_SIZE_PX;//the column
		int row  = GAME_BOARD_SIZE -1 - (y - UL_BOARD_CORNER_Y)/PIECE_SIZE_PX;//the row
		int piece_code  = get_piece_code(gw->board->board[row][column]);
		player_color curr_player = gw->board->curr_player;
		return (piece_code == 0) || (piece_code >= 1 && piece_code <=6 && curr_player == BLACK_PLAYER) ||
				(piece_code >= 7 && piece_code <=12 && curr_player == WHITE_PLAYER) ;
	}
	return false;
}

void update_gw_with_event_firing(game_window* gw){
	if(update_gw_status(gw) != GUI_SUCCESS){// either thread creation failed or updateing failed.
		//push event with error
		fprintf(stderr,"ERROR: A PROBLEM OCCURED WHILE HANDLING EVENT %s \n",SDL_GetError());
		fire_user_event(FAILURE,NULL,NULL);
	}
}
game_tools get_pawn_promotion(SDL_Window* w,player_color color){
	int choice;
	 SDL_MessageBoxButtonData msg_choices [] = {
	        { 0, 0 , 	"PAWN" },
	        { 0, 1 , "KNIGHT" },
			{ 0, 2, 	"BISHOP " },
			{ 0, 3, 	"QUEEN" },
			{ 0, 4, 	"ROOK" },
	    };
	 game_tools black_tools[] = {BLACK_PAWN,BLACK_KNIGHT,BLACK_BISHOP,BLACK_QUEEN};
	 game_tools white_tools[] = {WHITE_PAWN,WHITE_KNIGHT,WHITE_BISHOP,WHITE_QUEEN};
	 game_tools* tools = (color == WHITE_PLAYER)?white_tools:black_tools;
	 char *message = (color == WHITE_PLAYER)? "WHITE PLAYER - CHOOSE YOUR TOOL":"BLACK PLAYER - CHOOSE YOUR TOOL";
	 SDL_MessageBoxData msg_data =
			{SDL_MESSAGEBOX_INFORMATION,
			w,
			"PAWN PROMOTION",
			message,
			SDL_arraysize(msg_choices),
			msg_choices,
			NULL //use default system color scheme
			};
	 int ret = SDL_ShowMessageBox(&msg_data,&choice);//show the messagebox
	 if(ret < 0){// a problem occured, print and continue with pawn.
		 fprintf(stderr,"ERROR: PAWN PRMOTION FAILED OR DIALOG WAS CLOSED. PAWN SELECTED BY DEFAULT\n");
		 choice = 0;
	 }
	 return tools[choice];
}
int get_piece_code(game_tools tool){
	switch(tool){
	case EMPTY_ENTRY:
		return 0;
	case WHITE_PAWN:
		return 1;
	case WHITE_KING:
		return 2;
	case WHITE_QUEEN:
		return 3;
	case WHITE_BISHOP:
		return 4;
	case WHITE_ROOK:
		return 5;
	case WHITE_KNIGHT:
		return 6;
	case BLACK_PAWN:
		return 7;
	case BLACK_KING:
		return 8;
	case BLACK_QUEEN:
		return 9;
	case BLACK_BISHOP:
		return 10;
	case BLACK_ROOK:
		return 11;
	case BLACK_KNIGHT:
		return 12;
	default:
		return 16;
	}
}
GUI_MESSAGE gw_save_current(game_board* board){
	if(!board){return GUI_INVALID_ARGUMENT;}
	//create an xml_game for the board:
	int mode = (board->user_mode == MULTI_PLAYER_GAME)?2:1;
	int col = board->curr_player;
	xml_game* game = create_xml_game(board,board->difficulty,mode,col);
	if(!game){
		return GUI_FS_FAILURE;
	}
	//copy the old files to new ones:
	char file_name[35] = "./svg/gui_saved_game_i.xml";
	file_name[26] = '\0';
	char file_name_new [35] = "./svg/gui_saved_game_i.xml";
	file_name_new[26] = '\0';
	for(int i = MAX_FILES -1  ; i > 0; i--){
		file_name[21] = i + 48 ; //ASCII
		file_name_new[21] = i + 49 ; //ASCII - THIS FILENAME IS ADVANCED BY ONE.
		if(access(file_name,F_OK) != -1){
			//SDL_Delay(20); //allow the OS to clean the file descriptor table before continuing. O.W the file will not get renamed.
			//the file exists - renaming it.
			remove(file_name_new); //not needed anymore. safe even file does not exist.
			rename(file_name,file_name_new);
		}
	}
	GUI_MESSAGE msg = (write_to_file("./svg/gui_saved_game_1.xml",game) == 0)?GUI_SUCCESS:GUI_FS_FAILURE;
	destroy_xml_game(game);//NULL SAFE.
	return msg;
}
void update_game_state(game_window* gw){
	if(!gw){return;}
	CHESS_STATUS board_state = game_state(gw->board);
	if(is_check(gw->board,WHITE_PLAYER) ){
		((Button*)(gw->widgets[6]->data))->set_active(gw->widgets[6],false);//just make the picture appear. no need to be clickable
	}
	if(is_check(gw->board,BLACK_PLAYER) ){
		((Button*)(gw->widgets[7]->data))->set_active(gw->widgets[7],false);//just make the picture appear. no need to be clickable
	}
	switch(board_state){
	case CHESS_STATUS_WHITE_IN_MATE:
		((Button*)(gw->widgets[8]->data))->set_active(gw->widgets[8],false);//just make the picture appear. no need to be clickable
		break;
	case CHESS_STATUS_BLACK_IN_MATE:
		((Button*)(gw->widgets[9]->data))->set_active(gw->widgets[9],false);//just make the picture appear. no need to be clickable
		break;
	case CHESS_STATUS_TIE:
		((Button*)(gw->widgets[10]->data))->set_active(gw->widgets[10],false);//just make the picture appear. no need to be clickable
		break;
	case CHESS_STATUS_NORMAL:
		break;
	default:
		fprintf(stderr,"ERROR: UNABLE TO GAME STATE IN GUI %s\n",SDL_GetError());
		break;
	}
}
void notify_promotion(game_window* gw,MOVE_COMMAND move){
	tool_pos t_start = move.from;
	tool_pos t_end = move.to;
	if((t_start.tool_type == WHITE_PAWN && t_start.pos.row == GAME_BOARD_SIZE -2 && t_end.pos.row == GAME_BOARD_SIZE -1 && gw->board->curr_player == WHITE_PLAYER) ||
				(t_start.tool_type == BLACK_PAWN &&t_start.pos.row == 1 && t_end.pos.row == 0 && gw->board->curr_player == BLACK_PLAYER)){
				//this is a pawn promotion, notify the user.
				char msg[MAX_STRING +1] ;
				if(t_end.tool_type == BLACK_KNIGHT ||t_end.tool_type == WHITE_KNIGHT ){
					sprintf(msg,"Computer: move %s <%d,%c> to <%d,%c> and promote to %s\n","pawn",t_start.pos.row + 1,t_start.pos.column + 'A',t_end.pos.row + 1,t_end.pos.column + 'A', "Knight");

				}else{
					sprintf(msg,"Computer: move %s <%d,%c> to <%d,%c> and promote to %s\n","pawn",t_start.pos.row + 1,t_start.pos.column + 'A',t_end.pos.row + 1,t_end.pos.column + 'A', "Queen");
				}
				SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "PAWN PROMOTION",
									msg, NULL );
				}
}

