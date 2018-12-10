/*
 * Game_dialog.c
 *
 *  Created on: 2 Sep 2017
 *      Author: GalL
 */
#include "Game_dialog.h"
#include "GameBoard.h"
#include "Button.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#define GD_WIDGET_NUM 12
int gd_create_buttons(SDL_Renderer* r,SDL_Rect ul,SDL_Rect ll,SDL_Rect ur,SDL_Rect lr,Widget** dest){
	//one/two player button
	dest[0] = create_button(r,ul,"./img/one_player_active.bmp","./img/one_player_inactive.bmp",gd_cb_one_player);
	dest[1] = create_button(r,ur,"./img/two_players_active.bmp","./img/two_players_inactive.bmp",gd_cb_two_player);
	//start button
	dest[2] = create_button(r,lr,"./img/start_active.bmp","./img/start_inactive.bmp",gd_cb_start);
	//back button
	dest[3] = create_button(r,ll,"./img/back_active.bmp","./img/back_inactive.bmp",gd_cb_back);
	//"next" button
	dest[4] = create_button(r,lr,"./img/next_active.bmp","./img/next_inactive.bmp",gd_cb_next);
	//positions for the diff buttons
	SDL_Rect diffs[5];
	for(int i = 0 ; i < 5 ;i++){
		int col = 50+ 60*i ;
		diffs[i] = (SDL_Rect){.x=200,.y=col,.w=180,.h=40};
	}//difficulty buttons
	dest[5] = create_button(r,diffs[0],"./img/noob_active.bmp","./img/noob_inactive.bmp",gd_cb_noob);
	dest[6] = create_button(r,diffs[1],"./img/easy_active.bmp","./img/easy_inactive.bmp",gd_cb_easy);
	dest[7] = create_button(r,diffs[2],"./img/moderate_active.bmp","./img/moderate_inactive.bmp",gd_cb_moderate);
	dest[8] = create_button(r,diffs[3],"./img/hard_active.bmp","./img/hard_inactive.bmp",gd_cb_hard);
	dest[9] = create_button(r,diffs[4],"./img/expert_active.bmp","./img/expert_inactive.bmp",gd_cb_expert);
	//user color buttons
	SDL_Rect black_pos = {.x= 200,.y=200,.w=45,.h=45};
	SDL_Rect white_pos = {.x= 270,.y=200,.w=45,.h=45};
	dest[10] = create_button(r,white_pos,"./img/white_king_active.bmp","./img/white_king_inactive.bmp",gd_cb_white);
	dest[11] = create_button(r,black_pos,"./img/black_king_active.bmp","./img/black_king_inactive.bmp",gd_cb_black);
	//make sure all widgets created successfully:
	for(int i = 0 ; i< GD_WIDGET_NUM ; i++){
		if(!dest[i]){
			destroy_widgets_list(dest,GD_WIDGET_NUM);
			return -1;
		}
	}
	return 0;
}
//updates the buttons that need to be shown
GUI_MESSAGE update_gd_status(game_dialog* gd){
	if(!gd){return GUI_INVALID_ARGUMENT;}
	Widget** widgets = gd->widgets;
	bool diff_chosen = (gd->difficulty != DIFF_NOT_SET);
	//make all buttons stop handling events.
	for(int i = 0 ; i< gd->widget_num ; i++){
		(((Button*)widgets[i]->data))->is_active = false;
	}
	//clear the rendering target
	if(SDL_RenderClear(gd->renderer) != 0){return GUI_SDL_FAILURE;}
	GUI_MESSAGE msg = GUI_SUCCESS;
	switch(gd->phase){
	case GUI_GAME_MODE:
		//one player, two player,next/start and back will get drawn
		msg = (((Button*)widgets[0]->data)->set_active(widgets[0],true) != GUI_SUCCESS) ? GUI_SDL_FAILURE : msg;
		msg = (((Button*)widgets[1]->data)->set_active(widgets[1],true) != GUI_SUCCESS) ? GUI_SDL_FAILURE : msg;
		//back
		msg = (((Button*)widgets[3]->data)->set_active(widgets[3],true) != GUI_SUCCESS) ? GUI_SDL_FAILURE : msg;
		switch(gd->game_mode){
			case SINGLE_PLAYER:
				//next button,active
				msg = (((Button*)widgets[4]->data)->set_active(widgets[4],true) != GUI_SUCCESS) ? GUI_SDL_FAILURE : msg;
				break;
			case MULTI_PLAYER:
				//start button, active
				msg = (((Button*)widgets[2]->data)->set_active(widgets[2],true) != GUI_SUCCESS) ? GUI_SDL_FAILURE : msg;
				break;
			default:
				//next button,inactive:
				msg = (((Button*)widgets[4]->data)->set_active(widgets[4],false) != GUI_SUCCESS) ? GUI_SDL_FAILURE : msg;
		}
		break;
	case GUI_DIFFICULTY:
		//difficulties, next and back:
		//back
		msg = (((Button*)widgets[3]->data)->set_active(widgets[3],true) != GUI_SUCCESS) ? GUI_SDL_FAILURE : msg;
		//next -
		msg = (((Button*)widgets[4]->data)->set_active(widgets[4],diff_chosen) != GUI_SUCCESS) ? GUI_SDL_FAILURE : msg;
		//difficulties:
		for(int i = 5 ; i<=9 ; i++){
			msg = (((Button*)widgets[i]->data)->set_active(widgets[i],false) != GUI_SUCCESS) ? GUI_SDL_FAILURE : msg;
			((Button*)widgets[i]->data)->is_active = true;
		}//if a difficulty was selected, re-organize
		if(diff_chosen){//change the background of the chosen difficulty
			msg = (((Button*)widgets[gd->difficulty+5]->data)->set_active(widgets[gd->difficulty+5],true) != GUI_SUCCESS) ? GUI_SDL_FAILURE : msg;
		}
		break;
	case GUI_USER_COLOR:
		//colors, start and back will get drawn:
		//back
		msg = (((Button*)widgets[3]->data)->set_active(widgets[3],true) != GUI_SUCCESS) ? GUI_SDL_FAILURE : msg;
		//start will be clickable iff a color was chosen.
		msg = (((Button*)widgets[2]->data)->set_active(widgets[2],(gd->color != CLR_NOT_SET)) != GUI_SUCCESS) ? GUI_SDL_FAILURE : msg;
		//black&white:
		msg = (((Button*)widgets[10]->data)->set_active(widgets[10],(gd->color == GUI_WHITE_PLAYER)) != GUI_SUCCESS) ? GUI_SDL_FAILURE : msg;
		((Button*)widgets[10]->data)->is_active = true;
		msg = (((Button*)widgets[11]->data)->set_active(widgets[11],(gd->color == GUI_BLACK_PLAYER)) != GUI_SUCCESS) ? GUI_SDL_FAILURE : msg;
		((Button*)widgets[11]->data)->is_active = true;
		break;
	default:
		return GUI_SUCCESS;
	}
	((game_dialog*)gd)->render_needed = true;
	return msg;
}
Widget** create_gd_widgets(SDL_Renderer* r){
	if(!r){return NULL;}
	Widget** res = (Widget**)malloc(sizeof(Widget*)*GD_WIDGET_NUM);
	if(!res){
		fprintf(stderr,"ERROR: MALLOC ERROR\n");
		return NULL;
	}
	//locations for the 4  buttons
	//upper left button location
	SDL_Rect ul = {.x= 100,.y= 50,.w=180,.h=40};
	//lower left
	SDL_Rect ll = {.x= 100,.y= 350,.w=180,.h=40};
	//upper right
	SDL_Rect ur = {.x= 400,.y= 50,.w=180,.h=40};
	//lower right
	SDL_Rect lr = {.x= 400,.y= 350,.w=180,.h=40};
	//create the widgets
	if(gd_create_buttons( r, ul, ll, ur,lr,res) != 0 ){
		fprintf(stderr,"ERROR CREATING DIALOG WINDOW WIDGETS. %s \n",(char*)SDL_GetError());
		free(res);
		return NULL;
	}
	return res;
}
Window* create_dialog(SDL_Window* window,SDL_Renderer* r){
	if(!window||!r){return NULL;}
	Window* res = (Window*)malloc(sizeof(Window));
	//alloc space for the dialog window  itself
	game_dialog* data = (game_dialog*)malloc(sizeof(game_dialog));
	//create the window.
	//SDL_Window* window = SDL_CreateWindow("Settings",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,800,800,SDL_WINDOW_BORDERLESS);
	//SDL_Renderer* r = SDL_CreateRenderer(window,-1,SDL_RENDERER_TARGETTEXTURE || SDL_RENDERER_SOFTWARE);
	Widget** widgets = create_gd_widgets(r);
	if(!res || !data || !window|| !r || !widgets){
		free(data);
		free(res);
		destroy_widgets_list(widgets,GD_WIDGET_NUM);
		//SDL_DestroyWindow(window);
		//SDL_DestroyRenderer(r);
		return NULL;
	}
	data->widget_num = GD_WIDGET_NUM;
	data->widgets = widgets;
	data->renderer = r;
	data->window = window;
	data->phase = GUI_GAME_MODE;
	data->game_mode = MODE_NOT_SET;
	data->difficulty = DIFF_NOT_SET;
	data->color = CLR_NOT_SET;
	data->render_needed = true;
	res->data = data;
	res->destroy = destroy_gd;
	res->handle_event = handle_gd_event;
	res->draw = draw_gd;
	return res;
}
GUI_MESSAGE draw_gd(Window* w ){
	if(!w){return GUI_INVALID_ARGUMENT;}
	game_dialog* gd = (game_dialog*)w->data;
	int fail_num = 0;
	fail_num += SDL_SetRenderDrawColor(gd->renderer,BACKGROUND_RGB_R,BACKGROUND_RGB_G,BACKGROUND_RGB_B,SDL_BLENDMODE_ADD);
	fail_num += SDL_RenderClear(gd->renderer);
	GUI_MESSAGE msg = update_gd_status(gd);
	SDL_RenderPresent(gd->renderer);
	gd->render_needed = false;
	if(fail_num!=0 || msg != GUI_SUCCESS){
		fprintf(stderr,"ERROR DRAWING GD %s\n",SDL_GetError());
		return GUI_SDL_FAILURE;
	}
	return GUI_SUCCESS;
}
void handle_gd_event(Window* w  ,SDL_Event* e){
	if(!w||!e){return;}
	game_dialog* gd = (game_dialog*)(w->data);
	//each widget will handle the event
	for(int i = 0 ; i < gd->widget_num;i++){
		gd->widgets[i]->handle_event(gd->widgets[i],e,(void*)gd);
	}
	//re-render so changes will take place.
	if(gd->render_needed){
		SDL_RenderPresent(gd->renderer);
		gd->render_needed = false;
	}
}
//callback function for the one player button
void gd_cb_one_player(void* gd){
	if(!gd){return;}
	((game_dialog*)gd)->game_mode = SINGLE_PLAYER;
	Widget** widgets = ((game_dialog*)gd)->widgets;
	//change background for two player button but keep it active.
	((Button*)(widgets[1]->data))->set_active(widgets[1],false);
	((Button*)(widgets[1]->data))->is_active = true;
	//make the next button active,start inactive, make the one player active (if was'nt until now)
	((Button*)(widgets[4]->data))->set_active(widgets[4],true);
	((Button*)(widgets[2]->data))->is_active = false;
	((Button*)(widgets[0]->data))->set_active(widgets[0],true);
	((game_dialog*)gd)->render_needed = true;
}
//callback function for the two player button
void gd_cb_two_player(void* gd){
	Widget** widgets = ((game_dialog*)gd)->widgets;
	//change background for two player button but keep it active.
	((Button*)(widgets[0]->data))->set_active(widgets[0],false);
	((Button*)(widgets[0]->data))->is_active = true;
	//make the start button active, make the one player active (if was'nt until now)
	((Button*)(widgets[2]->data))->set_active(widgets[2],true);
	((Button*)(widgets[4]->data))->is_active = false;
	((game_dialog*)gd)->game_mode = MULTI_PLAYER;
	((Button*)(widgets[0]->data))->set_active(widgets[1],true);
	((game_dialog*)gd)->render_needed = true;
}
//callback function for the "next" button
void gd_cb_next(void* gd){
	//if the next button was clickable, then this is single user mode.just increment the phase and  re-update
	 ((game_dialog*)gd)->phase++;
	//update status
	update_gd_with_event_firing(gd);
}
//callback function for "back" button
void gd_cb_back(void* gd){
	switch(((game_dialog*)gd)->phase){
	case GUI_GAME_MODE:
		//fire the event which will make us return to main.
		fire_user_event(BACK_TO_MAIN,NULL,NULL);
		return;
	case GUI_DIFFICULTY:
		((game_dialog*)gd)->phase = GUI_GAME_MODE;
		((game_dialog*)gd)->difficulty = DIFF_NOT_SET;
		break;
	case GUI_USER_COLOR:
		((game_dialog*)gd)->phase = GUI_DIFFICULTY;
		((game_dialog*)gd)->difficulty = DIFF_NOT_SET;
		((game_dialog*)gd)->color = CLR_NOT_SET;
		break;
	}
	//update status
	update_gd_with_event_firing(gd);
}
//callback function for the start button
void gd_cb_start(void* gd){
	game_dialog* dialog = (game_dialog*)gd;
	dialog->render_needed = true;
	game_board* game ;
	if(dialog->game_mode == MULTI_PLAYER){
		 game = create_game(5);
		if(game){
			game->user_mode = MULTI_PLAYER_GAME;
			fire_user_event(START_GAME,game,NULL);
		}else{
			fprintf(stderr,"ERROR: A PROBLEM OCCURED WHILE CREATING BOARD AT GUI \n");
			fire_user_event(FAILURE,NULL,NULL);
		}
		return;
	}
	game = create_game(dialog->difficulty+1);
	if(game){
		game->p_type = (dialog->color == GUI_WHITE_PLAYER)?HUMAN:PC;
		game->user_mode = SINGLE_PLAYER_GAME;
		fire_user_event(START_GAME,game,NULL);
	}else{
		fprintf(stderr,"ERROR: A PROBLEM OCCURED WHILE CREATING BOARD AT GUI \n");
		fire_user_event(FAILURE,NULL,NULL);
	}
}
//callback functions for the difficulties:
void gd_cb_noob(void* gd){
	((game_dialog*)gd)->difficulty = NOOB;
	//update status
	update_gd_with_event_firing(gd);
}
void gd_cb_easy(void* gd){
	((game_dialog*)gd)->difficulty = EASY;
	//update status
	update_gd_with_event_firing(gd);
}
void gd_cb_moderate(void* gd){
	((game_dialog*)gd)->difficulty = MODERATE;
	//update status
	update_gd_with_event_firing(gd);
}
void gd_cb_hard(void* gd){
	((game_dialog*)gd)->difficulty = HARD;
	//update status
	update_gd_with_event_firing(gd);
}
void gd_cb_expert(void* gd){
	((game_dialog*)gd)->difficulty = EXPERT;
	//update status
	update_gd_with_event_firing(gd);
}
//callback functions for the colors:
void gd_cb_black(void* gd){
	((game_dialog*)gd)->color = GUI_BLACK_PLAYER;
	//update status
	update_gd_with_event_firing(gd);
}
void gd_cb_white(void* gd){
	((game_dialog*)gd)->color = GUI_WHITE_PLAYER;
	//update status
	update_gd_with_event_firing(gd);
}

void update_gd_with_event_firing(game_dialog* gd){
	if(update_gd_status(gd)!= GUI_SUCCESS){
		//push event with error, exit
		fprintf(stderr,"ERROR: A PROBLEM OCCURED WHILE HANDLING EVENT %s \n",SDL_GetError());
		fire_user_event(FAILURE,NULL,NULL);
	}
}
void destroy_gd(Window* w){
	if(!w){return;}
	game_dialog* gd = (game_dialog*)w->data;
	destroy_widgets_list(gd->widgets,gd->widget_num);
	//SDL_DestroyRenderer(gd->renderer);
//	if(gd->window){
//		SDL_DestroyWindow(gd->window);
//	}
	free(gd);
	free(w);
}
