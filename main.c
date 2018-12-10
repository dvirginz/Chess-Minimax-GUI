/*
 * main.c
 *
 *  Created on: Aug 5, 2017
 *      Author: dvginzbu
 */

#include "console_game_flow.h"
#include <string.h>
#include "gui_game_flow.h"
#include <SDL.h>
#include <SDL_video.h>

int main(int argc,char* argv[]){
	if(argc == 1){
		//exec in console mode
		do_gui_game_flow();
				 return 0;
	}
	char* arg = argv[1];
	if(strcmp(arg,"-c")==0){
		//exec in console mode
		return settings_phase();
	}else if (strcmp(arg,"-g")==0){
		//GUI mode
		 do_gui_game_flow();
		 return 0;
	}else{
		//exec in console mode
		do_gui_game_flow();
				 return 0;
	}
	return 0;
}
