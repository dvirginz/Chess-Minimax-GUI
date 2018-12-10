/*
 * common.h
 *
 *  Created on: Aug 10, 2017
 *      Author: dvginzbu
 */
#ifndef COMMON_H_
#define COMMON_H_
#include <stdbool.h>

#define MAX_STRING 1024
typedef enum chess_status_manager_t {
	CHESS_STATUS_INVALID_MOVE,
	CHESS_STATUS_INVALID_ARGUMENT,
	CHESS_STATUS_NO_HISTORY,
	CHESS_STATUS_MALLOC_ERROR,
	CHESS_STATUS_SUCCESS,
	CHESS_STATUS_WHITE_IN_MATE,
	CHESS_STATUS_BLACK_IN_MATE,
	CHESS_STATUS_TIE,
	CHESS_STATUS_NORMAL,
	CHESS_STATUS_RESTART,
//You may add any message you like
} CHESS_STATUS;

typedef enum CASTLE_TYPE_t{
	CASTLE_WHITE_RIGHT,
	CASTLE_WHITE_LEFT,
	CASTLE_BLACK_RIGHT,
	CASTLE_BLACK_LEFT,
	CASTLE_UNDEF,
}CASTLE_TYPE;

enum CONSTS{
	BOARD_MAX_INDEX = 7,
	BOARD_SIZE = 8,
	HIST_SIZE = 6,
	MAX_TOOLS = 16,
};

typedef struct position_t{
	int row;
	int column;
}position;
typedef enum GameTools_t{EMPTY_ENTRY = '_',WHITE_PAWN = 'm' ,WHITE_KING='k',WHITE_QUEEN='q',WHITE_BISHOP='b',\
	WHITE_ROOK='r',WHITE_KNIGHT='n',BLACK_PAWN = 'M',BLACK_KING='K',BLACK_QUEEN='Q',BLACK_BISHOP='B'\
	,BLACK_ROOK='R',BLACK_KNIGHT='N'
}game_tools;
typedef struct tool_pos_t{
	game_tools tool_type;
	position pos;
}tool_pos;
//enumerated type for the different tools that can be on the board, note that an empty slot
//is treated as EMPTY_ENTRY


typedef enum pc_or_player_t{
	HUMAN,
	PC,
}player_type;

typedef enum game_mode_t{
	CLI,
	GUI
}game_mode;
typedef enum user_mode_t{
	SINGLE_PLAYER_GAME = 1 ,
	MULTI_PLAYER_GAME = 2
}user_mode;
typedef enum player_t{
	WHITE_PLAYER = 'W',\
	BLACK_PLAYER = 'B'
} player_color;

//used to rollback moves.
typedef struct roll_back_t{
	tool_pos from;
	tool_pos to;
	bool scw, scb, lcw, lcb ;
	game_tools tool_to_if_castle;
}rollback;
//represents a game board
typedef struct game_board_t{
	game_tools** board;
	int hist_size;
	rollback** undo_data;
	int curr_hist_pos;
	int curr_hist_len;
	player_color curr_player;
	int difficulty;
	bool short_castle_white,short_castle_black,long_castle_white,long_castle_black;
	tool_pos* white_tools;
	tool_pos* black_tools;
	player_type p_type;
	game_mode game_mode;
	user_mode user_mode;
}game_board;

typedef struct moves_t{
	tool_pos* valid_pos;
	int moves_num;
	int enemy_tools;
}legit_moves;

typedef enum valid_t{
	VALID_MOVE,
	INVALID_MOVE,
	CASTLE_MOVE
}valid_move;

#endif /* COMMON_H_ */
