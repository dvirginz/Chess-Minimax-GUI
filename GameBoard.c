/*
 * GameBoard.c
 *
 *  Created on: 4 Aug 2017
 *      Author: GalL
 */

#include "GameBoard.h"
#include "Tools.h"
#include <ctype.h>

void init_game_board(game_board* src){
	if(!src){return;}
	game_tools **a = src->board;
	//initialize the empty entries in the board
	for(int i=2 ; i<GAME_BOARD_SIZE -2 ; i++){
		for(int j=0; j< GAME_BOARD_SIZE ; j++){
			a[i][j] =EMPTY_ENTRY;
		}
	}
	//initialize the rows of pawns
	for(int i=0;i<GAME_BOARD_SIZE; i++){
		a[1][i] = WHITE_PAWN;
		a[GAME_BOARD_SIZE -2][i]=BLACK_PAWN;
	}
	//ROOKS
	a[0][0] = WHITE_ROOK;
	a[0][GAME_BOARD_SIZE -1] = WHITE_ROOK;
	a[GAME_BOARD_SIZE -1][0] = BLACK_ROOK;
	a[GAME_BOARD_SIZE -1][GAME_BOARD_SIZE -1] = BLACK_ROOK;
	//KNIGHTS
	a[0][1] = WHITE_KNIGHT;
	a[0][GAME_BOARD_SIZE -2] =  WHITE_KNIGHT;
	a[GAME_BOARD_SIZE -1][1] = BLACK_KNIGHT;
	a[GAME_BOARD_SIZE -1][GAME_BOARD_SIZE -2] = BLACK_KNIGHT;
	//BISHOPS
	a[0][2] = WHITE_BISHOP;
	a[0][GAME_BOARD_SIZE -3] =  WHITE_BISHOP;
	a[GAME_BOARD_SIZE -1][2] = BLACK_BISHOP;
	a[GAME_BOARD_SIZE -1][GAME_BOARD_SIZE -3] =BLACK_BISHOP;
	//KINGS AND QUEENS
	a[0][4] = WHITE_KING;
	a[0][3] =  WHITE_QUEEN;
	a[GAME_BOARD_SIZE -1][4] = BLACK_KING;
	a[GAME_BOARD_SIZE -1][3] =BLACK_QUEEN;
	src->long_castle_black = src->long_castle_white = src->short_castle_black = src->short_castle_white = true;

}
void update_tools_array(game_board* src){
	if(!src){return;}
	int white_count,black_count;
	white_count = black_count=0;
	for(int i = 0 ; i< GAME_BOARD_SIZE ; i++){
		for(int j=0 ; j< GAME_BOARD_SIZE ; j++){
			if(src->board[i][j] != EMPTY_ENTRY){
				if(isupper(src->board[i][j])){
					//black
					src->black_tools[black_count].pos.row = i;
					src->black_tools[black_count].pos.column = j;
					src->black_tools[black_count].tool_type = src->board[i][j];
                    black_count++;
                }
				else{
					//white
					src->white_tools[white_count].pos.row = i;
					src->white_tools[white_count].pos.column = j;
					src->white_tools[white_count].tool_type = src->board[i][j];
                    white_count++;
                }
			}
		}
	}
	//fill the rest with garbage
	tool_pos dummy = {EMPTY_ENTRY,{10,10}};
	for(int i = white_count; i< MAX_TOOLS;i++){
		src->white_tools[i] = dummy;
	}
	for(int i = black_count; i< MAX_TOOLS;i++){
		src->black_tools[i] = dummy;
	}
}
rollback* create_rollback(tool_pos t1_orig , tool_pos t2_orig){
	//allocate space & verify success
	rollback* res = (rollback*)malloc(sizeof(rollback));
	if(!res){return NULL;}
	res->from = t1_orig;
	res->to = t2_orig;
	res->scw = res->scb = res->lcw =res->lcb = false;
	return res;
}

void destroy_rollback(rollback* src){
	free(src);
}

