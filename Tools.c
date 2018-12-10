/*
 * Tools.c
 *
 *  Created on: Aug 4, 2017
 *      Author: dvginzbu
 */

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "Tools.h"

/*
 * All the hard coded numbers in this file refer to standard chess rules.
 * for further information please refer to the chess rules book.
 * for unique moves, such as castling and pawn promotion, added documentation can be find in the relevant function
 */

DIAGONAL diagonal_direction(int offset_column, int offset_row){
	// (-1) represents  that the offset_colum and offset_row are in opposite directions.

	if(offset_column == offset_row && offset_column > 0){
		return FORWARD_R;
	}else if(offset_column == ((-1) * offset_row) && offset_column > 0){
		return BACKWORD_R;
	}else if(offset_column == ((-1) * offset_row) && offset_column < 0){
		return FORWARD_L;
	}else if(offset_column ==  offset_row && offset_column < 0){
		return BACKWORD_L;
	}
	return NOT_DIAGONAL;
}

STRAIGHT straight_direction(int offset_column, int offset_row){
	if(offset_column == 0 && offset_row > 0){
		return FORWARD;
	}else if(offset_column == 0 && offset_row < 0){
		return BACKWORD;
	}else if(offset_column > 0 && offset_row == 0){
		return RIGHT;
	}else if(offset_column < 0 && offset_row == 0){
		return LEFT;
	}
	return NOT_STRAIGHT;
}

valid_move is_valid_straight(game_board* board, position from, STRAIGHT direction, int amount){
	switch(direction){
		case FORWARD:
		{
			if(from.row + amount > BOARD_MAX_INDEX)
				return INVALID_MOVE;
			for (int var = 1; var < amount; ++var) {
				if(board->board[from.row+var][from.column] != EMPTY_ENTRY)
					return INVALID_MOVE;
			}
			if(board->board[from.row+amount][from.column] == EMPTY_ENTRY||
					opposite_players(board->board[from.row][from.column], board->board[from.row + amount][from.column]))
				return VALID_MOVE;
			break;

		}
		case BACKWORD:
			{
				if(from.row - amount < 0)
					return INVALID_MOVE;
				for (int var = 1; var < amount; ++var) {
					if(board->board[from.row-var][from.column] != EMPTY_ENTRY)
						return INVALID_MOVE;
				}
				if(board->board[from.row-amount][from.column] == EMPTY_ENTRY||
									opposite_players(board->board[from.row][from.column], board->board[from.row - amount][from.column]))
					return VALID_MOVE;
				break;

			}
		case LEFT:
			{
				if(from.column - amount < 0)
								return INVALID_MOVE;
				for (int var = 1; var < amount; ++var) {
					if(board->board[from.row][from.column-var] != EMPTY_ENTRY)
						return INVALID_MOVE;
				}
				if(board->board[from.row][from.column-amount] == EMPTY_ENTRY||
													opposite_players(board->board[from.row][from.column], board->board[from.row][from.column - amount]))
									return VALID_MOVE;
				break;
			}
		case RIGHT:
			{
				if(from.column + amount > BOARD_MAX_INDEX)
						return INVALID_MOVE;
				for (int var = 1; var < amount; ++var) {
					if(board->board[from.row][from.column+var] != EMPTY_ENTRY)
						return INVALID_MOVE;
				}
				if(board->board[from.row][from.column+amount] == EMPTY_ENTRY||
													opposite_players(board->board[from.row][from.column], board->board[from.row][from.column+amount]))
					return VALID_MOVE;
			break;
			}
		default:
			return INVALID_MOVE;
	}
	return INVALID_MOVE;
}

