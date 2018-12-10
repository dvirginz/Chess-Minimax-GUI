/*
 * console_game_flow.c
 *
 *  Created on: Aug 13, 2017
 *      Author: dvginzbu
 */

#include "console_game_flow.h"
#include "Tools.h"
#include "suggest_move.h"
#include "XML.h"

int settings_phase(){
	bool game_is_alive = true;
	do{
		char *messages = start_settings();
		bool settings_mode = true;
		int game_mode = 1,difficulty = 2, user_color = 1;
		game_board* game = NULL;
		while(settings_mode){
			COMMAND_s command = chess_parser_parse_line(messages);
			switch(command.cmd){
			case GAME_MODE:
				game_mode_message(command, &game_mode);
				break;
			case PRINT_SETTINGS:
				print_settings_message(command, game_mode,difficulty,user_color);
				break;
			case DIFFICULTY:
				difficulty_message(command, &difficulty, game_mode);
				break;
			case USER_COLOR:
				user_color_message(command, &user_color, game_mode);
				break;
			case DEFAULT:
				default_message(command, &game_mode, &difficulty, &user_color);
				break;
			case LOAD:
				load_message(command, &game_mode, &difficulty, &user_color, &game);
				break;
			case QUIT:
				quit_message(command, messages, game);
				break;
			case START:
				free(messages);
				settings_mode = false;
				start_message(command, game_mode,difficulty, user_color, game);
				break;
			default:
				if(command.arg_str){free(command.arg_str);}
				break;
			}
			if(settings_mode)
				fgets(messages,MAX_STRING+1,stdin);
		}
	}while(game_is_alive);
	return 0;
}


char* start_settings(){
	char* str = (char*)malloc((MAX_STRING+1)*sizeof(char));
	if(!str){
		//if the allocation failed, we finish the game - no memory to free.
		return NULL;
	}
	printf("Specify game setting or type 'start' to begin a game with the current setting:\n");
	fgets(str,MAX_STRING+1,stdin);
	return str;
}

void game_mode_message(COMMAND_s command,int* game_mode){
	if(command.validArg&&(command.arg_num == 1 ||command.arg_num == 2)){
		*game_mode = command.arg_num;
		printf("Game mode is set to %s\n",(command.arg_num == 1)? "1 player": "2 players");
	}else{
		printf("Wrong game mode\n");
	}
}

void difficulty_message(COMMAND_s command,int* difficulty, int game_mode){
	if(command.validArg && (game_mode == 1)){
	if(command.arg_num >= 1 &&command.arg_num <= 5){
		*difficulty = command.arg_num;
	}else{
		printf("Wrong difficulty level. The value should be between 1 to 5\n");
	}
	}
}

void user_color_message(COMMAND_s command,int* user_color, int game_mode){
	if(command.validArg && game_mode == 1){
		if(command.arg_num == 1 ||command.arg_num == 0)
			*user_color = command.arg_num;
	}
}

void load_message(COMMAND_s command,int* game_mode,int* difficulty, int* user_color,game_board** game){
	if(command.validArg){
		xml_game* game_xml = load_from_file(command.arg_str);
		free(command.arg_str);
		if(!game_xml){
			printf("Error: File doesn't exist or cannot be opened\n");
		}else{
			*game_mode = game_xml->game_mode;
			*difficulty = game_xml->difficulty;
			*user_color = game_xml->user_color;
			*game = game_xml->game;
			(*game)->p_type = PC;
			destroy_xml_game(game_xml);
		}
	}

}


void default_message(COMMAND_s command,int* game_mode,int* difficulty, int* user_color){
	if(command.validArg){
		*game_mode = 1;
		*difficulty = 2;
		*user_color = 1;
	}
}

void print_settings_message(COMMAND_s command, int game_mode,int difficulty, int user_color){
	if(command.validArg){
	switch(game_mode){
	case 1:
		printf("SETTINGS:\n"
				"GAME_MODE: 1\n"
				"DIFFICULTY_LVL: %d\n"
				"USER_CLR: %s\n",
				difficulty,(user_color == 0)? "BLACK": "WHITE");
		break;
	case 2:
		printf("SETTINGS:\n"
				"GAME_MODE: 2\n");
		break;
	}
	}
}

