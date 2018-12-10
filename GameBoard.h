/*
 * GameBoard.h
 *
 *  Created on: 4 Aug 2017
 *      Author: GalL
 */

#ifndef GAMEBOARD_H_
#define GAMEBOARD_H_
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "common.h"
#include "chess_parser.h"
#define GAME_BOARD_SIZE 8
#define GAME_BOARD_HIST_SIZE 6
#define WHITE 'W'
#define BLACK 'B'



// check if a tool can move to the desired position based on tool_type function form
typedef valid_move (*check_valid_func)(game_board*,position,position);

/**
 * the function check whethers a move is valid,
 * which means after making it the current player is not under check
 * @param board					 game board
 * @param from to				 the position from and to for the move
 * @param valid_func			 each tool has it's unique function to determine legit moves
 *
 * @return
 * returns diagonal/ straight move and direction
 */
valid_move valid_move_for_tool(game_board* board, position from, position to, check_valid_func valid_func);

/**
 * Creates a game_board, returns a pointer to it.
 * @param difficulty - the difficulty level
 * @return - on success - pointer to a game_board. on failure - NULL.
 */
game_board* create_game(int difficulty);

/**
 * Copies a game_board, without changing it's input.
 * @param src - pointer to the origin game board.
 * @return - on success - pointer to the copied game_board. on failure - NULL.
 */
game_board* copy_game(game_board* src);

/**
 * Initializes the tools in the board.
 * @param src - pointer to the origin game board.
 * @return - void
 */
void init_game_board(game_board* src);

/**
 * Destroys a game_board
 * @param src - pointer to the  game board.
 * @return - void
 */
void destroy_game(game_board* src);

/**
 * Destroys a rollback struct.
 * @param src - pointer to the rollback struct.
 * @return - void
 */
void destroy_rollback(rollback* src);

/**
 * creates a rollback struct.
 * @param t1_orig - the original position of the moving tool
 * @param t2_orig  - the original content of the tools destination.
 * @return - on success, pointer to the resultant rollback struct, else- NULL.
 */
rollback* create_rollback(tool_pos t1_orig,tool_pos t2_orig);

/**
 * Copies a rollback struct, without changing it's content.
 * @param src - pointer to the original rollback struct
 * @return - on success, pointer to the copies rollback struct, else- NULL.
 */
rollback* copy_rollback(rollback* src);

/**
 * Undo's the last move done on the board.
 * @param src - pointer to the game board.
 * @return - on success, CHESS_STATUS_SUCCESS. O/W - an appropriate CHESS_STATUS
 * informative of the failure(s) encountered.
 */
CHESS_STATUS undo_move(game_board* src);


/**
 * On success, the function prints the board game. If an error occurs, then the
 * function does nothing. The capital letters represents the black tools
 * lower case represents white tool.
 * tools definition characters can be found in game_tools struct.
 *
 * @param src - the target game
 * @return
 * CHESS_STATUS_INVALID_ARGUMENT - if src==NULL
 * CHESS_STATUS_SUCCESS - otherwise
 *
 */
CHESS_STATUS print_board_console(game_board* src);

/**
 * Performs a move
 * @param src - pointer to the game board.
 * @param t1_orig - the original position of the moving tool
 * @param t2_orig  - the original content of the tools destination.
 * @return - on success, CHESS_STATUS_SUCCESS, O/W
 * informative of the failure(s) encountered.
 */
CHESS_STATUS set_move(game_board* src,tool_pos t1_orig,tool_pos t2_orig);

/**
 * Adds an undo entry to the games undo space (Implemented using a Circular Array)
 * @param src - pointer to the game board.
 * @param t1_orig - the original position of the moving tool
 * @param t2_orig  - the original content of the tools destination.
 * @param scw - the value of short_castle_white BEFORE the move.
 * @param scb - the value of short_castle_black BEFORE the move.
 * @param lcw - the value of long_castle_white BEFORE the move.
 * @param lcb - the value of long_castle_black BEFORE the move.
 * @return - void
 */
void add_undo_entry(game_board* src,tool_pos t1_orig,tool_pos t2_orig,bool scw,bool scb,bool lcw,bool lcb);