game_board* create_game(int difficulty){
	//alloc space
	game_board* res = (game_board*)malloc(sizeof(game_board));
	//verify successful allocation
	if(!res){ return NULL;}
	res->curr_player = WHITE_PLAYER;
	res->board = malloc(sizeof(game_tools*)*BOARD_SIZE);
	if(!res->board) exit(1);
	for (int row = 0; row < BOARD_SIZE; ++row) {
		res->board[row] = malloc(sizeof(game_tools)* BOARD_SIZE);
		if(!res->board) exit(1);
	}
	res->black_tools = malloc(MAX_TOOLS*sizeof(tool_pos));
	if(!res->black_tools) exit(1);
	res->white_tools = malloc(MAX_TOOLS*sizeof(tool_pos));
	if(!res->white_tools) exit(1);

	init_game_board(res);
	update_tools_array(res);
	//initialize the undo space
	res->hist_size=HIST_SIZE;
	rollback** res_undo = (rollback**)malloc((res->hist_size)*sizeof(rollback*));
	if(!res_undo){
		free(res);
		return NULL;
	}
	//DUMMY ROLLBACK
	tool_pos t1 = {EMPTY_ENTRY,{0,0}};
	//place a rollback struct in each slot, initially with DUMMY values.
	for(int i=0;i<res->hist_size; i++){
		rollback*  tmp = create_rollback(t1,t1);
		if(!tmp){
			//failed allocation, free resources and return null
			for(int j=0;j<i;j++){
				destroy_rollback(res->undo_data[j]);
				free(res_undo);
				free(res);
				return NULL;
			}
		}
		res_undo[i]= tmp;
	}
	res->undo_data = res_undo;
	res->curr_hist_pos=0;
	res->curr_hist_len=0;
	res->difficulty = difficulty;
	res->short_castle_white = res->short_castle_black = res->long_castle_white = res->long_castle_black = true;
	//default is CLI. when creating a game in GUI mode, changing it immediatly.
	res->game_mode = CLI;
	res->user_mode = SINGLE_PLAYER_GAME; // default is single player. this field has no effect in CLI mode.
	return res;
}
void destroy_game(game_board* src){
	if(!src){return;}
	//destroy each rollback
	for(int i = 0 ; i<src->hist_size;i++){
		destroy_rollback(src->undo_data[i]);
	}
	for (int var = 0; var < BOARD_SIZE; ++var) {
		free(src->board[var]);
	}
	free(src->board);
	free(src->black_tools);
	free(src->white_tools);
	//free the undo data and src itself.
	free(src->undo_data);
	free(src);
}
int modulo(int a,int b){
	//calculates a mod b
	return (a%b +b)%b;
}

