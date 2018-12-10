/*
 * suggest_move.h
 *
 *  Created on: Aug 11, 2017
 *      Author: dvginzbu
 */

#ifndef SUGGEST_MOVE_H_
#define SUGGEST_MOVE_H_

#include "common.h"

typedef struct move_command_t{
	tool_pos from, to;
	int value,alpha,beta;
}MOVE_COMMAND;

//the function assumes it is not called on a winning board
/**
 * The function will suggest the relevant player a move according to the min-max algorithm
 * the algorithm uses "alpha-beta probing" for complexity enhancements.
 *
 * @param board - the target game
 * @param max_depth - how many min-max stages the algorithm will perform, higher is better but consumes
 * more calculation time
 *
 *
 * @return
 * the function will return a struct containing the suggested move (game tool position and where to)
 *
 */
MOVE_COMMAND suggest_move(game_board* board, int max_depth);

/**
 * Inner function with alpha beta proning and depth tracking, tries to maximize the best move for
 * the white player.
 *
 * @param board - the target game
 * @param depth - current recursion depth, this function on depth 0 evaluate the board according to evaluation function
 * @param alpha/beta - alpha/beta probing tracking
 *
 * @return
 * the function will return a struct containing the suggested move (game tool position and where to)
 *
 */
MOVE_COMMAND suggest_max_move_ab(game_board* board, int depth,int alpha,int beta);

/**
 * Inner function with alpha beta proning and depth tracking, tries to maximize the best move for
 * the black player.
 *
 * @param board - the target game
 * @param depth - current recursion depth, this function on depth 0 evaluate the board according to evaluation function
 * @param alpha/beta - alpha/beta probing tracking
 *
 * @return
 * the function will return a struct containing the suggested move (game tool position and where to)
 *
 */
MOVE_COMMAND suggest_min_move_ab(game_board* board, int depth,int alpha,int beta);



#endif /* SUGGEST_MOVE_H_ */
