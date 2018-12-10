/*
 * console_game_flow.h
 *
 *  Created on: Aug 13, 2017
 *      Author: dvginzbu
 */

#ifndef CONSOLE_GAME_FLOW_H_
#define CONSOLE_GAME_FLOW_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "common.h"
#include "chess_parser.h"
#include "GameBoard.h"

/**
 * runs the game in console mode
 * operator, parse input from user and calls functions
 *
 */

typedef enum WINNIER_STATE_E{
	WINNER_WHITE,
	WINNER_BLACK,
	TIE,
	NO_WINNER
}WINNER_STATE;


/**
 * The function controls the settings phase of the console game mode
 */
int settings_phase();

/**
 * Helper function that handles the message buffer creation.
 */
char* start_settings();

/**
 * Helper function that handles the game_mode change in the settings phase.
 */
void game_mode_message(COMMAND_s command,int* game_mode);

/**
 * Helper function that handles the difficulty change in the settings phase.
 */
void difficulty_message(COMMAND_s command,int* difficulty, int game_mode);

/**
 * Helper function that handles the user_color change in the settings phase.
 */
void user_color_message(COMMAND_s command,int* user_color, int game_mode);

/**
 * Helper function that loads a stored game from the game's folder
 * in the settings phase, after a load call, a start call should be called to start
 * a new game.
 */
void load_message(COMMAND_s command,int* game_mode,int* difficulty, int* user_color,game_board** game);

/**
 * Helper function that handles for changing all settings to default
 * Can be called from the settings phase.
 * default values are difficulty = 2 game_mode = one player(1) user color = white(1)
 */
void default_message(COMMAND_s command,int* game_mode,int* difficulty, int* user_color);

/**
 * Helper function to print the current settings in the settings phase.
 */
void print_settings_message(COMMAND_s command,int game_mode,int difficulty, int user_color);

/**
 * Helper function to quit the game from the settings phase.
 */
void quit_message(COMMAND_s command, char* message, game_board* game);

/**
 * Helper function to start the game from the settings phase.
 * The function will test for loaded game, and if not, create a starting chess game.
 */
void start_message(COMMAND_s command, int game_mode,int difficulty,int user_color,game_board* game);

/**
 * Helper function to control the game in one player mode, where no suggest moves are called.
 */
CHESS_STATUS one_player_game_phase(game_board* game,int user_color);

/**
 * Helper function to manage the game in the players stage
 */
CHESS_STATUS user_turn_manager(game_board* game,int game_mode, player_color p_color);

/**
 * Helper function to manage the game in the pc stage
 */
CHESS_STATUS pc_turn_manager(game_board* game);

/**
 * Helper function to control the game in two players mode, where no suggest moves are called.
 */
void two_player_game_phase(game_board* game);

/**
 * Helper function to handle the move login in the game phase
 */
CHESS_STATUS move_message(COMMAND_s command,game_board* game);

/**
 * Helper function to handle the castle login in the game phase
 */
CHESS_STATUS castle_message(COMMAND_s command,game_board* game);

/**
 * Helper function to handle the castle login in the game phase
 */
CHESS_STATUS get_moves_message(COMMAND_s command,game_board* game);

/**
 * Helper function to handle the save mechanism in the game phase
 */
CHESS_STATUS save_message(COMMAND_s command,game_board* game,int game_mode, player_color p_color);

/**
 * Helper function to handles the undo mechanism in the game phase
 */
CHESS_STATUS undo_message(COMMAND_s command,game_board* game);

/**
 * Helper function that moves the player to the settings window
 */
CHESS_STATUS reset_message(COMMAND_s command,game_board* game);


#endif /* CONSOLE_GAME_FLOW_H_ */