void quit_message(COMMAND_s command, char* message, game_board* game){
	if(command.validArg){
		printf("Exiting...\n");
		destroy_game(game);
		free(message);
		exit(0);
	}
}

void start_message(COMMAND_s command, int game_mode, int difficulty,int user_color,game_board* game){
	if(command.validArg){
		if(!game){
			game = create_game(difficulty);
			// one player phase choose who plays based on the previous player
			if(user_color == 0)
				game->p_type = HUMAN;
			else
				game->p_type = PC;
		}
		if(game_mode == 1)
			one_player_game_phase(game, user_color);
		else
			two_player_game_phase(game);
	}
}

CHESS_STATUS user_turn_manager(game_board* game,int game_mode, player_color p_color){
	CHESS_STATUS status = print_board_console(game);
	if (status != CHESS_STATUS_SUCCESS){
		return status;
	}
	game->p_type = HUMAN;

	char* str = (char*)malloc((MAX_STRING+1)*sizeof(char));
	if(!str){
		destroy_game(game);
		return CHESS_STATUS_MALLOC_ERROR;
	}
	bool move_command = false;
	COMMAND_s command = {.validArg = false};
	do{
	printf("%s player - enter your move:\n", (game->curr_player == BLACK_PLAYER)? "black": "white");
	fgets(str,MAX_STRING+1,stdin);
	command = chess_parser_parse_line(str);
	switch(command.cmd) {
		case MOVE:
			status = move_message(command, game);
			move_command = true;
			break;
		case CASTLE:
			status = castle_message(command, game);
			move_command = true;
			break;
		case GET_MOVES:
			status = get_moves_message(command, game);
			break;
		case SAVE:
			status = save_message(command, game,game_mode, p_color);
			break;
		case UNDO:
			if (game_mode == 1) {
				status = undo_message(command, game);
				break;
			} else {
				printf("Undo command not available in 2 players mode\n");
				status = CHESS_STATUS_INVALID_ARGUMENT;
			}
			break;

		case RESET:
			status = reset_message(command, game);
			move_command = true;
			break;
		case QUIT:
			quit_message(command,str,game);
			break;

		default:
			//invalid_line_message(str);
			status = CHESS_STATUS_INVALID_ARGUMENT;
	}
	}while(command.validArg == false || status == CHESS_STATUS_INVALID_ARGUMENT || !move_command);
	free(str);
	return status;
}

CHESS_STATUS pc_turn_manager(game_board* game){
	if(!game){
		return CHESS_STATUS_MALLOC_ERROR;
	}
	game->p_type = PC;
	MOVE_COMMAND move = suggest_move(game, (game->difficulty == 5)? 4:game->difficulty);

	if(abs(move.from.pos.column-move.to.pos.column) == 2 && (move.from.tool_type ==WHITE_KING || move.from.tool_type == BLACK_KING)){
		position rook_castle_pos;
		if(move.to.pos.column == 6)
			rook_castle_pos = (position){7,5};
		else if(move.to.pos.column == 6)
			rook_castle_pos = (position){7,3};
		printf("Computer: castle King at <%d,%c> and Rook at <%d,%c>\n",move.from.pos.row + 1,move.from.pos.column+'A',rook_castle_pos.row + 1,rook_castle_pos.column+'A');
	}else if((game->board[move.from.pos.row][move.from.pos.column] == BLACK_PAWN && move.to.pos.row == 0) ||
			(game->board[move.from.pos.row][move.from.pos.column] == WHITE_PAWN && move.to.pos.row == 7)){
		print_promotion_from_pc(move.from.pos,move.to.pos,move.to.tool_type);
	}else{
		print_move_of_tool(game,move.from.pos, move.to.pos);
	}

	CHESS_STATUS status = set_move(game, move.from, move.to);

	if(status != CHESS_STATUS_SUCCESS){
		return CHESS_STATUS_MALLOC_ERROR;
	}
	return CHESS_STATUS_SUCCESS;
}