CHESS_STATUS undo_move(game_board* src){
if(!src){
		return CHESS_STATUS_INVALID_ARGUMENT;
	}
	if(src->curr_hist_len<1){
		if(src->game_mode == CLI && src->p_type == HUMAN){
			printf("Empty history, move cannot be undone\n");
		}
			return CHESS_STATUS_NO_HISTORY; //  no undo possible
	}
	//the rollback struct relevant for this undo
	rollback* r = src->undo_data[src->curr_hist_pos];
	//calculate the new position
	src->curr_hist_pos = modulo(src->curr_hist_pos-1,src->hist_size);
	//src->curr_hist_pos--;
	src->board[r->from.pos.row][r->from.pos.column] = r->from.tool_type;
	src->board[r->to.pos.row][r->to.pos.column] = r->to.tool_type;

	//if king moves 2 blocks, it was a castle
	if(r->from.tool_type == WHITE_KING && r->to.pos.column - r->from.pos.column == 2){
		src->board[0][7] = WHITE_ROOK;
		src->board[0][5] = EMPTY_ENTRY;
	}else if(r->from.tool_type == WHITE_KING && r->to.pos.column - r->from.pos.column == -2){
		src->board[0][0] = WHITE_ROOK;
		src->board[0][3] = EMPTY_ENTRY;
	}else if(r->from.tool_type == BLACK_KING && r->to.pos.column - r->from.pos.column == 2){
		src->board[7][7] = BLACK_ROOK;
		src->board[7][5] = EMPTY_ENTRY;
	}else if(r->from.tool_type == BLACK_KING && r->to.pos.column - r->from.pos.column == -2){
		src->board[7][0] = BLACK_ROOK;
		src->board[7][3] = EMPTY_ENTRY;
	}

	//old castling values.
	src->short_castle_white = r->scw;
	src->short_castle_black = r->scb;
	src->long_castle_white = r->lcw;
	src->long_castle_black = r->lcb;
	update_tools_array(src);
	//decrease hist_len
	src->curr_hist_len--;
	//adjust turns
	char* player = (src->curr_player == WHITE_PLAYER)? "black":"white";
	src->curr_player = (src->curr_player == WHITE_PLAYER)? BLACK_PLAYER:WHITE_PLAYER;
	if(src->game_mode == CLI && src->p_type == HUMAN){
		//THIS IS NOT A BUG, FOR PRINTING PUSPOSES. DONT GET CONFUSED
		int from_row = r->to.pos.row+1;
		char from_col = r->to.pos.column + 65;
		int to_row = r->from.pos.row+1;
		char to_col = r->from.pos.column + 65;
		printf("Undo move for player %s : <%d,%c> -> <%d,%c>\n",player,from_row,from_col,to_row,to_col);
	}
		return CHESS_STATUS_SUCCESS;
}
rollback* copy_rollback(rollback* src){
	if(!src){
		return NULL;
	}
	//allocate the result, copy fields from source.
	rollback* res = (rollback*)malloc(sizeof(rollback));
	if(!res){return NULL;}
	res->from = src->from;
	res->to = src->to;
	res->scw = src->scw;
	res->scb = src->scb;
	res->lcw = src->lcw;
	res->lcb = src->lcb;
	return res;
}


CHESS_STATUS print_board_console(game_board* src){
	if(!src){
		return CHESS_STATUS_INVALID_ARGUMENT;
	}

	int rows = GAME_BOARD_SIZE -1;
	//print the rows
	for(int i = 0 ; i < GAME_BOARD_SIZE ; i++){
		printf("%d| ",GAME_BOARD_SIZE-i);
		for(int j=0 ; j < GAME_BOARD_SIZE ; j++){
			printf("%c ",src->board[rows -i][j]);
		}
		printf("|\n");
	}
	printf("  -----------------\n  ");
	fflush(stdout);
	for(int i = 0 ; i < GAME_BOARD_SIZE ; i++){
		printf(" %c",i+65);
	}
	printf(" \n");
	fflush(stdout);
	return CHESS_STATUS_SUCCESS;
}
bool is_catchable_reg(game_board* src,position pos){
	if(!src){return false;}
	//the tools for which we will check - if pos currently holds a white tool -
	//check the black tools and vice versa.
    tool_pos* tools_arr;
    if(src->board[pos.row][pos.column] != EMPTY_ENTRY){
        tools_arr = (isupper(src->board[pos.row][pos.column]))?src->white_tools:src->black_tools;
    }else{
        tools_arr = (src->curr_player == BLACK_PLAYER)?src->white_tools:src->black_tools;
    }
//    src->curr_player = (src->curr_player == BLACK_PLAYER)?WHITE_PLAYER:BLACK_PLAYER;
	for(int i=0 ; i<MAX_TOOLS;i++){
		//no need to check empty entry, it can never catch anything.
		if(tools_arr[i].tool_type != EMPTY_ENTRY){
			legit_moves* valid_moves = valid_moves_array(src, tools_arr[i].pos,get_validator(src->board[tools_arr[i].pos.row][tools_arr[i].pos.column]));
			if(is_pos_valid(valid_moves,pos)){
//			    src->curr_player = (src->curr_player == BLACK_PLAYER)?WHITE_PLAYER:BLACK_PLAYER;
				return true;
			}
		}
	}
	//no threat was found, return 0
//    src->curr_player = (src->curr_player == BLACK_PLAYER)?WHITE_PLAYER:BLACK_PLAYER;
	return false;
}
bool is_catchable_king(game_board* src,position pos){
	if(!src){return false;}
	//the tools for which we will check - if pos currently holds a white tool -
	//check the black tools and vice versa.
    tool_pos* tools_arr;
    if(src->board[pos.row][pos.column] != EMPTY_ENTRY){
        tools_arr = (isupper(src->board[pos.row][pos.column]))?src->white_tools:src->black_tools;
    }else{
        tools_arr = (src->curr_player == BLACK_PLAYER)?src->white_tools:src->black_tools;
    }

	for(int i=0 ; i<MAX_TOOLS;i++){
		//no need to check empty entry, it can never catch anything.
		if(tools_arr[i].tool_type != EMPTY_ENTRY){

			//get all the legit moves
			valid_move move_type = (get_validator(tools_arr[i].tool_type))(src,tools_arr[i].pos,pos);
			//valid_move move_type = valid_move_for_tool(src,tools_arr[i].pos,pos,get_validator(tools_arr[i].tool_type));
			if(move_type != INVALID_MOVE){
				return true;
			}
		}
	}
	//no threat was found, return 0
	return false;
}

