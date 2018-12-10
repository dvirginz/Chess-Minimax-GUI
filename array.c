/*
 * array.c
 *
 *  Created on: Aug 4, 2017
 *      Author: dvginzbu
 */

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>
#include "array.h"
#include "Tools.h"

void init_tools_array(game_board* board){
	int black_num = 0, white_num = 0;
	for (int row = 0; row < BOARD_SIZE; ++row) {
		for (int column = 0; column < BOARD_SIZE; ++column) {
			char tool_type = board->board[column][row];
			position pos = {column, row};
			tool_pos tool; tool.pos = pos; tool.tool_type = tool_type;

			if(isupper(board->board[column][row])){
				board->black_tools[black_num] = tool;
				black_num++;
			}else if(islower(board->board[column][row])){
				board->white_tools[white_num] = tool;
				white_num++;
			}
		}
	}
}

bool compare_pos(position player1, position player2){
	if(player1.row == player2.row && player1.column == player2.column)
		return true;
	return false;
}

bool compare_tools(tool_pos player1, tool_pos player2){
	if(compare_pos(player1.pos,player2.pos) && player1.tool_type == player2.tool_type)
		return true;
	return false;
}

int get_tool_in_array(game_board* board, tool_pos tool){
	for (int var = 0; var < MAX_TOOLS; ++var) {
		tool_pos black = board->black_tools[var];
		tool_pos white = board->white_tools[var];
			if(compare_tools(black, tool)||compare_tools(white,tool))
					return var;
		}
	return -1;
}

void del_tool_array(game_board* board, tool_pos tool){
	int index = get_tool_in_array(board, tool);
	if(index == -1)
		return;
	if(isupper(tool.tool_type))
		board->black_tools[index].tool_type = EMPTY_ENTRY;
	if(islower(tool.tool_type))
		board->white_tools[index].tool_type = EMPTY_ENTRY;
}

void add_tool_array(game_board* board, tool_pos tool){
	tool_pos *tools_array;
	if(isupper(tool.tool_type))
		tools_array = board->black_tools;
	if(islower(tool.tool_type))
		tools_array = board->white_tools;

	for (int index = 0; index < MAX_TOOLS; ++index) {
		if(tools_array[index].tool_type == EMPTY_ENTRY){
			tools_array[index].tool_type = tool.tool_type;
			tools_array[index].pos = tool.pos;
			return;
		}
	}
}
