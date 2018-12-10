/*
 * Window.h
 *
 *  Created on: 31 Aug 2017
 *      Author: GalL
 */

#ifndef WINDOW_H_
#define WINDOW_H_
#include "Widget.h"
typedef struct window_t Window;
struct window_t{
	//used as the actual window pointer (main_window etc.)
	void* data;
	GUI_MESSAGE (*draw)(Window*);
	void (*handle_event)(Window* ,SDL_Event*); // handles event
	void (*destroy)(Window* );//destroys the widget
} ;

/**
  * Destroys a window - NULL SAFE.
  * @param w  - Pointer to a window to destroy.
  * @return void
  * @action - frees resources used by the window, by means of calling it's destroy function.
  */
void destroy_window(Window* w);

#endif /* WINDOW_H_ */