bool is_catchable_after_move(game_board* src,position from, position to){
	if(!src){
			return false;
		}
		//keep the original tools.
		tool_pos t1_orig = (tool_pos){src->board[from.row][from.column],(position){from.row,from.column}};
		tool_pos t2_orig = (tool_pos){src->board[to.row][to.column],(position){to.row,to.column}};
		//change to new ones (temporarily).
		src->board[from.row][from.column] = EMPTY_ENTRY;
		src->board[to.row][to.column] = t1_orig.tool_type;
		update_tools_array(src);
		src->curr_player = (src->curr_player == WHITE_PLAYER)? BLACK_PLAYER:WHITE_PLAYER;
		//look for check, keep the result.
		bool res = (is_catchable(src,to));
		//restore original tools
		src->board[from.row][from.column] = t1_orig.tool_type;
		src->board[to.row][to.column] = t2_orig.tool_type;
		update_tools_array(src);
		src->curr_player = (src->curr_player == WHITE_PLAYER)? BLACK_PLAYER:WHITE_PLAYER;
		//return the result
		return res;
}


bool is_catchable(game_board* src,position pos){
	if(!src){
		return false;
	}
	if(src->board[pos.row][pos.column] == WHITE_KING || src->board[pos.row][pos.column] == BLACK_KING ){
		//it's a king
		return is_catchable_king(src,pos);
	}
	//it's a regular tool.
	return is_catchable_reg(src,pos);
}

bool is_check(game_board* src,player_color col){
	if(!src){
		return false;
	}
	game_tools king = (col == 'W')?WHITE_KING : BLACK_KING;
	tool_pos* tools_arr = (col == 'W')? src->white_tools : src->black_tools ;
	//determine the location of the king
	position king_pos = {10,10};
	for(int i=0; i< MAX_TOOLS ; i++){
		if(tools_arr[i].tool_type == king){
			king_pos = tools_arr[i].pos;
			break;
		}
	}
	//check if the king is catchable
	return is_catchable(src,king_pos);
}
void add_undo_entry(game_board* src,tool_pos from,tool_pos to,bool scw,bool scb,bool lcw,bool lcb){
	if(!src){
		return ;
	}
	//calculate the new position and increment curr_hist_pos
	src->curr_hist_pos = modulo(src->curr_hist_pos+1,src->hist_size);
	rollback* target_roll = src->undo_data[src->curr_hist_pos];
	target_roll->from = from;
	target_roll->to = to;
	//in case of undo of promotions, those value may be different
	target_roll->to.tool_type = src->board[to.pos.row][to.pos.column];
	//if the history is not yet "full" (i.e - no circulation), increment hist_len.
	if(src->curr_hist_len<src->hist_size){
		src->curr_hist_len++;
	}
	target_roll->scw = scw;
	target_roll->scb = scb;
	target_roll->lcw = lcw;
	target_roll->lcb = lcb;
}