CHESS_STATUS one_player_game_phase(game_board* game, int user_color){
	CHESS_STATUS winner = CHESS_STATUS_NORMAL, status;
	player_color p_color = (user_color == 1)? WHITE_PLAYER : BLACK_PLAYER;

	while((winner = game_state(game)) == CHESS_STATUS_NORMAL){


		if(is_check(game,game->curr_player)){
			if(game->p_type == PC)
				printf("Check!\n");
			else
				printf("Check: %s King is threatened!\n",(game->curr_player == BLACK_PLAYER)? "black": "white");
		}

		if(game->p_type == PC){
			status = user_turn_manager(game, 1, p_color);
			if(status != CHESS_STATUS_SUCCESS)
				break;

		}else{
			status = pc_turn_manager(game);
			if(status != CHESS_STATUS_SUCCESS)
				break;
		}
	}

	if(winner == CHESS_STATUS_BLACK_IN_MATE){
		printf("Checkmate! white player wins the game\n");
		destroy_game(game);
		exit(0);
	}
	if(winner == CHESS_STATUS_WHITE_IN_MATE){
		printf("Checkmate! black player wins the game\n");
		destroy_game(game);
				exit(0);
	}
	if(winner == CHESS_STATUS_TIE){
		printf("The game is tied\n");
		destroy_game(game);
		exit(0);
	}
	return status;
}

void two_player_game_phase(game_board* game){
	CHESS_STATUS winner = CHESS_STATUS_NORMAL, status;
	while((winner = game_state(game)) == CHESS_STATUS_NORMAL){
		if(is_check(game,game->curr_player)){
			printf("Check: %s King is threatened!\n",(game->curr_player == BLACK_PLAYER)? "black": "white");
		}

		status = user_turn_manager(game, 2, game->curr_player);
		if(status != CHESS_STATUS_SUCCESS)
			break;
	}

	if(winner == CHESS_STATUS_BLACK_IN_MATE){
		printf("Checkmate! white player wins the game\n");
		destroy_game(game);
				exit(0);
	}
	if(winner == CHESS_STATUS_WHITE_IN_MATE){
		printf("Checkmate! black player wins the game\n");
		destroy_game(game);
				exit(0);
	}
	if(winner == CHESS_STATUS_TIE){
		printf("The game is tied\n");
		destroy_game(game);
		exit(0);
	}
}

CHESS_STATUS move_message(COMMAND_s command,game_board* game){
	if(command.validArg){
		position from = {command.from.row - 1,command.from.column - 1};
		position to = {command.to.row - 1,command.to.column - 1};
		if(from.row > BOARD_MAX_INDEX || from.column > BOARD_MAX_INDEX || to.row > BOARD_MAX_INDEX ||
		to.column > BOARD_MAX_INDEX ||from.row < 0|| from.column < 0|| to.row < 0||to.column < 0){
			printf("Invalid position on the board\n");
			return CHESS_STATUS_INVALID_ARGUMENT;
		} else if(!(isupper(game->board[from.row][from.column]) && game->curr_player == BLACK_PLAYER) &&
				!(islower(game->board[from.row][from.column]) && game->curr_player == WHITE_PLAYER)){
			printf("The specified position does not contain your piece\n");
			return CHESS_STATUS_INVALID_ARGUMENT;
		}else{
			if((game->board[from.row][from.column] == WHITE_KING || game->board[from.row][from.column] == BLACK_KING)
					&& abs(to.column - from.column) > 1){
				printf("Illegal move\n");
				return CHESS_STATUS_INVALID_ARGUMENT;
			}
			tool_pos from_tool = {game->board[from.row][from.column], from};
			tool_pos to_tool = {game->board[to.row][to.column], to};
			valid_move move_type = valid_move_for_tool(game,from,to,get_validator(from_tool.tool_type));
			if(move_type == INVALID_MOVE){
				printf("Illegal move\n");
				return CHESS_STATUS_INVALID_ARGUMENT;
			}
			CHESS_STATUS status = set_move(game,from_tool,to_tool);
			if(status != CHESS_STATUS_SUCCESS) return status;
		}
		return CHESS_STATUS_SUCCESS;
	}
	printf("Invalid position on the board\n");
	return CHESS_STATUS_INVALID_ARGUMENT;
}

