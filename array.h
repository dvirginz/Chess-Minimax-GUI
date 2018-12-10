/*
 * array.h
 *
 *  Created on: Aug 4, 2017
 *      Author: dvginzbu
 */

#ifndef ARRAY_H_
#define ARRAY_H_

#include <stdint.h>
#include "GameBoard.h"
#include "Tools.h"

/**
 * function initializes both tool arrays (white and black) for specific board to hold all tools
 * relevant for the start of the game only
 *
 * @return
 * void
 */
void init_tools_array(game_board* board);

/**
 * operator = implementation for position struct (helper function)
 *
 * @return
 * true if same position, false else
 */
bool compare_pos(position player1, position player2);

/**
 * operator = implementation for tool_pos struct (helper function)
 *
 * @return
 * true if same object, false else
 */
bool compare_tools(tool_pos player1, tool_pos player2);

/**
 * On success, the function will return the index of the tool in the relevant board array
 *
 * @param board - the target game
 * @param tool -  the desired tool queried
 * @return
 * index > 0 - if tool found in one of the arrays
 * index == -1  otherwise
 *
 */
int get_tool_in_array(game_board* board, tool_pos tool);

/**
 * On success, the function will delete the tool from the array, the function will be called
 * after deleting the tool from the board
 *
 * @param board - the target game
 * @param tool -  the desired tool to delete
 * @return
 * void
 *
 */
void del_tool_array(game_board* board, tool_pos tool);

/**
 * On success, the function will add the tool to the relevant array
 * will be called only on pawn promotion move, after changing the pawn in the board
 *
 * @param board - the target game
 * @param tool -  the desired tool to add
 * @return
 * void
 *
 */
void add_tool_array(game_board* board, tool_pos tool);

#endif /* ARRAY_H_ */