game_board* copy_change_history(game_board* src,int new_hist_size){
	
	if(!src){return NULL;}
	game_board* res = create_game(new_hist_size);
	if(!res){return NULL;}
	res->curr_player = src->curr_player;
	//clone the board
	for(int i = 0 ; i < GAME_BOARD_SIZE;i++){
		for(int j = 0 ; j < GAME_BOARD_SIZE;j++){
			res->board[i][j] = src->board[i][j];
		}
	}
	update_tools_array(res);
	if(new_hist_size >= src->hist_size){
		for(int k = 0 ; k < src->curr_hist_pos -1  ; k++){
			int loc = modulo (src->curr_hist_pos -k, src->hist_size);
			rollback* r = src->undo_data[loc];
			res->undo_data[k]->from = r->from;
			res->undo_data[k]->to = r->to;
			res->undo_data[k]->scw = r->scw;
			res->undo_data[k]->scb = r->scb;
			res->undo_data[k]->lcw = r->lcw;
			res->undo_data[k]->lcb = r->lcb;
		}
	}
	//copy parameters
	res->hist_size = new_hist_size;
	res->curr_player = src->curr_player;
	res->difficulty = src->difficulty;
	res->short_castle_white = src->short_castle_white;
	res->short_castle_black = src->short_castle_black;
	res->long_castle_white = src->long_castle_white;
	res->long_castle_black = src->long_castle_black;
	res->p_type = src->p_type;
	res->game_mode = src->game_mode;
	res->user_mode = src->user_mode;
	//we copied the entries and rearranged them in the new game, hence hist_pos and hist_len are identical.
	res->curr_hist_len = res->curr_hist_pos =\
			(new_hist_size >= src->hist_size)?src->curr_hist_len:0;

	return res;
}
game_board* copy_game(game_board* src){
	if(!src){return NULL;}
	return copy_change_history(src,src->hist_size);
}

bool is_move_valid(game_board* src,position p1,position p2){
	if(!src){
		return false;
	}
	player_color orig_color = src->curr_player;
	//keep the original tools.
	tool_pos t1_orig = (tool_pos){src->board[p1.row][p1.column],(position){p1.row,p1.column}};
	tool_pos t2_orig = (tool_pos){src->board[p2.row][p2.column],(position){p2.row,p2.column}};
	//change to new ones (temporarily).
	src->board[p1.row][p1.column] = EMPTY_ENTRY;
	src->board[p2.row][p2.column] = t1_orig.tool_type;
	update_tools_array(src);
	src->curr_player = (src->curr_player == WHITE_PLAYER)? BLACK_PLAYER:WHITE_PLAYER;
	//look for check, keep the result.
	bool res = !(is_check(src,orig_color));
	//restore original tools
	src->board[p1.row][p1.column] = t1_orig.tool_type;
	src->board[p2.row][p2.column] = t2_orig.tool_type;
	update_tools_array(src);
	src->curr_player = (src->curr_player == WHITE_PLAYER)? BLACK_PLAYER:WHITE_PLAYER;
	//return the result
	return res;
}


valid_move valid_move_for_tool(game_board* board, position from, position to, check_valid_func valid_func){
	valid_move move_type = valid_func(board,from,to);
	if(move_type == VALID_MOVE || move_type == CASTLE_MOVE){
		if(is_move_valid(board,from,to))
			return move_type;
	}
	return INVALID_MOVE;
}

legit_moves* valid_moves_array(game_board* board, position from,
		valid_move (*is_valid_move_tool_type)(game_board* board, position from, position to)){
	//get all the moves,then filter out those that result in check.
	legit_moves* res = legit_moves_array(board,from,is_valid_move_tool_type);
	if(!res){
		return NULL;
	}
	int actual_moves = 0;
	for(int i = 0 ; i < res->moves_num ; i++){
		if(is_move_valid(board,from,res->valid_pos[i].pos)){
			actual_moves++;
		}
		else{//this is not a valid move - getting rid of it by changing it's coordinate.
			//without incrementing actual_moves - this will make the move "hidden" after QSORT.
			if(board->board[res->valid_pos[i].pos.row][res->valid_pos[i].pos.column] != EMPTY_ENTRY)
				res->enemy_tools--;
			res->valid_pos[i].pos.row=10;


		}

	}
	//perform quicksort
	int (*cmp)(const void* t1,const void*t2);
	cmp = compare_full_moves;
	qsort(res->valid_pos,res->moves_num,sizeof(tool_pos),cmp);
	res->moves_num = actual_moves;
	return res;
}