valid_move is_valid_diagonal(game_board* board, position from, DIAGONAL direction, int amount){
	switch(direction){
			case FORWARD_R:
			{
				if(from.row + amount > BOARD_MAX_INDEX || from.column + amount > BOARD_MAX_INDEX)
					return INVALID_MOVE;
				for (int var = 1; var < amount; ++var) {
					if(board->board[from.row+var][from.column+var] != EMPTY_ENTRY)
						return INVALID_MOVE;
				}
				if(board->board[from.row+amount][from.column+amount] == EMPTY_ENTRY||
						opposite_players(board->board[from.row][from.column], board->board[from.row+amount][from.column+amount]))
					return VALID_MOVE;
				break;
			}
			case FORWARD_L:
				{
					if(from.column - amount < 0 || from.row + amount > BOARD_MAX_INDEX)
						return INVALID_MOVE;
					for (int var = 1; var < amount; ++var) {
						if(board->board[from.row+var][from.column-var] != EMPTY_ENTRY)
							return INVALID_MOVE;
					}
					if(board->board[from.row+amount][from.column-amount] == EMPTY_ENTRY||
																		opposite_players(board->board[from.row][from.column], board->board[from.row+amount][from.column-amount]))
						return VALID_MOVE;
					break;
				}
			case BACKWORD_R:
				{
					if(from.row - amount < 0 || from.column + amount > BOARD_MAX_INDEX)
									return INVALID_MOVE;
					for (int var = 1; var < amount; ++var) {
						if(board->board[from.row-var][from.column+var] != EMPTY_ENTRY)
							return INVALID_MOVE;
					}
					if(board->board[from.row-amount][from.column+amount] == EMPTY_ENTRY||
																		opposite_players(board->board[from.row][from.column], board->board[from.row-amount][from.column+amount]))
						return VALID_MOVE;
					break;
				}
			case BACKWORD_L:
				{
					if(from.row - amount < 0 || from.column - amount < 0)
							return INVALID_MOVE;
					for (int var = 1; var < amount; ++var) {
						if(board->board[from.row-var][from.column-var] != EMPTY_ENTRY)
							return INVALID_MOVE;
					}
					if(board->board[from.row-amount][from.column-amount] == EMPTY_ENTRY||
							opposite_players(board->board[from.row][from.column], board->board[from.row-amount][from.column-amount]))
						return VALID_MOVE;
					break;
				}
			default:
						return INVALID_MOVE;

	}
	return INVALID_MOVE;
}

valid_move is_valid_queen(game_board* board, position from, position to){
	int offset_column = to.column - from.column;
	int offset_row = to.row - from.row;
	DIAGONAL d_mode = diagonal_direction(offset_column, offset_row);
	STRAIGHT s_mode = straight_direction(offset_column, offset_row);

	if(d_mode != NOT_DIAGONAL){
		return is_valid_diagonal(board, from, d_mode, abs(offset_column));
	}
	if(s_mode != NOT_STRAIGHT){
		//only one of offsets will be >0
		return is_valid_straight(board, from, s_mode, abs(offset_row) + abs(offset_column));
	}
	return INVALID_MOVE;

}

