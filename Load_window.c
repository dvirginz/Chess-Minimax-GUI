/*
 * Load_window.c
 *
 *  Created on: 15 Sep 2017
 *      Author: GalL
 */
#include "Button.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include "Load_window.h"
#include "XML.h"
#define LW_WIDGET_NUM 7
#define MAX_STRING 1024
Widget** create_lw_widgets(SDL_Renderer* r){
	/** the following indices assigned are considered constants in the scope of this file.
	 * for code readability reasons they are not replaced with constants/enum values.
	 */
	if(!r){return NULL;}
	Widget** res = (Widget**)malloc(sizeof(Widget*)*LW_WIDGET_NUM);
	if(!res){
		fprintf(stderr,"ERROR: MALLOC ERROR\n");
		return NULL;
	}
	//start creating the widgets
	//load button
	res[0] = create_button(r,(SDL_Rect){.x = 75,.y = 500 , .w = 180, .h=40 },"./img/load_active.bmp","./img/load_inactive.bmp",lw_cb_load);
	//back
	res[1] = create_button(r,(SDL_Rect){.x = 275,.y = 500 , .w = 180, .h=40 },"./img/back_active.bmp","./img/back_active.bmp",lw_cb_back);
	//buttons for slots
	res[2] = create_button(r,(SDL_Rect){.x = 75,.y = 50 , .w = 180, .h=40 }, "./img/slot1_active.bmp","./img/slot1_inactive.bmp",lw_cb_slot1);
	res[3] = create_button(r,(SDL_Rect){.x = 75,.y = 100 , .w = 180, .h=40 },"./img/slot2_active.bmp","./img/slot2_inactive.bmp",lw_cb_slot2);
	res[4] = create_button(r,(SDL_Rect){.x = 75,.y = 150 , .w = 180, .h=40 },"./img/slot3_active.bmp","./img/slot3_inactive.bmp",lw_cb_slot3);
	res[5] = create_button(r,(SDL_Rect){.x = 75,.y = 200 , .w = 180, .h=40 },"./img/slot4_active.bmp","./img/slot4_inactive.bmp",lw_cb_slot4);
	res[6] = create_button(r,(SDL_Rect){.x = 75,.y = 250 , .w = 180, .h=40 },"./img/slot5_active.bmp","./img/slot5_inactive.bmp",lw_cb_slot5);
	for(int i = 0 ; i < LW_WIDGET_NUM ; i++){
		if(!res[i]){
			destroy_widgets_list(res,LW_WIDGET_NUM);
			fprintf(stderr,"ERROR: A PROBLEM OCCURED WHILE CREATING WIDGETS AT LOAD WINDOW\n");
			return NULL;
		}
	}
	return res;
}
Window* create_load_window(SDL_Window* window,SDL_Renderer* r){
	if(!window || !r){return NULL;}
	Window* res = (Window*)malloc(sizeof(Window));
	//alloc space for the main_window itself
	load_window* data = (load_window*)malloc(sizeof(load_window));
	//create the window.
	Widget** widgets = create_lw_widgets(r);
	if(!res || !data || !widgets){
		free(data);
		free(res);
		destroy_widgets_list(widgets,LW_WIDGET_NUM);
		//SDL_DestroyWindow(window);
		//SDL_DestroyRenderer(r);
		return NULL;
	}
	data->widget_num = LW_WIDGET_NUM;
	data->widgets = widgets;
	data->renderer = r;
	data->window = window;
	data->current_choice = 0;
	data->draw_needed = true;
	res->data = data;
	res->destroy = destroy_lw;
	res->handle_event = handle_lw_event;
	res->draw = draw_lw;
	return res;
}
GUI_MESSAGE draw_lw(Window* w ){
	if(!w){return GUI_INVALID_ARGUMENT;}
	load_window* lw = (load_window*)w->data;
	int fail_num = 0;
	fail_num += SDL_SetRenderDrawColor(lw->renderer,BACKGROUND_RGB_R,BACKGROUND_RGB_G,BACKGROUND_RGB_B,SDL_BLENDMODE_ADD);
	fail_num += SDL_RenderClear(lw->renderer);
	GUI_MESSAGE msg = GUI_SUCCESS;
	//make all widgets inactive:
	for(int i=0; i < LW_WIDGET_NUM; i++){
		(((Button*)((lw->widgets[i])->data)))->is_active = false;
	}
	//load active iff choice is not zero.
	msg = (((Button*)lw->widgets[0]->data)->set_active(lw->widgets[0],(lw->current_choice != 0)) != GUI_SUCCESS) ? GUI_SDL_FAILURE : msg;
	//back is always active
	msg = (((Button*)lw->widgets[1]->data)->set_active(lw->widgets[1],true) != GUI_SUCCESS) ? GUI_SDL_FAILURE : msg;
	//draw only for slots where the associated file exists and has READWRITE permissions:
	char file_name[35] = "./svg/gui_saved_game_i.xml";
	file_name[26] = '\0';
	bool is_chosen;
	for(int i = 2 ; i < LW_WIDGET_NUM ; i++){
		file_name[21] = i + 47 ; //ASCII
		is_chosen = (lw->current_choice == i-1);
		if(access(file_name,F_OK) != -1){
			//the file exists
			msg = (((Button*)lw->widgets[i]->data)->set_active(lw->widgets[i],is_chosen) != GUI_SUCCESS) ? GUI_SDL_FAILURE : msg;
			(((Button*)(lw->widgets[i]->data)))->is_active = true; // all the choice buttons which are drawn are actually active - the previous call was to control the background image.
		}
	}
	lw->draw_needed=false;
	SDL_RenderPresent(lw->renderer);
	if(fail_num!=0){
		fprintf(stderr,"ERROR: COULD NOT DRAW Load Window %s",SDL_GetError());
		return GUI_SDL_FAILURE;
	}
	return msg;
}
void handle_lw_event(Window* w  ,SDL_Event* e){
	if(!w||!e){return;}
	load_window* lw = (load_window*)(w->data);
	for(int i = 0 ; i < LW_WIDGET_NUM;i++){
		lw->widgets[i]->handle_event(lw->widgets[i],e,(void*)lw);
	}
	if(lw->draw_needed){
		w->draw(w);//this is a very small window, can be drawn if necessary.
	}
}
void destroy_lw(Window* w){
	if(!w){return;}
	load_window* lw = (load_window*)w->data;
	destroy_widgets_list(lw->widgets,lw->widget_num);
	free(lw);
	free(w);
}
void lw_cb_slot1(void* lw){
	if(lw){
	((load_window*)(lw))->current_choice = 1;
	((load_window*)(lw))->draw_needed = true;
	}
}
void lw_cb_slot2(void* lw){
	if(lw){
	((load_window*)(lw))->current_choice = 2;
	((load_window*)(lw))->draw_needed = true;
	}
}
void lw_cb_slot3(void* lw){
	if(lw){
	((load_window*)(lw))->current_choice = 3;
	((load_window*)(lw))->draw_needed = true;
	}
}
void lw_cb_slot4(void* lw){
	if(lw){
	((load_window*)(lw))->current_choice = 4;
	((load_window*)(lw))->draw_needed = true;
	}
}
void lw_cb_slot5(void* lw){
	if(lw){
	((load_window*)(lw))->current_choice = 5;
	((load_window*)(lw))->draw_needed = true;
	}
}
void lw_cb_load(void* lw){
	char file_name[35] = "./svg/gui_saved_game_i.xml";
	file_name[21] =  ((load_window*)(lw))->current_choice + 48;
	xml_game* x_game = load_from_file(file_name);//AS INSTRUCTED BY THE SPECIFICATION, THE FILE CAN BE ASSUMED TO BE VALID.
	if(!x_game){
		fprintf(stderr,"ERROR: COULD NOT LOAD FILE\n");
		return ; // in this case, we avoid crashing the program.

	}
	game_board* board = x_game->game;
	if(board->user_mode == SINGLE_PLAYER_GAME && (x_game->user_color)!= (board->curr_player == WHITE_PLAYER)?1:0){
		//the computer needs to take the first turn.
		board->p_type = PC;
	}
	destroy_xml_game(x_game);
	fire_user_event(START_GAME,board,NULL);//FIRE THE EVENT WHICH WILL CAUSE THE EVENT LOOP TO START THE GAME.
}
void lw_cb_back(void* lw){
	if(lw){
		fire_user_event(BACK_TO_PREV,NULL,NULL);
	}
}