void update_castle_flags(const tool_pos* from, game_board* src) {
	if (from->tool_type == WHITE_KING) {
		src->short_castle_white = src->long_castle_white = false;
	} else if (from->tool_type == BLACK_KING) {
		src->short_castle_black = src->long_castle_black = false;
	} else if (from->tool_type == WHITE_ROOK && from->pos.column == 0) {
		src->long_castle_white = false;
	} else if (from->tool_type == WHITE_ROOK && from->pos.column == 7) {
		src->short_castle_white = false;
	} else if (from->tool_type == BLACK_ROOK && from->pos.column == 0) {
		src->long_castle_black = false;
	} else if (from->tool_type == BLACK_ROOK && from->pos.column == 7)
		src->short_castle_black = false;
}

void update_rook_by_castle(const tool_pos* to, const tool_pos* from,
		game_board* src) {
	if (to->pos.column == 6 && from->tool_type == WHITE_KING) {
		src->board[0][7] = EMPTY_ENTRY;
		src->board[0][5] = WHITE_ROOK;
	} else if (to->pos.column == 2 && from->tool_type == WHITE_KING) {
		src->board[0][0] = EMPTY_ENTRY;
		src->board[0][3] = WHITE_ROOK;
	} else if (to->pos.column == 6 && from->tool_type == BLACK_KING) {
		src->board[7][7] = EMPTY_ENTRY;
		src->board[7][5] = BLACK_ROOK;
	} else if (to->pos.column == 2 && from->tool_type == BLACK_KING) {
		src->board[7][0] = EMPTY_ENTRY;
		src->board[7][3] = BLACK_ROOK;
	}
}

CHESS_STATUS set_move(game_board* src,tool_pos from,tool_pos to){
	//bad game pointer
	if(!src || from.tool_type == EMPTY_ENTRY){
		return CHESS_STATUS_INVALID_ARGUMENT;}
	//the tool_pos's we put in the undo are created in regard to the actual state of the board.
	tool_pos from_copy = from;
	from_copy.tool_type = src->board[from.pos.row][from.pos.column];
	tool_pos to_copy = to;
	to_copy.tool_type = src->board[from.pos.row][from.pos.column];
	//adding an undo entry
	add_undo_entry(src,from_copy,to_copy,src->short_castle_white,src->short_castle_black,src->long_castle_white,src->long_castle_black);
	update_castle_flags(&from, src);
	//perform the actual move
	src->board[to.pos.row][to.pos.column] = from.tool_type;
	src->board[from.pos.row][from.pos.column] = EMPTY_ENTRY;
	if(abs(from.pos.column-to.pos.column) == 2 && (from.tool_type ==WHITE_KING || from.tool_type == BLACK_KING)){
		update_rook_by_castle(&to, &from, src);
	}
	if(src->game_mode == CLI && src->p_type == HUMAN && from.tool_type == WHITE_PAWN && to.pos.row == 7){
		game_tools tool = tool_for_promotion(src);
		src->board[to.pos.row][to.pos.column] = tool;
	}else if(src->game_mode == CLI && src->p_type == HUMAN && from.tool_type == BLACK_PAWN && to.pos.row == 0){
		game_tools tool = tool_for_promotion(src);
		src->board[to.pos.row][to.pos.column] = tool;
	}else if(src->p_type == PC && from.tool_type == BLACK_PAWN && to.pos.row == 0){
		if(to.tool_type == BLACK_KNIGHT)
			src->board[to.pos.row][to.pos.column] = to.tool_type;
		else
			src->board[to.pos.row][to.pos.column] = BLACK_QUEEN;
	}else if(src->p_type == PC && from.tool_type == WHITE_PAWN && to.pos.row == 7){
		if(to.tool_type == WHITE_KNIGHT)
			src->board[to.pos.row][to.pos.column] = to.tool_type;
		else
			src->board[to.pos.row][to.pos.column] = WHITE_QUEEN;
	}
	src->curr_player = (src->curr_player == WHITE_PLAYER)? BLACK_PLAYER:WHITE_PLAYER;
	update_tools_array(src);
	fflush(stdout);
	return CHESS_STATUS_SUCCESS;
}