valid_move is_valid_king(game_board* board, position from, position to){
	int offset_column = to.column - from.column;
		int offset_row = to.row - from.row;
	DIAGONAL d_mode = diagonal_direction(offset_column, offset_row);
	STRAIGHT s_mode = straight_direction(offset_column, offset_row);

	/*
	 * All the hard-coded numbers in this section refer to chess rules regarding the castling.
	 * for further information, please refer to the chess rules book
	 * The logic behind the ifs is:
	 * Test that all the relevant spots (rook king and the spots in between) are not catchable and are empty
	 * if so, and the relevant castling flag is on, it is a castling move.
	 * else, test by the regular tool rules it is a valid move.
	 */
	if(board->curr_player == WHITE_PLAYER && from.column == 4 && from.row == 0 && to.column == 6 && to.row == 0 &&
	   board->board[0][7] == WHITE_ROOK && board->short_castle_white == true && board->board[0][5] == EMPTY_ENTRY &&
	    board->board[0][6] == EMPTY_ENTRY  && !is_catchable(board,from) && !is_catchable(board,(position){0,5}) &&
	   !is_catchable(board,(position){0,6}) && !is_catchable(board,(position){0,7})){
		return CASTLE_MOVE;
	}
	if(board->curr_player == WHITE_PLAYER && from.column == 4 && from.row == 0 && to.row == 0 && to.column == 2 &&
	   board->board[0][0] == WHITE_ROOK && board->long_castle_white == true && board->board[0][3] == EMPTY_ENTRY
	   && board->board[0][1] == EMPTY_ENTRY && board->board[0][2] == EMPTY_ENTRY && !is_catchable(board,from)
	   && !is_catchable(board,(position){0,3}) &&  !is_catchable(board,(position){0,2}) && !is_catchable(board,(position){0,1}) && !is_catchable(board,(position){0,0})){
	   	  	return CASTLE_MOVE;
	}
	if(board->curr_player == BLACK_PLAYER && from.column == 4 && from.row == 7 && to.row == 7 && to.column == 6 &&
	   board->board[7][7] == BLACK_ROOK && !is_catchable(board,from) && board->short_castle_black == true &&
	   board->board[7][5] == EMPTY_ENTRY && board->board[7][6] == EMPTY_ENTRY && !is_catchable(board,(position){7,5}) &&
	   !is_catchable(board,(position){7,6}) && !is_catchable(board,(position){7,7})){
		return CASTLE_MOVE;
	}
	if(board->curr_player == BLACK_PLAYER && from.column == 4 && from.row == 7 && to.row == 7 && to.column == 2 &&
	   board->board[7][0] == BLACK_ROOK  && board->long_castle_black == true && board->board[7][1] == EMPTY_ENTRY
	    && board->board[7][2] == EMPTY_ENTRY && board->board[7][3] == EMPTY_ENTRY &&  !is_catchable(board,from) &&
	   !is_catchable(board,(position){7,0}) &&   !is_catchable(board,(position){7,1}) && !is_catchable(board,(position){7,2})  && !is_catchable(board,(position){7,3})){
		return CASTLE_MOVE;
	}


	if(abs(offset_column) > 1 || abs(offset_row) > 1){
		return INVALID_MOVE;
	}

	if(d_mode != NOT_DIAGONAL){
		return is_valid_diagonal(board, from, d_mode, abs(offset_column));
	}
	if(s_mode != NOT_STRAIGHT){
		//only one of offsets will be >0
				return is_valid_straight(board, from, s_mode, abs(offset_row) + abs(offset_column));
	}
	return INVALID_MOVE;
}

check_valid_func get_validator(game_tools tool){
	switch(tool){
		case EMPTY_ENTRY:
			return 0;
		case WHITE_PAWN:
		case BLACK_PAWN:
			return is_valid_pawn;
		case WHITE_KING:
		case BLACK_KING:
			return is_valid_king;
		case WHITE_QUEEN:
		case BLACK_QUEEN:
			return is_valid_queen;
		case WHITE_ROOK:
		case BLACK_ROOK:
			return is_valid_rook;
		case WHITE_BISHOP:
		case BLACK_BISHOP:
			return is_valid_bishop;
		case WHITE_KNIGHT:
		case BLACK_KNIGHT:
			return is_valid_knight;
	}
	return 0;
}

