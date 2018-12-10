/*
 * Window.c
 *
 *  Created on: 31 Aug 2017
 *      Author: GalL
 */
#include "Window.h"
void destroy_window(Window* w){
	if(w){
		w->destroy(w);
	}
}