CHESS_STATUS castle_message(COMMAND_s command,game_board* game){
	if(command.validArg){
		position from = {command.from.row - 1,command.from.column - 1};

		if(from.row > BOARD_MAX_INDEX || from.column > BOARD_MAX_INDEX || from.row < 0|| from.column < 0){
			printf("Invalid position on the board\n");
			return CHESS_STATUS_INVALID_ARGUMENT;
		} else if(!(isupper(game->board[from.row][from.column]) && game->curr_player == BLACK_PLAYER) &&
				  !(islower(game->board[from.row][from.column]) && game->curr_player == WHITE_PLAYER)){
			printf("The specified position does not contain your piece\n");
			return CHESS_STATUS_INVALID_ARGUMENT;
		} else if(game->board[from.row][from.column] != BLACK_ROOK && game->board[from.row][from.column] != WHITE_ROOK){
			printf("Wrong position for a rook\n");
			return CHESS_STATUS_INVALID_ARGUMENT;
		}

		bool scw , scb , lcw , lcb;
		scw = scb = lcw = lcb = false;
		tool_pos from_tool,to_tool;
		valid_move castle_move;
		if(from.row == 7 && from.column == 7 && game->board[7][4] == BLACK_KING){
			position king_from = {7,4}, king_to = {7,6};
			castle_move = valid_move_for_tool(game,king_from,king_to,is_valid_king);
			scb = (castle_move == CASTLE_MOVE)? true:false;

			from_tool.tool_type = BLACK_KING; from_tool.pos = king_from;
			to_tool.tool_type = EMPTY_ENTRY; to_tool.pos = king_to;
		}
		if(from.row == 0 && from.column == 7 && game->board[0][4] == WHITE_KING){
			position king_from = {0,4}, king_to = {0,6};
			castle_move = valid_move_for_tool(game,king_from,king_to,is_valid_king);
						scw = (castle_move == CASTLE_MOVE)? true:false;

			from_tool.tool_type = WHITE_KING; from_tool.pos = king_from;
						to_tool.tool_type = EMPTY_ENTRY; to_tool.pos = king_to;
		}
		if(from.row == 7 && from.column == 0 && game->board[7][4] == BLACK_KING){
			position king_from = {7,4}, king_to = {7,2};
			castle_move = valid_move_for_tool(game,king_from,king_to,is_valid_king);
						lcb = (castle_move == CASTLE_MOVE)? true:false;

			from_tool.tool_type = BLACK_KING; from_tool.pos = king_from;
				to_tool.tool_type = EMPTY_ENTRY; to_tool.pos = king_to;
		}

		if(from.row == 0 && from.column == 0 && game->board[0][4] == WHITE_KING){
			position king_from = {0,4}, king_to = {0,2};
			castle_move = valid_move_for_tool(game,king_from,king_to,is_valid_king);
			lcw = (castle_move == CASTLE_MOVE)? true:false;

			from_tool.tool_type = WHITE_KING; from_tool.pos = king_from;
			to_tool.tool_type = EMPTY_ENTRY; to_tool.pos = king_to;
		}


		if(!scw && !scb && !lcw && !lcb){
			printf("Illegal castling move\n");
			return CHESS_STATUS_INVALID_ARGUMENT;
		}


		CHESS_STATUS status = set_move(game,from_tool,to_tool);
		if(status != CHESS_STATUS_SUCCESS){
			printf("Illegal castling move\n");
			return CHESS_STATUS_INVALID_ARGUMENT;
		}

		return CHESS_STATUS_SUCCESS;
	}
	printf("Invalid position on the board\n");
	return CHESS_STATUS_INVALID_ARGUMENT;
}