valid_move is_valid_pawn(game_board* board, position from, position to){
	int offset_column = to.column - from.column;
			int offset_row = to.row - from.row;

	DIAGONAL d_mode = diagonal_direction(offset_column, offset_row);
	STRAIGHT s_mode = straight_direction(offset_column, offset_row);
	if(islower(board->board[from.row][from.column])){
		if(s_mode == FORWARD && offset_row == 2 && from.row == 1 && board->board[to.row][to.column] == EMPTY_ENTRY
				&& board->board[to.row - 1][to.column] == EMPTY_ENTRY)
			return VALID_MOVE;
		if(s_mode == FORWARD && offset_row == 1 && board->board[to.row][to.column] == EMPTY_ENTRY)
					return VALID_MOVE;
		if((d_mode == FORWARD_R || d_mode == FORWARD_L) && offset_row == 1 && opposite_players(board->board[to.row][to.column], board->board[from.row][from.column])){
			return VALID_MOVE;
		}
	}else{
		if(s_mode == BACKWORD && offset_row == -2 && from.row == 6 && board->board[to.row][to.column] == EMPTY_ENTRY
				&& board->board[to.row + 1][to.column] == EMPTY_ENTRY)
					return VALID_MOVE;
		if(s_mode == BACKWORD && offset_row == -1 && board->board[to.row][to.column] == EMPTY_ENTRY)
					return VALID_MOVE;
		if((d_mode == BACKWORD_R || d_mode == BACKWORD_L) && offset_row == -1 && opposite_players(board->board[to.row][to.column], board->board[from.row][from.column])){
			return VALID_MOVE;
		}
	}
	return INVALID_MOVE;
}

valid_move is_valid_bishop(game_board* board, position from, position to){
	int offset_column = to.column - from.column;
			int offset_row = to.row - from.row;
	DIAGONAL d_mode = diagonal_direction(offset_column, offset_row);

	if(d_mode != NOT_DIAGONAL){
		return is_valid_diagonal(board, from, d_mode, abs(offset_column));
	}
	return INVALID_MOVE;
}

valid_move is_valid_rook(game_board* board, position from, position to){
	/*
	 * All the hard-coded numbers in this section refer to chess rules regarding the castling.
	 * for further information, please refer to the chess rules book
	 * The logic behind the ifs is:
	 * Test that all the relevant spots (rook king and the spots in between) are not catchable and are empty
	 * if so, and the relevant castling flag is on, it is a castling move.
	 * else, test by the regular tool rules it is a valid move.
	 */

	int offset_row = to.row - from.row;
	int offset_column = to.column - from.column;
	STRAIGHT s_mode = straight_direction(offset_column, offset_row);

	if(s_mode != NOT_STRAIGHT){
		//only one of offsets will be >0
				return is_valid_straight(board, from, s_mode, abs(offset_row) + abs(offset_column));
	}

	return INVALID_MOVE;
}



valid_move is_valid_knight(game_board* board, position from, position to){
	int offset_column = to.column - from.column;
			int offset_row = to.row - from.row;

	if(((abs(offset_column) == 1) && (abs(offset_row) == 2)) || ((abs(offset_column) == 2) && (abs(offset_row) == 1))){
		if(board->board[to.row][to.column] == EMPTY_ENTRY|| opposite_players(board->board[to.row][to.column], board->board[from.row][from.column]))
			return VALID_MOVE;
	}
	return INVALID_MOVE;
}

void insert_possible_promotions_white(tool_pos availible_pos[BOARD_SIZE*BOARD_SIZE*3], int* num_pos, position to){
	availible_pos[*num_pos].pos = to;
	availible_pos[*num_pos].tool_type = WHITE_QUEEN;(*num_pos)++;
	availible_pos[*num_pos].pos = to;
	availible_pos[*num_pos].tool_type = WHITE_ROOK;(*num_pos)++;
	availible_pos[*num_pos].pos = to;
	availible_pos[*num_pos].tool_type = WHITE_KNIGHT;(*num_pos)++;
	availible_pos[*num_pos].pos = to;
	availible_pos[*num_pos].tool_type = WHITE_BISHOP;(*num_pos)++;
	availible_pos[*num_pos].pos = to;
	availible_pos[*num_pos].tool_type = WHITE_PAWN;
	(*num_pos)++;
}

