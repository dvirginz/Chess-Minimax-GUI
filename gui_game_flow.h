/*
 * gui_game_flow.h
 *
 *  Created on: 16 Sep 2017
 *      Author: GalL
 */

#ifndef GUI_GAME_FLOW_H_
#define GUI_GAME_FLOW_H_

/**
 * Performs the GUI game flow - including the initialization of SDL and the event loop.
 * @return - void
 */
void do_gui_game_flow();

/**
 * Validates the existence of the "./svg" directory. POSIX ONLY.
 * @return - void
 */
void validate_dirs();

/**
 * Handles the START_GAME event, by creating/destroying relevant windows.
 * @return - void
 */
void handle_start_game();

/**
 * Handles the SETTINGS  event, by creating/destroying relevant windows.
 * @return - void
 */
void handle_game_settings();

/**
 * Handles the LOAD_REQUESTED event, by creating/destroying relevant windows.
 * @return - void
 */
void handle_load_requested();

/**
 * Handles the BACK_TO_MAIN event, by creating/destroying relevant windows.
 * @return - void
 */
void handle_back_to_main();

/**
 * Handles the BACK_TO_PREV event, by creating/destroying relevant windows.
 * @return - void
 */
void handle_back_to_prev();
#endif /* GUI_GAME_FLOW_H_ */