CHESS_STATUS get_moves_message(COMMAND_s command,game_board* game){
	if(command.validArg){
		position from = {command.from.row - 1,command.from.column - 1};
		char right_castle_buffer[MAX_LINE_LENGTH] = ""; char left_castle_buffer[MAX_LINE_LENGTH] = "";
		if(from.row > BOARD_MAX_INDEX || from.column > BOARD_MAX_INDEX ||from.row < 0|| from.column < 0){
			printf("Invalid position on the board\n");
		} else if(!(isupper(game->board[from.row][from.column]) && game->curr_player == BLACK_PLAYER) &&
				!(islower(game->board[from.row][from.column]) && game->curr_player == WHITE_PLAYER)){
			printf("The specified position does not contain your piece\n");
		}else{
			legit_moves* legit_arr = valid_moves_array(game, from,get_validator(game->board[from.row][from.column]));
			for (int i = 0; i < legit_arr->moves_num; ++i) {
				if(game->difficulty <=2){
					handle_castle_moves(from, right_castle_buffer,
							left_castle_buffer, game, legit_arr->valid_pos[i].pos);
					if (game->board[from.row][from.column] == WHITE_KING
								|| game->board[from.row][from.column] == BLACK_KING) {
							valid_move move_status = is_valid_king(game, from,
									legit_arr->valid_pos[i].pos);
							if (move_status == CASTLE_MOVE) {
								continue;
							}}

					if((game->board[legit_arr->valid_pos[i].pos.row][legit_arr->valid_pos[i].pos.column] != EMPTY_ENTRY) &&
							(is_catchable_after_move(game,from, legit_arr->valid_pos[i].pos))){
						printf("<%d,%c>*^",legit_arr->valid_pos[i].pos.row + 1,legit_arr->valid_pos[i].pos.column + 'A');
					}else if(game->board[legit_arr->valid_pos[i].pos.row][legit_arr->valid_pos[i].pos.column] != EMPTY_ENTRY){
						printf("<%d,%c>^",legit_arr->valid_pos[i].pos.row + 1,legit_arr->valid_pos[i].pos.column + 'A');
					}else if(is_catchable_after_move(game,from, legit_arr->valid_pos[i].pos)){
						printf("<%d,%c>*",legit_arr->valid_pos[i].pos.row + 1,legit_arr->valid_pos[i].pos.column + 'A');
					}
				else{
					printf("<%d,%c>",legit_arr->valid_pos[i].pos.row + 1,legit_arr->valid_pos[i].pos.column + 'A');
				}
			}else{
				//printf("<%d,%c>",legit_arr->valid_pos[i].pos.row + 1,legit_arr->valid_pos[i].pos.column + 'A');
			}
			if((game->board[from.row][from.column] == WHITE_PAWN && legit_arr->valid_pos[i].pos.row == 7) ||
						(game->board[from.row][from.column] == BLACK_PAWN && legit_arr->valid_pos[i].pos.row == 0)){
					// if it is a pawn promotion move, show only one move
					i += 4;
				}
			}printf("%s",left_castle_buffer); printf("%s", right_castle_buffer);
			printf("\n");
			destroy_legit_moves(legit_arr);
		}

		return CHESS_STATUS_SUCCESS;
	}
	printf("Invalid position on the board\n");
	return CHESS_STATUS_INVALID_ARGUMENT;
}

CHESS_STATUS save_message(COMMAND_s command,game_board* game,int game_mode, player_color p_color){
	if(command.validArg){
		xml_game* new_xml_game = create_xml_game(game, game->difficulty,game_mode,p_color);
		int save_res = write_to_file(command.arg_str,new_xml_game);
		destroy_xml_game(new_xml_game);
		free(command.arg_str);
		if(save_res !=0){
			printf("File cannot be created or modified\n");
			return CHESS_STATUS_MALLOC_ERROR;
		}
		return CHESS_STATUS_SUCCESS;
			}
	return CHESS_STATUS_INVALID_ARGUMENT;
}

CHESS_STATUS undo_message(COMMAND_s command,game_board* game){
	if(command.validArg){
		if(game->curr_hist_len > 1){
			CHESS_STATUS status = undo_move(game);
			if(status == CHESS_STATUS_NO_HISTORY){
				return CHESS_STATUS_NO_HISTORY;
			}else{
						undo_move(game);
			}
			print_board_console(game);
			return CHESS_STATUS_SUCCESS;
		}else{
			printf("Empty history, move cannot be undone\n");
			return CHESS_STATUS_NO_HISTORY;
		}
	}
	return CHESS_STATUS_INVALID_ARGUMENT;
}

CHESS_STATUS reset_message(COMMAND_s command,game_board* game){
	if(command.validArg){
		destroy_game(game);
		printf("Restarting...\n");
		return CHESS_STATUS_RESTART;
	}
	return CHESS_STATUS_INVALID_ARGUMENT;
}
