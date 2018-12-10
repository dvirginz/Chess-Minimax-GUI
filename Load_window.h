/*
 * Load_window.h
 *
 *  Created on: 15 Sep 2017
 *      Author: GalL
 */

#ifndef LOAD_WINDOW_H_
#define LOAD_WINDOW_H_
#include "Widget.h"
#include "Window.h"
typedef struct load_window_t load_window;
struct load_window_t{
	SDL_Window* window;
	SDL_Renderer* renderer;
	//simple widgets
	Widget** widgets;
	int widget_num;
	int current_choice;
	bool draw_needed;
};

/**
 * Creates a Window, where the data field is a pointer to a load_window, and returns a pointer to that Window.
 * @param window - Pointer to an SDL_Window to be used.
 * @param renderer - Pointer to an SDL_Renderer to be used.
 * @return - on success - pointer to a Window. on failure - NULL.
 */
Window* create_load_window(SDL_Window* window,SDL_Renderer* r);

/**
 * draws a load window, makes selection visible if the corresponding files exist.
 * @pre - the files created in ./svg directory are not touched by anyone except gw_save_current.
 * this assumption was approves by moab in the forum. this results in us not having
 * to sort the files by change date,since their indices provide a total order over the files,
 * thus the order is hard-coded and does not require run-time sorting.
 * @param w - Pointer to a Window, for which the data field points to game_dialog.
 * @return - if succesfully drawn all necessary items, GUI_SUCCESS, O.W - an appropriate GUI_MESSAGE,
 * informative of the kind of failure.
 */

GUI_MESSAGE draw_lw(Window* w );

/**
 * manages the event handling for the load_window, routing events to widgets.
 * @param w - Pointer to a Window, for which the data field points to load_window.
 * @param e - pointer to the SDL_Event that was fired.
 * @return - void
 */

void handle_lw_event(Window* w  ,SDL_Event* e);

/**
  * Destroys a load_window - NULL SAFE.
  * @param src  - Pointer to a Window, for which the data field is a pointer to a load_window.
  * @return void
  * @action - frees resources, NOTE: the renderer and the SDL_Window are not destroyed!
  */

void destroy_lw(Window* w);

/**
 * Creates the widgets for the load_dialog
 * @param r - Pointer to an SDL_Renderer
 * @return - on success - an "array" of pointers, each points to widget used in
 *  the load window, else NULL.
 */

Widget** create_lw_widgets(SDL_Renderer* r);

/**
 * Callback function for slot #1
 */
void lw_cb_slot1(void* lw);

/**
 * Callback function for slot #2
 */
void lw_cb_slot2(void* lw);

/**
 * Callback function for slot #3
 */
void lw_cb_slot3(void* lw);

/**
 * Callback function for slot #4
 */

void lw_cb_slot4(void* lw);

/**
 * Callback function for slot #5
 */

void lw_cb_slot5(void* lw);

/**
 * Callback function for the load button
 */
void lw_cb_load(void* lw);

/**
 * Callback function for the back button
 */
void lw_cb_back(void* lw);

#endif /* LOAD_WINDOW_H_ */
