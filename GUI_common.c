/*
 * GUI_common.c
 *
 *  Created on: 8 Sep 2017
 *      Author: GalL
 */
#include "GUI_common.h"
#include <unistd.h>
void fire_user_event(CHESS_EVENT_CODE code,void* data1,void* data2){
	//create and event and push it.
	SDL_Event evt;
	SDL_memset(&evt, 0, sizeof(evt));
	evt.type = SDL_USEREVENT;
	evt.user.code = code;
	evt.user.data1 = data1;
	evt.user.data2 = data2;
	SDL_PushEvent(&evt);
}