//int is_valid(game_board* src,int curr_x,int curr_y,int dest_x,int dest_y);

/**
 * checks if there exists a tool belonging to col's oponent which can catch col's king,
 * in the context of the input game (src)
 * @param src - the target game , col - the color for which to determine if there is a check.
 * @return
 * -1 if an error occured
 * 0 if no threat exists, 1 if there exists a threat (i.e - a tool that can catch pos)
 */
bool is_check(game_board* src,player_color col);
//checks the game state - CHESS_STATUS_NORMAL if !(mate||tie), appropriate message for mate/tie, and errors - if such arise.

/**
 * Checks the game state
 * @param src - pointer to the game board.
 * @return - the boards status, or an appropriate CHESS_STATUS informative of the failure(s) encountered.
 */
CHESS_STATUS game_state(game_board* src);

/**
 * checks if the position pos represents a tool that can be caught,
 * in the context of the input game (src)
 * @param src - the target game , pos - the position to check.
 * @return true if and only  a threat exists.O/W false.
 */
bool is_catchable(game_board* src,position pos);

/**
 * checks if the tool can be eaten after a move
 * in the context of the input game (src)
 * @param src - the target game , pos - the position to check.
 * @param from,to - position of the tool being moves
 * @return
 * -1 if an error occured
 * 0 if no threat exists, 1 if there exists a threat (i.e - a tool that can catch pos)
 */
bool is_catchable_after_move(game_board* src,position from, position to);

/**
 * IMPLEMENTS THE MATHEMATICAL MODULO OPERATOR
 * @param a,b
 * @return
 * MOD(a,b) where MOD is the MATHEMATICAL DEFINITION OF MODULO. O(1) running time.
 */
int modulo(int a,int b);

/**
 * updates white_tools and black_tools
 * @param src - pointer to a game_board
 * ACTION - update white_tools and black_tools according to the
 * current location on the board.
 */
void update_tools_array(game_board* src);

/**
 * checks if the move defined by moving the tool in p1 to p2 will not cause a check
 * @param src - the target game , p1,p1 - target positions
 * @return
 * -1 if given null game
 * 0 if the move will not result in check, 1 if the move will result in check
 */
bool is_move_valid(game_board* src,position p1,position p2);

/**
 * CLONES A GAME BOARD AND CHANGES THE HISTORY SIZE.
 * @param src, new_hist_size
 * @return if src!= NULL AND new_hist_size >src->hist_size AND ALL operation are successful -
 * then a  pointer to a new  game board is returned, which is a clone of src but with new_hist_size.
 * O.W - NULL
 */
game_board* copy_change_history(game_board* src,int new_hist_size);

/**
 * The function checks all the valid moves for a specific tool in a given game board and returns
 * an array of the valid move positions - AFTER SCREENING THE ONES THAT RESULT IN CHECK.
 *
 * @param board - game being queried
 * @param from - position of the tool being queried (tool type can be queried from this information)
 * @param is_valid_move_tool_type - the is_valid_type function (see above)
 *
 * @return
 * returns an array of all the valid moves, the number of valid moves, and the number
 * of moves that result in capturing an enemy
 */
legit_moves* valid_moves_array(game_board* board, position from,
		valid_move (*is_valid_move_tool_type)(game_board* board, position from, position to));

/**
 * checks if the position pos represents a tool that can be caught,ASSUMING THE TOOL IS NOT A KING
 * in the context of the input game (src)
 * @param src - the target game , pos - the position to check.
 * @return true if and only  a threat exists. O/W false.
 */
bool is_catchable_reg(game_board* src,position pos);

/**
 * checks if the position pos represents a tool that can be caught,ASSUMING THE TOOL IS A KING
 * in the context of the input game (src)
 * @param src - the target game , pos - the position to check.
 * @return true if and only  a threat exists. O/W false.
 */
bool is_catchable_king(game_board* src,position pos);

/*
 * helper function to detect castle moves for both rock and king.
 */
CASTLE_TYPE handle_castle_moves(position from,
						char right_castle_buffer[MAX_LINE_LENGTH],
						char left_castle_buffer[MAX_LINE_LENGTH], game_board* game,
						position to);

#endif /* GAMEBOARD_H_ */