void insert_possible_promotions_black(tool_pos availible_pos[BOARD_SIZE*BOARD_SIZE*3], int* num_pos, position to){
	availible_pos[*num_pos].pos = to;
	availible_pos[*num_pos].tool_type = BLACK_QUEEN;(*num_pos)++;
	availible_pos[*num_pos].pos = to;
	availible_pos[*num_pos].tool_type = BLACK_ROOK;(*num_pos)++;
	availible_pos[*num_pos].pos = to;
	availible_pos[*num_pos].tool_type = BLACK_KNIGHT;(*num_pos)++;
	availible_pos[*num_pos].pos = to;
	availible_pos[*num_pos].tool_type = BLACK_PAWN;(*num_pos)++;
	availible_pos[*num_pos].pos = to;
	availible_pos[*num_pos].tool_type = BLACK_BISHOP;
	(*num_pos)++;
}

legit_moves* legit_moves_array(game_board* board, position from,
		valid_move (*is_valid_move_tool_type)(game_board* board, position from, position to)){
	//for pawn promotions there can be several move options
	tool_pos availible_pos[BOARD_SIZE*BOARD_SIZE*3];
	int num_pos = 0, enemy_tools = 0;
	for (int row = 0; row < BOARD_SIZE; ++row) {
		for (int column = 0; column < BOARD_SIZE; ++column) {
			if(row == from.row && column == from.column) continue;
			position to = {row,column};
			valid_move move_status = is_valid_move_tool_type(board,from,to);
			if(move_status == VALID_MOVE || move_status == CASTLE_MOVE){
				if(from.row == 6 && board->board[from.row][from.column] == WHITE_PAWN){
					insert_possible_promotions_white(availible_pos,&num_pos,to);
				}else if(from.row == 1 && board->board[from.row][from.column] == BLACK_PAWN){
					insert_possible_promotions_black(availible_pos,&num_pos,to);
				}else{
					availible_pos[num_pos].pos = to;
					availible_pos[num_pos].tool_type = board->board[row][column];
					num_pos++;
				}
				if(board->board[row][column] != EMPTY_ENTRY)
					enemy_tools++;
			}
		}
	}
	tool_pos* valid_pos = malloc(sizeof(tool_pos)* num_pos);
	if(!valid_pos){return NULL;}
	for (int index = 0; index < num_pos; ++index) {
		valid_pos[index] = availible_pos[index];
	}
	legit_moves* legit =(legit_moves*)malloc(sizeof(legit_moves));
	if(!legit){
		free(valid_pos);
		return NULL;
	}
	legit->valid_pos = valid_pos;
	legit->moves_num = num_pos;
	legit->enemy_tools = enemy_tools;
	return legit;
}
void destroy_legit_moves(legit_moves* src){
	if(!src){return ;}
	if(src->valid_pos)
		free(src->valid_pos);
	free(src);
}
bool is_pos_valid(legit_moves* src,position pos){
	if(!src){
		return false;
	}
	for(int i=0;i<src->moves_num;i++){
		//the input position is in the "list" of valid position, therefore return 1.
		if(src->valid_pos[i].pos.row == pos.row && src->valid_pos[i].pos.column == pos.column){
			return true;
		}
	}
	return false;
}

bool opposite_players(char player_1, char player_2){
	if(player_1 == EMPTY_ENTRY || player_2 == EMPTY_ENTRY)
		return false;
	switch(isupper(player_1)){
		case 0 /*false for isupper*/ :
			return isupper(player_2);
		default /*false for isupper*/:
			return islower(player_2);
	}
	return false;
}
int compare_full_moves(const void* t1,const void* t2){
	tool_pos* move1 = (tool_pos*)t1;
	tool_pos* move2 = (tool_pos*)t2;
	if (!move1 || !move2){
		return 0;
	}
	//if both have the same x coordinate, let y break the tie.
	if(move1->pos.row == move2->pos.row){
		return move1->pos.column-move2->pos.column;
	}
	//different x coordinates, continue as usual.
	 return move1->pos.row-move2->pos.row;
}

void identify_castle_from_move(bool is_castle, bool* short_castle_white, bool* short_castle_black,
		bool* long_castle_white, bool* long_castle_black, position from, position to){
	if(!is_castle)
		return;
	if(from.row == 0 && from.column == 4){
		if(to.column == 6)
			*short_castle_white = true;
		if(to.column == 2)
			*long_castle_white = true;
	}else if (from.row == 7 && from.column == 4){
		if(to.column == 6)
			*short_castle_black = true;
		if(to.column == 2)
			*long_castle_black = true;
	}
}


