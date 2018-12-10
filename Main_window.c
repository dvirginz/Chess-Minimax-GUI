/*
 * Main_window.c
 *
 *  Created on: 31 Aug 2017
 *      Author: GalL
 */
#include "Main_window.h"
#include "Button.h"
#include <stdio.h>
#include <unistd.h>
#define MW_WIDGET_NUM 3
Widget** create_mw_widgets(SDL_Renderer* r){
	/** the following indices assigned are considered constants in the scope of this file.
	 * for code readability reasons they are not replaced with constants/enum values.
	 */
	if(!r){return NULL;}
	//change here to control amount of widgets.
	//int widget_num = 3;
	Widget** res = (Widget**)malloc(sizeof(Widget*)*MW_WIDGET_NUM);
	if(!res){
		fprintf(stderr,"MALLOC ERROR\n");
		return NULL;
	}
	//start creating the widgets
	//start game button
	res[0] = create_button(r,(SDL_Rect){.x = 75,.y = 50 , .w = 180, .h=40 },"./img/start_game_active.bmp","./img/start_game_inactive.bmp",mw_cb_new_game);
	//load game button
	res[1] = create_button(r,(SDL_Rect){.x = 75,.y = 90 , .w = 180, .h=40 },"./img/load_game_active.bmp","./img/load_game_inactive.bmp",mw_cb_load_button);
	//exit buton
	res[2] = create_button(r,(SDL_Rect){.x = 75,.y = 130 , .w = 240, .h=50 },"./img/exit.bmp","./img/exit.bmp",mw_cb_exit);
	for(int i = 0 ; i < MW_WIDGET_NUM ; i++){
		if(!res[i]){
			destroy_widgets_list(res,MW_WIDGET_NUM);
			fprintf(stderr,"ERROR CREATING WIDGETS AT MAIN WINDOW\n");
			return NULL;
		}
	}
	return res;
}
void handle_mw_event(Window* w  ,SDL_Event* e){
	if(!w||!e){return;}
	main_window* mw = (main_window*)(w->data);
	if(!mw->hidden){
		//if not hidden,let widgets know of the event. send the main window as extra data
		for(int i = 0 ; i < mw->widget_num;i++){
			mw->widgets[i]->handle_event(mw->widgets[i],e,(void*)mw);
		}
	}

}
Window* create_main_window(SDL_Window* window,SDL_Renderer* r){
	if(!window){return NULL;}
	Window* res = (Window*)malloc(sizeof(Window));
	//alloc space for the main_window itself
	main_window* data = (main_window*)malloc(sizeof(main_window));
	//create the window.
	//SDL_Window* window = SDL_CreateWindow("CHESS MASTER BY DVIR GINZBURG & GAL LUSHI",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,800,800,SDL_WINDOW_BORDERLESS);
	//SDL_Renderer* r = SDL_CreateRenderer(window,-1,SDL_RENDERER_TARGETTEXTURE || SDL_RENDERER_SOFTWARE);
	Widget** widgets = create_mw_widgets(r);
	if(!res || !data|| !widgets){
		free(data);
		free(res);
		destroy_widgets_list(widgets,MW_WIDGET_NUM);
		//SDL_DestroyWindow(window);
		//SDL_DestroyRenderer(r);
		return NULL;
	}
	data->hidden = false;
	data->widget_num = MW_WIDGET_NUM;
	data->widgets = widgets;
	data->renderer = r;
	data->window = window;
	res->data = data;
	res->destroy = destroy_mw;
	res->handle_event = handle_mw_event;
	res->draw = draw_mw;
	return res;
}

//callback functions:
void mw_cb_new_game(void* mw){
	if(!mw){return;}
	fire_user_event(GAME_SETTINGS,NULL,NULL);
}
void mw_cb_load_button(void* mw){
	if(!mw){return;}
	fire_user_event(LOAD_REQUESTED,NULL,NULL);
	return;
}
void mw_cb_exit(void* mw){
	if(!mw){return;}
	SDL_Event quit_event = {.type = SDL_QUIT};
	SDL_PushEvent(&quit_event);
}
void destroy_mw(Window* src){
	if(!src){return;}
	main_window* mw = (main_window*)src->data;
	destroy_widgets_list(mw->widgets,mw->widget_num);
	//SDL_DestroyWindow(mw->window);
	//SDL_DestroyRenderer(mw->renderer);
	free(mw);
	free(src);
}
GUI_MESSAGE draw_mw(Window* w ){
	if(!w){return GUI_INVALID_ARGUMENT;}
	main_window* mw = (main_window*)w->data;
	int fail_num = 0;
	fail_num += SDL_SetRenderDrawColor(mw->renderer,BACKGROUND_RGB_R,BACKGROUND_RGB_G,BACKGROUND_RGB_B,SDL_BLENDMODE_ADD);
	fail_num += SDL_RenderClear(mw->renderer);
	for(int i = 0 ; i < mw->widget_num ; i++){
		GUI_MESSAGE msg = (*mw->widgets[i]->draw)(mw->widgets[i]);
		if(msg != GUI_SUCCESS){
			fprintf(stderr,"ERROR DRAWING MW %s",SDL_GetError());
			return msg;
		}
	}
	SDL_RenderPresent(mw->renderer);
	if(fail_num!=0){
		fprintf(stderr,"ERROR DRAWING MW %s",SDL_GetError());
		return GUI_SDL_FAILURE;
	}
	return GUI_SUCCESS;
}
