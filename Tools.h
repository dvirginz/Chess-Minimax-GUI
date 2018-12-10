/*
 * Tools.h
 *
 *  Created on: Aug 4, 2017
 *      Author: dvginzbu
 */

#ifndef TOOLS_H_
#define TOOLS_H_

#include <stdint.h>
#include <stdbool.h>
#include "common.h"
#include "GameBoard.h"




typedef enum straight_t{
	FORWARD,
	BACKWORD,
	RIGHT,
	LEFT,
	NOT_STRAIGHT
}STRAIGHT;

typedef enum diagonal_t{
	FORWARD_R,
	FORWARD_L,
	BACKWORD_R,
	BACKWORD_L,
	NOT_DIAGONAL,
}DIAGONAL;





/**
 * Both diagonal and straight direction functions parse the x,y offsets and evaluate whether
 * it is a straight/diagonal move and direction (UP/DOWN etc.)
 *
 * @param offset_x offset_y - by_axis gap between from and to positions
 *
 * @return
 * returns diagonal/ straight move and direction
 */
DIAGONAL diagonal_direction(int offset_x, int offset_y);
STRAIGHT straight_direction(int offset_x, int offset_y);

/**
 * Both diagonal and straight valid functions check whether the move exits the board borders(INVALID)
 *
 * @param board - game being queried
 * @param from - position of the tool being queried
 * @param direction - direction of movement(see STRAIGHT struct)
 * @param amount - offset gap
 *
 * @return
 * returns if it is a valid diagonal/ straight move and direction
 */
valid_move is_valid_straight(game_board* board, position from, STRAIGHT direction, int amount);

valid_move is_valid_diagonal(game_board* board, position from, DIAGONAL direction, int amount);




/**
 * All is_valid tool functions checks whether the move is legal
 * depending on the tool type (see chess rules for per-tool move rules)
 *
 * @param board - game being queried
 * @param from - position of the tool being queried (tool type can be queried from this information)
 * @param to - place the tool desired to go to.
 *
 * @return
 * returns if it is a valid move depending on the tool type.
 */
valid_move is_valid_queen(game_board* board, position from, position to);

valid_move is_valid_king(game_board* board, position from, position to);

valid_move is_valid_pawn(game_board* board, position from, position to);

valid_move is_valid_bishop(game_board* board, position from, position to);

valid_move is_valid_rook(game_board* board, position from, position to);

valid_move is_valid_knight(game_board* board, position from, position to);

/**
 * The function checks all the valid moves for a specific tool in a given game board and returns
 * an array of the valid move positions
 *
 * @param board - game being queried
 * @param from - position of the tool being queried (tool type can be queried from this information)
 * @param is_valid_move_tool_type - the is_valid_type function (see above)
 *
 * @return
 * returns an array of all the valid moves, the number of valid moves, and the number
 * of moves that result in capturing an enemy
 */
legit_moves* legit_moves_array(game_board* board, position from,
		valid_move (*tool_type)(game_board* board, position from, position to));
void destroy_legit_moves(legit_moves* src);
//checks if pos is a valid target for a move in src

//checks if pos is a valid target for a move in src
bool is_pos_valid(legit_moves* src,position pos);
/**
 * The function check all the valid moves for a specific tool in a given game board and returns
 * an array of the valid move positions
 *
 * @param board - game being queried
 * @param from - position of the tool being queried (tool type can be queried from this information)
 * @param is_valid_move_tool_type - the is_valid_type function (see above)
 *
 * @return
 * returns an array of all the valid moves, the number of valid moves, and the number
 * of moves that result in capturing an enemy
 */
//legit_moves legit_moves_array(game_board* board, position from,
//		valid_move (*is_valid_move_tool_type)(game_board* board, position from, position to));


/**
 * helper function to check if player_1 can move to player_2 position
 *
 * @param player_1, player_2 - player types are represented via lower(white)/ upper(black) letters
 *
 * @return
 * true if opposite player, false else
 */
bool opposite_players(char player_1, char player_2);
int compare_full_moves(const void* t1,const void* t2);

/**
 * parse tool type and returns the right function to check for valid move (according to tool move rules)
 */
check_valid_func get_validator(game_tools tool);

/**
 * identify which castle is the move depending on the king position and the goto position of the king
 */
void identify_castle_from_move(bool is_castle, bool* short_castle_white, bool* short_castle_black,
		bool* long_castle_white, bool* long_castle_black, position from, position to);


/**
 * print the move as defined
 */
void print_move_of_tool(game_board* board, position from, position to);

/**
 * print the promotion for the user
 */
void print_promotion_from_pc(position from, position to, game_tools tool_promoted);

/**
 * helper function for user to define which tool to promote
 */
game_tools tool_for_promotion(game_board* board);

/**
 * hard coded helper for insertion of possible tools after promotion
 */
void insert_possible_promotions_white(tool_pos availible_pos[BOARD_SIZE*BOARD_SIZE*3], int* num_pos, position to);
void insert_possible_promotions_black(tool_pos availible_pos[BOARD_SIZE*BOARD_SIZE*3], int* num_pos, position to);
#endif /* TOOLS_H_ */