void print_move_of_tool(game_board* board, position from, position to){
	char tool_name[10];
	switch(board->board[from.row][from.column]){
			case WHITE_PAWN:
			case BLACK_PAWN:
				strcpy(tool_name,"pawn");
				break;
			case WHITE_KING:
			case BLACK_KING:
				strcpy(tool_name,"king");
				break;
			case WHITE_QUEEN:
			case BLACK_QUEEN:
				strcpy(tool_name,"queen");
				break;
			case WHITE_ROOK:
			case BLACK_ROOK:
				strcpy(tool_name,"rook");
				break;
			case WHITE_BISHOP:
			case BLACK_BISHOP:
				strcpy(tool_name,"bishop");
				break;
			case WHITE_KNIGHT:
			case BLACK_KNIGHT:
				strcpy(tool_name,"knight");
				break;
			default:
				break;
		}
	if(board->p_type == PC)
		printf("Computer: move %s at <%d,%c> to <%d,%c>\n",tool_name,from.row + 1,from.column + 'A', to.row + 1, to.column + 'A');
	}

game_tools tool_for_promotion(game_board* board){
	    char str[MAX_STRING];
		bool valid_tool = false;
		do{
		printf("Pawn promotion- please replace the pawn by queen, rook, knight, bishop or pawn:\n");
		scanf("%s", str);
		if(!strcmp(str,"queen")){
			valid_tool = true;
			return (board->curr_player == WHITE_PLAYER)? WHITE_QUEEN:BLACK_QUEEN;
		}else if(!strcmp(str,"rook")){
			valid_tool = true;
			return (board->curr_player == WHITE_PLAYER)? WHITE_ROOK:BLACK_ROOK;
		}else if(!strcmp(str,"knight")){
					valid_tool = true;
					return (board->curr_player == WHITE_PLAYER)? WHITE_KNIGHT:BLACK_KNIGHT;
		}else if(!strcmp(str,"bishop")){
					valid_tool = true;
					return (board->curr_player == WHITE_PLAYER)? WHITE_BISHOP:BLACK_BISHOP;
		}else if(!strcmp(str,"pawn")){
			valid_tool = true;
			return (board->curr_player == WHITE_PLAYER)? WHITE_PAWN:BLACK_PAWN;
		}else{
			printf("Invalid Type\n");
		}}while(!valid_tool);
	return EMPTY_ENTRY;
}

void print_promotion_from_pc(position from, position to, game_tools tool_promoted){
	char tool_name[10];
	switch(tool_promoted){
			case WHITE_PAWN:
			case BLACK_PAWN:
				strcpy(tool_name,"pawn");
				break;
			case WHITE_KING:
			case BLACK_KING:
				strcpy(tool_name,"king");
				break;
			case WHITE_QUEEN:
			case BLACK_QUEEN:
				strcpy(tool_name,"queen");
				break;
			case WHITE_ROOK:
			case BLACK_ROOK:
				strcpy(tool_name,"rook");
				break;
			case WHITE_BISHOP:
			case BLACK_BISHOP:
				strcpy(tool_name,"bishop");
				break;
			case WHITE_KNIGHT:
			case BLACK_KNIGHT:
				strcpy(tool_name,"knight");
				break;
			default:
				break;
		}
	if(!strcmp(tool_name, "knight"))
		printf("Computer: move %s <%d,%c> to <%d,%c> and promote to %s\n","pawn",from.row + 1,from.column + 'A', to.row + 1, to.column + 'A', tool_name);
	else
		printf("Computer: move %s <%d,%c> to <%d,%c> and promote to %s\n","pawn",from.row + 1,from.column + 'A', to.row + 1, to.column + 'A', "queen");
}

