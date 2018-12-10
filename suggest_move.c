#include "suggest_move.h"
#include "evaluation.h"
#include "GameBoard.h"
#include "Tools.h"
#include <limits.h>


MOVE_COMMAND suggest_move(game_board* board, int max_depth){
	MOVE_COMMAND res = {(tool_pos){EMPTY_ENTRY,{0,0}},(tool_pos){EMPTY_ENTRY,{0,0}},INT_MIN,0,0};
	game_board* board_cpy = copy_change_history(board, board->difficulty);
	if(!board_cpy){
		return res;
	}
	switch(board->curr_player){
	case WHITE_PLAYER:
		res = suggest_max_move_ab(board_cpy, max_depth, INT_MIN, INT_MAX);
		break;
	case BLACK_PLAYER:
		res = suggest_min_move_ab(board_cpy, max_depth, INT_MIN, INT_MAX);
		break;
	}

	destroy_game(board_cpy);
	return res;
}

MOVE_COMMAND suggest_max_move_ab(game_board* board, int depth, int alpha,int beta){

	MOVE_COMMAND res = {(tool_pos){EMPTY_ENTRY,{0,0}},(tool_pos){EMPTY_ENTRY,{0,0}},INT_MIN,alpha,beta};
	tool_pos* player_tools = board->white_tools;
	CHESS_STATUS winner = game_state(board);

		if(winner == CHESS_STATUS_BLACK_IN_MATE || winner == CHESS_STATUS_WHITE_IN_MATE){
			return res;
		}

	if(depth == 0){
		res.value = evaluate_board(board);
		return res;
	}
	bool pron_move = false;
	legit_moves* tool_moves = NULL;
		for (int tool_idx = 0; tool_idx < MAX_TOOLS; ++tool_idx) {
			if(player_tools[tool_idx].tool_type != EMPTY_ENTRY){
				tool_moves = valid_moves_array(board, player_tools[tool_idx].pos, get_validator(player_tools[tool_idx].tool_type));
				for (int move_index = 0; move_index < tool_moves->moves_num; ++move_index) {
					CHESS_STATUS status = set_move(board, player_tools[tool_idx], tool_moves->valid_pos[move_index]);
					if(status != CHESS_STATUS_SUCCESS){
						return res;
					}

				MOVE_COMMAND current_move = suggest_min_move_ab(board,depth - 1,alpha,beta);

				if(current_move.value > res.value|| (current_move.value == INT_MIN && res.value == INT_MIN)){
					alpha = res.value = current_move.value;
					res.from = board->undo_data[board->curr_hist_pos]->from;
					res.to = tool_moves->valid_pos[move_index];
				}


				status = undo_move(board);
				if(status != CHESS_STATUS_SUCCESS)
					return res;

				if(beta <= alpha){
					pron_move = true;
					break;
				}
			}
			destroy_legit_moves(tool_moves);

		}
			if(pron_move)
				break;

	}

	return res;

}

MOVE_COMMAND suggest_min_move_ab(game_board* board, int depth, int alpha,int beta){
	MOVE_COMMAND res = {(tool_pos){EMPTY_ENTRY,{0,0}},(tool_pos){EMPTY_ENTRY,{0,0}},INT_MAX,alpha,beta};
	tool_pos* player_tools = board->black_tools;
	CHESS_STATUS winner = game_state(board);

	if(winner == CHESS_STATUS_BLACK_IN_MATE || winner == CHESS_STATUS_WHITE_IN_MATE){
		return res;
	}

	if(depth == 0){
		res.value = evaluate_board(board);
		return res;
	}

	bool pron_move = false;
	legit_moves* tool_moves = NULL;
	for (int tool_idx = 0; tool_idx < MAX_TOOLS; ++tool_idx) {
		if(player_tools[tool_idx].tool_type != EMPTY_ENTRY){
			tool_moves = valid_moves_array(board, player_tools[tool_idx].pos, get_validator(player_tools[tool_idx].tool_type));
			for (int move_index = 0; move_index < tool_moves->moves_num; ++move_index) {
				CHESS_STATUS status = set_move(board, player_tools[tool_idx], tool_moves->valid_pos[move_index]);
				if(status != CHESS_STATUS_SUCCESS){
					return res;
				}

				MOVE_COMMAND current_move = suggest_max_move_ab(board,depth - 1,alpha,beta);
				if(current_move.value < res.value || (current_move.value == INT_MAX && res.value == INT_MAX)){
					beta = res.value = current_move.value;
					res.from = board->undo_data[board->curr_hist_pos]->from;
					res.to = tool_moves->valid_pos[move_index];
				}


				status = undo_move(board);
				if(status != CHESS_STATUS_SUCCESS)
					return res;


				if(beta <= alpha){
					pron_move = true;
					break;
				}
				}
			destroy_legit_moves(tool_moves);
			}
		if(pron_move)
			break;

		}

	return res;

}
