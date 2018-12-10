/*
 * GUI_coomon.h
 *
 *  Created on: 25 Aug 2017
 *      Author: GalL
 */

#ifndef GUI_COMMON_H_
#define GUI_COMMON_H_
#include <stdbool.h>
#include <SDL.h> //SDL
#include <SDL_video.h>
#define GUI_RED = {255,0,0}
typedef enum gui_message_t{
	GUI_SUCCESS,
	GUI_FAILURE,
	GUI_FS_FAILURE,
	GUI_INVALID_ARGUMENT,
	GUI_SDL_FAILURE,
	GUI_QUIT
}GUI_MESSAGE;
typedef enum chess_event_code_t{
	BACK_TO_MAIN,
	BACK_TO_PREV,
	START_GAME,
	LOAD_REQUESTED,
	GAME_SETTINGS,
	FAILURE
}CHESS_EVENT_CODE;
typedef enum user_modes_t{
	SINGLE_PLAYER ,
	MULTI_PLAYER,
	MODE_NOT_SET
}USER_MODES;
typedef enum rgb_colors_t{
	BACKGROUND_RGB_R = 78,
	BACKGROUND_RGB_G = 152,
	BACKGROUND_RGB_B = 110
}RGB_COLORS;

/**
 * Fires a user event to the SDL Event Queue.
 * @param code - the code for the event to be fired.
 * @param data1 - the pointer to assign to the created event user.data1 field
 * @param data2 - the pointer to assign to the created event user.data2 field
 * @return -void
 */
void fire_user_event(CHESS_EVENT_CODE code,void* data1,void* data2);
#endif /* GUI_COMMON_H_ */