CHESS_STATUS game_state(game_board* src){
	if(!src){return CHESS_STATUS_INVALID_ARGUMENT;}
	tool_pos* arr = (src->curr_player == WHITE_PLAYER )?src->white_tools:src->black_tools;
	legit_moves* tmp_moves;
	for(int i = 0 ; i < MAX_TOOLS; ++i){
		if(arr[i].tool_type!=EMPTY_ENTRY){
			tmp_moves = valid_moves_array(src,arr[i].pos,get_validator(arr[i].tool_type));
			if(!tmp_moves){return CHESS_STATUS_MALLOC_ERROR;}
			if(tmp_moves->moves_num>0){
				destroy_legit_moves(tmp_moves);
				//the player has at least one move possible
				return CHESS_STATUS_NORMAL;
			}
			destroy_legit_moves(tmp_moves);
		}
	}
	//the player has zero moves. search for CHECK:
	if(is_check(src,src->curr_player)){
		//WE HAVE A MATE.
		return (src->curr_player == WHITE_PLAYER)?CHESS_STATUS_WHITE_IN_MATE:CHESS_STATUS_BLACK_IN_MATE;
	}
	//we have a tie:
	return CHESS_STATUS_TIE;
}

CASTLE_TYPE handle_castle_moves(position from,
						char right_castle_buffer[MAX_LINE_LENGTH],
						char left_castle_buffer[MAX_LINE_LENGTH], game_board* game,
						position to) {
	CASTLE_TYPE castle = CASTLE_UNDEF;
	if (game->board[from.row][from.column] == WHITE_ROOK && from.column == 7
			&& game->short_castle_white) {
		if (is_valid_king(game,(position){0,4} ,(position){0,6} ) == CASTLE_MOVE){
			sprintf(right_castle_buffer, "castle <%d,%c>", from.row + 1,
					from.column + 'A');
			castle = CASTLE_WHITE_RIGHT;}
	}
	if (game->board[from.row][from.column] == WHITE_ROOK && from.column == 0
			&& game->long_castle_white) {
		if (is_valid_king(game,(position){0,4} ,(position){0,2} ) == CASTLE_MOVE){
			sprintf(right_castle_buffer, "castle <%d,%c>", from.row + 1,
					from.column + 'A');
			castle = CASTLE_WHITE_LEFT;}
	}
	if (game->board[from.row][from.column] == BLACK_ROOK && from.column == 7
			&& game->short_castle_black) {
		if (is_valid_king(game,(position){7,4} ,(position){7,6} ) == CASTLE_MOVE){
			sprintf(right_castle_buffer, "castle <%d,%c>", from.row + 1,
					from.column + 'A');
			castle = CASTLE_BLACK_RIGHT;}
	}
	if (game->board[from.row][from.column] == BLACK_ROOK && from.column == 0
			&& game->long_castle_black) {
		if (is_valid_king(game, (position){7,4},(position){7,2} ) == CASTLE_MOVE){
			sprintf(right_castle_buffer, "castle <%d,%c>", from.row + 1,
					from.column + 'A');
			castle = CASTLE_BLACK_LEFT;}
	}
	if (game->board[from.row][from.column] == WHITE_KING
			|| game->board[from.row][from.column] == BLACK_KING) {
		valid_move move_status = is_valid_king(game, from,
				to);
		if (move_status == CASTLE_MOVE) {
			if (to.column > from.column)
				sprintf(right_castle_buffer, "castle <%d,%c>", from.row + 1,
						from.column + 3 + 'A');
			else
				sprintf(left_castle_buffer, "castle <%d,%c>", from.row + 1,
						from.column - 4 + 'A');
		}
	}
	return castle ;
}
