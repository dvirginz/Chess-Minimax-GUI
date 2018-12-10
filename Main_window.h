/*
 * Main_window.h
 *
 *  Created on: 31 Aug 2017
 *      Author: GalL
 */

#ifndef MAIN_WINDOW_H_
#define MAIN_WINDOW_H_
#include "Window.h"
#include "Widget.h"
typedef struct main_window_t main_window;
struct main_window_t{
	SDL_Window* window;
	SDL_Renderer* renderer;
	//simple widgets
	Widget** widgets;
	int widget_num;
	bool hidden; // this option is not used in the PRODUCTION version of the game, but is fully implemented.
};
/**
 * Creates a Window, where the data field is a pointer to a main_window, and returns a pointer to that Window.
 * @param window - Pointer to an SDL_Windpw to be used.
 * @param renderer - Pointer to an SDL_Renderer to be used.
 * @return - on success - pointer to a Window. on failure - NULL.
 */
Window* create_main_window(SDL_Window* w,SDL_Renderer* r);

/**
 * draws a main window
 * @param w - Pointer to a Window, for which the data field points to game_window.
 * @return - if succesfully drawn all necessary items, GUI_SUCCESS, O.W - an appropriate GUI_MESSAGE,
 * informative of the kind of failure.
 */
GUI_MESSAGE draw_mw(Window* w );

/**
 * manages the event handling for the main window, routing events to widgets.
 * @param w - Pointer to a Window, for which the data field points to game_window.
 * @param e - pointer to the SDL_Event that was fired.
 * @return - void
 */
void handle_mw_event(Window* w  ,SDL_Event* e);

/**
  * Destroys a main_window - NULL SAFE.
  * @param src  - Pointer to Window, for which the data field is a pointer to a main_window.
  * @return void
  * @action - frees resources, NOTE: the renderer and the SDL_Window are not destroyed!
  */
void destroy_mw(Window* w);

/**
 * Creates the widgets for the main window
 * @param r - Pointer to an SDL_Renderer
 * @return - on success - an "array" of pointers, each points to widget used in
 *  the main window, else NULL.
 */
Widget** create_mw_widgets(SDL_Renderer* r);

/**
 * Callback function for the new game button.
 */
void mw_cb_new_game(void* mw);

/**
 * Callback function for the load button.
 */
void mw_cb_load_button(void* mw);

/**
 * Callback function for the exit button.
 */
void mw_cb_exit(void* mw);
#endif /* MAIN_WINDOW_H_ */
