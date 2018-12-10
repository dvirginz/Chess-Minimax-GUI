/*
 * Chess_parser.c
 *
 *  Created on: Aug 6, 2017
 *      Author: dvginzbu
 */

#include "chess_parser.h"
#define MAX_STRING 1024 //MAX LENGTH OF PATH TO BE EXPECTED.


bool chess_parser_is_int(const char* str){
	//check the first char is +,- or a digit
	if (!((char)*str == '+'|| (char)*str == '-' || isdigit((char)*str))){
		return false;
	}
	//rest of the chars must be decimal digits
	while(*(++str)){
		if(!isdigit((char)*str)){
			return false;
		}
	}
	return true;
}

POSITION_INPUT parse_position(const char* str){

	POSITION_INPUT parsed_position = {false,{0,0}};
	if(!str || str[0] != '<')
		return parsed_position;
	char* str2 = (char*)malloc((strlen(str)+1)*sizeof(char));
	strcpy(str2,str);
	char *first = strtok(str2+1,",");
	char *second = strtok(NULL, ">");
	for (unsigned int i = 0; i < strlen(first); ++i) {
		if(!isdigit(first[i]))
			return parsed_position;
	}
	for (unsigned int i = 0; i < strlen(second); ++i) {
		if(!isupper(second[i]))
			return parsed_position;
	}
	if(strlen(second) != 1){
		//if second is longer than 1 letter, it is out of range for sure.
		parsed_position.pos.column = 10;
	}else{
		parsed_position.pos.column = second[0] - 'A' + 1;
	}

	parsed_position.pos.row = atoi(first);
	parsed_position.validArg = true;
	free(str2);
	return parsed_position;
}



COMMAND_s chess_parser_parse_line(const char* str){

	char* str2 = (char*)malloc((strlen(str)+1)*sizeof(char));
	strcpy(str2,str);
	char* first;
	char* second;
	COMMAND_s a = { INVALID_LINE, false,0, {0,0},{0,0},NULL};
	first = strtok(str2," \t\n");
	if(first){
		second = strtok(NULL," \n\r\t");
	}
	else{
		free(str2);
		return a;
	}

	if(!strcmp(first,"game_mode")){
			a.cmd = GAME_MODE;
			if(second && chess_parser_is_int(second)){
				a.arg_num = (int)strtol(second,NULL,10);
				a.validArg = true;
			}
		}
	if(!strcmp(first,"difficulty")){
				a.cmd = DIFFICULTY;
				if(second && chess_parser_is_int(second)){
					a.arg_num = (int)strtol(second,NULL,10);
					a.validArg = true;
				}
			}
	if(!strcmp(first,"user_color")){
					a.cmd = USER_COLOR;
					if(second && chess_parser_is_int(second)){
						a.arg_num = (int)strtol(second,NULL,10);
						a.validArg = true;
					}
				}
	if(!strcmp(first,"load")){
				a.cmd = LOAD;
				if(second){
					char* file_name = (char*)malloc((strlen(second)+1)*sizeof(char));
						strcpy(file_name,second);
					a.arg_str = file_name;
					a.validArg = true;
				}
			}
	if(!strcmp(first,"default")){
		a.cmd = DEFAULT;
		a.validArg = true;
	}
	if(!strcmp(first,"print_setting")){
		a.cmd = PRINT_SETTINGS;
		a.validArg = true;
	}
	if(!strcmp(first,"quit")){
			a.cmd = QUIT;
			a.validArg = true;
		}
	if(!strcmp(first,"start")){
					a.cmd = START;
					a.validArg = true;
				}
	if(!strcmp(first,"move")){

		a.cmd = MOVE;
		char* third;
		char* fourth;
		if(second){
			third = strtok(NULL," \t\r\n");
			if(third && !strcmp(third,"to")){
				fourth = strtok(NULL," \t\r\n");
				if(fourth){
							POSITION_INPUT from = parse_position(second);
							POSITION_INPUT to = parse_position(fourth);
							if(from.validArg && to.validArg)
								a.validArg = true;
								a.from = from.pos;
								a.to = to.pos;
						}
			}
		}

	}
	if(!strcmp(first,"get_moves")){
		a.cmd = GET_MOVES;
		if(second){
			POSITION_INPUT from = parse_position(second);
			if(from.validArg){
				a.validArg = true;
				a.from = from.pos;
			}
		}
	}
	if(!strcmp(first,"castle")){
		a.cmd = CASTLE;
		if(second){
			POSITION_INPUT from = parse_position(second);
			if(from.validArg){
				a.validArg = true;
				a.from = from.pos;
			}
		}
	}

	if(!strcmp(first,"save")){
		a.cmd = SAVE;
		if(second){
			char* file_name = (char*)malloc((strlen(second)+1)*sizeof(char));
			strcpy(file_name,second);
			a.arg_str = file_name;
			a.validArg = true;
		}
	}
	if(!strcmp(first,"undo")){
		a.cmd = UNDO;
		a.validArg = true;
	}
	if(!strcmp(first,"reset")){
		a.cmd = RESET;
		a.validArg = true;
	}

	free(str2);
	return a;
}
