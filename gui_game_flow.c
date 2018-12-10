/*
 * gui_game_flow.c
 *
 *  Created on: 16 Sep 2017
 *      Author: GalL
 */
#include "gui_game_flow.h"
#include <SDL.h>
#include <SDL_video.h>
#include "Game_window.h"
#include "Game_dialog.h"
#include "Load_window.h"
#include "Main_window.h"
#include <sys/stat.h>
#include <sys/types.h>
#define DIR_NAME "./svg"
static bool loop = true;
static bool game_running = false;
static int exit_code = 0;
static Window* curr_game_window;
static Window* curr_window;
static SDL_Window* window;
SDL_Renderer* r;
static game_board* game;
static SDL_Event e;
void do_gui_game_flow(){
	if(SDL_Init(SDL_INIT_VIDEO) != 0){//initialize SDL
		fprintf(stderr,"ERROR LOADING SDL %s",SDL_GetError());
		SDL_Quit();
		exit(127);
	}
	window = SDL_CreateWindow("CHESS MASTER BY DVIR GINZBURG & GAL LUSHI",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,800,800,SDL_WINDOW_INPUT_FOCUS);
	r =  SDL_CreateRenderer(window,-1, 0);
	validate_dirs(); // create save directory if it does not exist.
	curr_window  = create_main_window(window,r);//create a main window
	curr_game_window = NULL;//game_window USED ONLY TO RETURN FROM LOAD.
	if(!curr_window || !window||!r){
		loop = false;
		exit_code = 127;
	}else{curr_window->draw(curr_window);}
	while(loop){//starting the event loop
		SDL_WaitEvent(&e);
		if(e.type == SDL_QUIT){
			break;
		}else if (e.type == SDL_USEREVENT){
			switch(e.user.code){
			case FAILURE://if we got a failure event, clean whatever possible and exit - the cleanup is done AFTER the loop.
				loop = false;
				exit_code = 127;
				break;
			case LOAD_REQUESTED:
				handle_load_requested();
				break;
			case BACK_TO_MAIN:
				handle_back_to_main();
				break;
			case BACK_TO_PREV://ONLY THE LOAD WINDOW MAY FIRE THIS EVENT.
				handle_back_to_prev();
				break;
			case START_GAME:
				handle_start_game();
				break;
			case GAME_SETTINGS:
				handle_game_settings();
				break;
			}
			if(curr_window){
				curr_window->draw(curr_window);
			}
		}
		else if(curr_window){
				curr_window->handle_event(curr_window,&e);//else - let the window handle the event.
		}
	}
	if(exit_code != 0 ){fprintf(stderr,"AN ERROR OCCURED \n");}
	destroy_window(curr_window);
	if(curr_game_window){destroy_window (curr_game_window);}
	SDL_Quit();
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(r);
	exit(exit_code);
}
void handle_back_to_prev(){
	destroy_window(curr_window);
	if(game_running){
		curr_window = curr_game_window;
		//SDL_ShowWindow(((game_window*)(curr_game_window->data))->window);
		curr_game_window = NULL;
	}else{
		curr_window = create_main_window(window,r);
		if(!curr_window){//verify successful creation.
			loop = false;
			exit_code = 127;
		}
	}
}
void handle_back_to_main(){
	destroy_window(curr_window);
	game_running = false;
	curr_window = create_main_window(window,r);
	if(!curr_window){//verify successful creation.
		loop = false;
		exit_code = 127;
	}
}
void handle_load_requested(){
	if(game_running){
		curr_game_window = curr_window;
		//SDL_HideWindow(((game_window*)(curr_game_window->data))->window);
	}else{destroy_window(curr_window);}
	curr_window = create_load_window(window,r);
	if(!curr_window){//verify successful creation.
		loop = false;
		exit_code = 127;
	}
}
void handle_start_game(){
	 destroy_window(curr_window);
	 game = (game_board*)(e.user.data1);
	 game_running = true;
	 curr_window = create_game_window(window,r,game);
	if(!curr_window){//verify successful creation.
		loop = false;
		exit_code = 127;
	}
}
void handle_game_settings(){
	destroy_window(curr_window);
	curr_window = create_dialog(window,r);
	if(!curr_window){//verify successful creation.
		loop = false;
		exit_code = 127;
	}
}
void validate_dirs(){
	struct stat sb;
	if(stat(DIR_NAME,&sb) != 0 || !(S_ISDIR(sb.st_mode))){
		//the folder does not exist.
		if(mkdir("./svg",S_IRWXU) != 0){ // mkdir("./svg",S_IRWXU)
			fprintf(stderr,"ERROR CREATING DIRECTORIES FOR SAVE\n"); // no need to crash for this.
		}
	}
}

