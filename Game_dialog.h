/*
 * Game_dialog.h
 *
 *  Created on: 2 Sep 2017
 *      Author: GalL
 */

#ifndef GAME_DIALOG_H_
#define GAME_DIALOG_H_
#include "Window.h"
typedef enum game_dialog_phase_t{
	GUI_GAME_MODE,
	GUI_DIFFICULTY,
	GUI_USER_COLOR
}GAME_DIALOG_PHASE;

typedef enum game_dialog_diff_t{
	NOOB,
	EASY,
	MODERATE,
	HARD,
	EXPERT,
	DIFF_NOT_SET
}GAME_DIALOG_DIFFICULTIES;

typedef enum game_dialog_color_t{
	GUI_WHITE_PLAYER,
	GUI_BLACK_PLAYER,
	CLR_NOT_SET
}GAME_DIALOG_CLR;

typedef struct game_dialog_t game_dialog;
struct game_dialog_t{
	SDL_Window* window;
	SDL_Renderer* renderer;
	//parameters for the game
	USER_MODES game_mode;
	GAME_DIALOG_DIFFICULTIES difficulty;
	GAME_DIALOG_CLR color;
	GAME_DIALOG_PHASE phase;
	//simple widgets
	Widget** widgets;
	int widget_num;
	bool render_needed;
};
/**
 * Creates a Window, where the data field is a pointer to a game_dialog, and returns a pointer to that Window.
 * @param window - Pointer to an SDL_Window to be used.
 * @param renderer - Pointer to an SDL_Renderer to be used.
 * @return - on success - pointer to a Window. on failure - NULL.
 */
Window* create_dialog(SDL_Window* window,SDL_Renderer* r);

/**
 * Creates the buttons themselves, for the game_dialog
 * @param r - Pointer to an SDL_Renderer
 * @param ul,ll,ur,lr - SDL_Rect for the upper/lower left, upper/lower right (respectively) locations.
 * @param dest - the destination pointer "array"
 * @return - on success 0, O/W a negative number.
 */
int gd_create_buttons(SDL_Renderer* r,SDL_Rect ul,SDL_Rect ll,SDL_Rect ur,SDL_Rect lr,Widget** dest);

/**
 * draws a game dialog
 * @param w - Pointer to a Window, for which the data field points to game_dialog.
 * @return - if succesfully drawn all necessary items, GUI_SUCCESS, O.W - an appropriate GUI_MESSAGE,
 * informative of the kind of failure.
 */
GUI_MESSAGE draw_gd(Window* w );

/**
 * manages the event handling for the game_dialog, routing events to widgets.
 * @param w - Pointer to a Window, for which the data field points to game_dialog.
 * @param e - pointer to the SDL_Event that was fired.
 * @return - void
 */
void handle_gd_event(Window* w  ,SDL_Event* e);

/**
  * Destroys a game dialog - NULL SAFE.
  * @param src  - Pointer to a Window, for which the data field is a pointer to a game_dialog.
  * @return void
  * @action - frees resources, NOTE: the renderer and the SDL_Window are not destroyed!
  */
void destroy_gd(Window* w);

/**
 * Creates the widgets for the game_dialog
 * @param r - Pointer to an SDL_Renderer
 * @return - on success - an "array" of pointers, each points to widget used in
 *  the dialog, else NULL.
 */
Widget** create_gd_widgets(SDL_Renderer* r);

/**
 * Callback function for the single player choice button
 */
void gd_cb_one_player(void* gd);

/**
 * Callback function for the two player choice button
 */
void gd_cb_two_player(void* gd);

/**
 * Callback function for the "next" button
 */
void gd_cb_next(void* gd);

/**
 * Callback function for the "back" button
 */
void gd_cb_back(void* gd);

/**
 * Callback function for the "start" button
 */
void gd_cb_start(void* gd);

/**
 * Callback function for the noob difficulty selection button
 */
void gd_cb_noob(void* gd);

/**
 * Callback function for the easy difficulty selection button
 */
void gd_cb_easy(void* gd);

/**
 * Callback function for the moderate difficulty selection button
 */
void gd_cb_moderate(void* gd);

/**
 * Callback function for the hard difficulty selection button
 */
void gd_cb_hard(void* gd);

/**
 * Callback function for the expert difficulty selection button
 */
void gd_cb_expert(void* gd);

/**
 * Callback function for the black player selection button.
 */
void gd_cb_black(void* gd);

/**
 * Callback function for the white player selection button.
 */
void gd_cb_white(void* gd);

/**
 * Updates the screen in accordance with the game_dialog internal state
 * makes buttons active/inactive as necessary, shows/hides buttons as necessary.
 * @param gd - Pointer to a game_dialog
 * @return - on success, GUI_SUCCESS, else - an appropriate GUI_MESSAGE
 * informative of the failure.
 */
GUI_MESSAGE update_gd_status(game_dialog* gd);
/**
 * calls update_gd_status, if not successful - fires the FAILURE user event.
 * @param gd - Pointer to a game_dialog
 * @return - void
 */
void update_gd_with_event_firing(game_dialog* gd);

#endif /* GAME_DIALOG_H_ */
