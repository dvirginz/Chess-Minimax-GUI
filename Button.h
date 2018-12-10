/*
 * Button.h
 *
 *  Created on: 25 Aug 2017
 *      Author: GalL
 */

#ifndef BUTTON_H_
#define BUTTON_H_
#include "Widget.h"
typedef struct button_t Button;
 struct button_t{
	SDL_Texture* texture_active;
	SDL_Texture* texture_inactive;
	SDL_Renderer* renderer;
	SDL_Rect loc;
	bool is_active;
	GUI_MESSAGE (*set_active)(Widget*,bool);
	void (*handle)(void*);
};
 /**
  * Creates a Widget, where the data field is a pointer to a button.
  * @param renderer - Pointer to an SDL_Renderer to be used.
  * @param loc -an SDL_Rect that represents the buttons location.
  * @param image_active/image_inactive - the file names for the BMP
  *  files used as background when the button is active/inactive, respectively.
  * @param handle - pointer to a function that will be triggered when the button is pushed, if active.
  * @return - on success - pointer to a Widget. on failure - NULL.
  */
Widget* create_button(SDL_Renderer* renderer,SDL_Rect loc,const char* image_active,const char* image_inactive,void (*handle)(void*));
/**
  * Destroys a button- NULL SAFE.
  * @param src  - Pointer to Widget, for which the data field is a pointer to a button.
  * @return void
  */
void destroy_button(Widget* src);
/**
  * Sets a button active status.
  * @param src  - Pointer to Widget, for which the data field is a pointer to a button.
  * @param is_active - boolean, determines weather the button will be active or no.
  * @return GUI_SUCCESS on success, else - an appropriate GUI_MESSAGE.
  */
GUI_MESSAGE set_active(Widget* src,bool is_active);
/**
  * Draws a button to the screen, with the background image according to it's active state.
  * @param src  - Pointer to Widget, for which the data field is a pointer to a button.
  * @return GUI_SUCCESS on success, else - an appropriate GUI_MESSAGE.
  */
GUI_MESSAGE draw_button(Widget* src);
/**
  * Handles a button event.
  * @param src  - Pointer to Widget, for which the data field is a pointer to a button.
  * @param extra_data - a void* pointer, that will be passes to the button's
  *  handle function - if the button is active and was pressed. O.W - nothing is done.
  * @return void
  */
void handle_button_event(Widget* src,SDL_Event* e,void* extra_data);
#endif /* BUTTON_H_ */
