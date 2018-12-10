/*
 * evaluation.h
 *
 *  Created on: Aug 11, 2017
 *      Author: dvginzbu
 */

#ifndef EVALUATION_H_
#define EVALUATION_H_

#include "common.h"
#include "GameBoard.h"

// All the evaluation constants were taken from the starfish algorithm chess program.
static const short pawn_table[8][8] = {
	     {0,  0,  0,  0,  0,  0,  0,  0},
	    {50, 50, 50, 50, 50, 50, 50, 50},
	    {10, 10, 20, 30, 30, 20, 10, 10},
	     {5,  5, 10, 27, 27, 10,  5,  5},
	     {0,  0,  0, 25, 25,  0,  0,  0},
	     {5, -5,-10,  0,  0,-10, -5,  5},
	     {5, 10, 10,-25,-25, 10, 10,  5},
	     {0,  0,  0,  0,  0,  0,  0,  0}
	};

static const short knight_table[8][8] = {
    {-50,-40,-30,-30,-30,-30,-40,-50},
    {-40,-20,  0,  0,  0,  0,-20,-40},
    {-30,  0, 10, 15, 15, 10,  0,-30},
    {-30,  5, 15, 20, 20, 15,  5,-30},
    {-30,  0, 15, 20, 20, 15,  0,-30},
    {-30,  5, 10, 15, 15, 10,  5,-30},
    {-40,-20,  0,  5,  5,  0,-20,-40},
    {-50,-40,-20,-30,-30,-20,-40,-50},
};

static const short bishop_table[8][8] =
{
    {-20,-10,-10,-10,-10,-10,-10,-20},
    {-10,  0,  0,  0,  0,  0,  0,-10},
    {-10,  0,  5, 10, 10,  5,  0,-10},
    {-10,  5,  5, 10, 10,  5,  5,-10},
    {-10,  0, 10, 10, 10, 10,  0,-10},
    {-10, 10, 10, 10, 10, 10, 10,-10},
    {-10,  5,  0,  0,  0,  0,  5,-10},
    {-20,-10,-40,-10,-10,-40,-10,-20},
};

static const short rook_table[8][8] =
{
	{0,  0,  0,  0,  0,  0,  0,  0},
	{5, 10, 10, 10, 10, 10, 10,  5},
	{-5,  0,  0,  0,  0,  0,  0, -5},
	{-5,  0,  0,  0,  0,  0,  0, -5},
	{-5,  0,  0,  0,  0,  0,  0, -5},
	{-5,  0,  0,  0,  0,  0,  0, -5},
	{-5,  0,  0,  0,  0,  0,  0, -5},
	{0,  0,  0,  5,  5,  0,  0,  0}
};

static const short queen_table[8][8] =
{
	{-20,-10,-10, -5, -5,-10,-10,-20},
	{-10,  0,  0,  0,  0,  0,  0,-10},
	{-10,  0,  5,  5,  5,  5,  0,-10},
	{-5,  0,  5,  5,  5,  5,  0, -5},
	{ 0,  0,  5,  5,  5,  5,  0, -5},
	{-10,  5,  5,  5,  5,  5,  0,-10},
	{-10,  0,  5,  0,  0,  0,  0,-10},
	{-20,-10,-10, -5, -5,-10,-10,-20}
};

static const short king_table_start_game[8][8] =
{
  {-30, -40, -40, -50, -50, -40, -40, -30},
  {-30, -40, -40, -50, -50, -40, -40, -30},
  {-30, -40, -40, -50, -50, -40, -40, -30},
  {-30, -40, -40, -50, -50, -40, -40, -30},
  {-20, -30, -30, -40, -40, -30, -30, -20},
  {-10, -20, -20, -20, -20, -20, -20, -10},
  { 20,  20,   0,   0,   0,   0,  20,  20},
   {20,  30,  10,   0,   0,  10,  30,  20}
};

static const short king_table_end_game[8][8] =
{
    {-50,-40,-30,-20,-20,-30,-40,-50},
    {-30,-20,-10,  0,  0,-10,-20,-30},
    {-30,-10, 20, 30, 30, 20,-10,-30},
    {-30,-10, 30, 40, 40, 30,-10,-30},
    {-30,-10, 30, 40, 40, 30,-10,-30},
    {-30,-10, 20, 30, 30, 20,-10,-30},
    {-30,-30,  0,  0,  0,  0,-30,-30},
    {-50,-30,-30,-30,-30,-30,-30,-50}
};

typedef enum material_score_t{
	pawn_start = 95, pawn_end = 115,
	knight_start = 300, knight_end = 340,
	bishop_start = 310, bishop_end = 345,
	rook_start = 480, rook_end = 520,
	queen_start = 970, queen_end = 1000,
	king_start = 10000, king_end = 10000,
	pawn_naive = 1, knight_naive = 3,
	bishop_naive = 3, rook_naive = 5,
	queen_naive = 9, king_naive = 100,
}material_value;

/**
 * The function will return the number of tools currently on the board
 *
 * @param board - the target game
 * @return
 * number of tools on board
 *
 */
int num_of_tools(game_board* board);

/**
 * The function will calculate tool's score on board depending on three parameters
 * a) material score (see material_score_t)
 * b) tool position on table
 * c) state of game (start<->end) (e.g pawn at the end of the game have more relevance than pawn at the start
 *
 * @param board - the target game
 * @param game_state - (num_of_tools / max_num_of_tools) - states how close the game is to ending
 *
 * @return
 * tool evaluation number
 *
 */
int evaluate_tool(game_tools tool,int game_state, position pos);

/**
 * The function will calculate tool's score on board depending on one parameter, the 'value' of the tool
 *
 * @param board - the target game
 *
 * @return
 * tool evaluation number
 *
 */
int evaluate_naive(game_tools tool);


/**
 * The function will calculate board score depending on all tools currently on it
 * black tools will have mirror(*-1) values to white tools, board evaluation can be negative
 *
 * @param board - the target game
 *
 * @return
 * board evaluation number, absolute higher score is better position for the relevant side.
 *
 */
int evaluate_board(game_board* board);




#endif /* EVALUATION_H_ */
