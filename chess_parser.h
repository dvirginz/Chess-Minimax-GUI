/*
 * Chess_parser.h
 *
 *  Created on: Aug 6, 2017
 *      Author: dvginzbu
 */

#ifndef CHESS_PARSER_H_
#define CHESS_PARSER_H_

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "common.h"

//specify the maximum line length
#define MAX_LINE_LENGTH 1024


//a type used to represent a command in game state
typedef enum {
	GAME_MODE,
	DIFFICULTY,
	USER_COLOR,
	LOAD,
	DEFAULT,
	PRINT_SETTINGS,
	QUIT,
	START,
	MOVE,
	CASTLE,
	GET_MOVES,
	SAVE,
	UNDO,
	RESET,
	INVALID_LINE
} COMMAND;

typedef struct POSITION_INPUT_T{
	bool validArg;
	position pos;
}POSITION_INPUT;


//a new type that is used to encapsulate a parsed line
typedef struct command_t {
	COMMAND cmd;
	bool validArg; //is set to true if the line contains a valid argument
	int arg_num;
	position from,to; //for move and get_moves commands
	char* arg_str; // for I/O commands
} COMMAND_s;

/**
 * Checks if a specified string represents a valid integer. It is recommended
 * to use this function prior to calling the standard library function atoi.
 *
 * @return
 * true if the string represents a valid integer, and false otherwise.
 */
bool chess_parser_is_int(const char* str);

/**
 * parse the line from the input to the command, if extra parameters are expected
 * (e.g position, difficulty number etc.) the function parse them also
 *
 * @return
 * return value will be the command specified and it needed parameters, validarg == false on error
 */
COMMAND_s chess_parser_parse_line(const char* str);



#endif /* CHESS_PARSER_H_ */
