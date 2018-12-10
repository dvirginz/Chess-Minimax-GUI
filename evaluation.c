/*
 * evaluation.c
 *
 *  Created on: Aug 11, 2017
 *      Author: dvginzbu
 */


#include "evaluation.h"
int num_of_tools(game_board* board){
	int tools_in_game = 0;
	for (int i = 0; i < MAX_TOOLS; ++i) {
		(board->white_tools[i].tool_type != EMPTY_ENTRY) ? tools_in_game++ : 1;
		(board->black_tools[i].tool_type != EMPTY_ENTRY) ? tools_in_game++ : 1;
	}
	return tools_in_game;
}

int evaluate_tool(game_tools tool,int game_state, position pos){
	switch(tool){
		case WHITE_PAWN:
			return (pawn_start * game_state + (1-game_state) * pawn_end)
					+ (pawn_table[pos.row][pos.column]);
		case BLACK_PAWN:
			return ((pawn_start * game_state + (1-game_state) * pawn_end)
					+ (pawn_table[BOARD_SIZE + 1 -pos.row][BOARD_SIZE + 1 - pos.column]))  * (-1);
		case WHITE_QUEEN:
			return (queen_start * game_state + (1-game_state) * queen_end)
					+ (queen_table[pos.row][pos.column]);
		case BLACK_QUEEN:
			return ((queen_start * game_state + (1-game_state) * queen_end)
					+ (queen_table[BOARD_SIZE + 1 -pos.row][BOARD_SIZE + 1 - pos.column])) * (-1);
		case WHITE_ROOK:
			return rook_start * game_state + (1-game_state) * rook_end
					+ (rook_table[pos.row][pos.column]);
		case BLACK_ROOK:
			return ((rook_start * game_state + (1-game_state) * rook_end)
					+ (rook_table[BOARD_SIZE + 1 -pos.row][BOARD_SIZE + 1 - pos.column])) * (-1);
		case WHITE_BISHOP:
			return bishop_start * game_state + (1-game_state) * bishop_end
					+ (bishop_table[pos.row][pos.column]);
		case BLACK_BISHOP:
			return ((bishop_start * game_state + (1-game_state) * bishop_end)
					+ (bishop_table[BOARD_SIZE + 1 -pos.row][BOARD_SIZE + 1 - pos.column])) * (-1);
		case WHITE_KNIGHT:
			return ((knight_start * game_state + (1-game_state) * knight_end))
					+ (knight_table[pos.row][pos.column]);
		case BLACK_KNIGHT:
			return ((knight_start * game_state + (1-game_state) * knight_end)
					+ (knight_table[BOARD_SIZE + 1 -pos.row][BOARD_SIZE + 1 - pos.column])) * (-1);
		case WHITE_KING:
			return (king_start * game_state + (1-game_state) * king_end)
					+ ((king_table_start_game[pos.row][pos.column]* game_state) +
						(1-game_state) * king_table_end_game[pos.row][pos.column]);
		case BLACK_KING:
			return ((king_start * game_state + (1-game_state) * king_end)
					+ ((king_table_start_game[BOARD_SIZE + 1 -pos.row][BOARD_SIZE + 1 -pos.column]* game_state)
					+ (1-game_state) * king_table_end_game[BOARD_SIZE + 1 -pos.row][BOARD_SIZE + 1 -pos.column])) * (-1);
		default:
			return 0;
		}
}

int evaluate_naive(game_tools tool){
	switch(tool){
		case WHITE_PAWN:
			return pawn_naive;
		case BLACK_PAWN:
			return pawn_naive * (-1);
		case WHITE_QUEEN:
			return queen_naive;
		case BLACK_QUEEN:
			return queen_naive * (-1);
		case WHITE_ROOK:
			return rook_naive;
		case BLACK_ROOK:
			return rook_naive * (-1);
		case WHITE_BISHOP:
			return bishop_naive;
		case BLACK_BISHOP:
			return bishop_naive * (-1);
		case WHITE_KNIGHT:
			return knight_naive;
		case BLACK_KNIGHT:
			return knight_naive * (-1);
		case WHITE_KING:
			return king_naive;
		case BLACK_KING:
			return king_naive * (-1);
		default:
			return 0;
		}
}


int evaluate_board(game_board* board){
	int eval = 0;

	if(board->difficulty < 5){
		for (int i = 0; i < BOARD_SIZE; ++i) {
				for (int j = 0; j < BOARD_SIZE; ++j) {
					eval += evaluate_naive(board->board[i][j]);
				}
			}
	}else{
	//depending on number of tools in game, the state (start/end) of the game is decided.
	int game_state = num_of_tools(board) / (MAX_TOOLS+MAX_TOOLS);
	for (int i = 0; i < BOARD_SIZE; ++i) {
		for (int j = 0; j < BOARD_SIZE; ++j) {
			position pos = {i,j};
			eval += evaluate_tool(board->board[i][j],game_state,pos);
		}
	}
	}
	return eval;
}


