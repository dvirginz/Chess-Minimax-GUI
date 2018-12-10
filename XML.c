/*
 * XML.c
 *
 *  Created on: 9 Aug 2017
 *      Author: GalL
 */
#include "XML.h"
#include <stdlib.h>
#include <string.h>
#include "GameBoard.h"
xml_game* create_xml_game(game_board* game,int diff,int mode,int user_col){
	//alloc space for result, verify success.
	xml_game* res = (xml_game*)malloc(sizeof(xml_game));
	if(!res){ return NULL; }
	res->game = game;
	res->difficulty = diff;
	res->game_mode = mode;
	res->user_color = user_col;
	return res;
}
void destroy_xml_game(xml_game* game){
	free(game);
}
void destroy_xml_game_and_board(xml_game* game){
	if(game){
		destroy_game(game->game);
		free(game);
	}
}

char* read_file(char* filename){
	//open the file
	FILE* ifp = fopen(filename,"r");
	if(!ifp){

		return NULL;}
	//calculate the file's size, alloc space.
	fseek(ifp,0,SEEK_END);
	long size = ftell(ifp);
	rewind(ifp);
	//alloc space for the contents+another char for the '\0'
	char* res = (char*)malloc(size+sizeof(char));
	//verify success
	if(!res){
		fclose(ifp);
		return NULL;
	}
	//read the file (binary, even though it's a text file) into res.
	fread(res,size,1,ifp);
	//string terminator
	res[size]='\0';
	fclose(ifp);
	return res;
}

void write_opening_to_file(FILE* ofp){
	char* open_str = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	fprintf(ofp,"%s",open_str);
	fprintf(ofp,"<game>\n");
	fflush(ofp);
}
void write_str_tag_to_file(FILE* ofp,char* tab,char* tag_name,char* content){
	fprintf(ofp,"%s<%s>%s</%s>\n",tab,tag_name,content,tag_name);
	fflush(ofp);
}
void write_int_tag_to_file(FILE* ofp,char* tab, char* tag_name,int content){
	fprintf(ofp,"%s<%s>%d</%s>\n",tab,tag_name,content,tag_name);
	fflush(ofp);
}
void write_numbered_int_tag_to_file(FILE* ofp,char* tab, char* tag_name,int content,int number){
	fprintf(ofp,"%s<%s%d>%d</%s%d>\n",tab,tag_name,number,content,tag_name,number);
	fflush(ofp);
}
void write_numbered_str_tag_to_file(FILE* ofp,char* tab, char* tag_name,char* content,int number){
	fprintf(ofp,"%s<%s%d>%s</%s%d>\n",tab,tag_name,number,content,tag_name,number);
	fflush(ofp);
}
void write_parameters_to_file(FILE* ofp,xml_game* src){
	//write the current player
	write_int_tag_to_file(ofp,"\t","current_turn",((src->game->curr_player== WHITE_PLAYER) ? 1:0));
	//write the game mode
	write_int_tag_to_file(ofp,"\t","game_mode",src->game_mode);
	//write difficulty
	write_int_tag_to_file(ofp,"\t","difficulty",src->difficulty);
	//write player color
	write_int_tag_to_file(ofp,"\t","user_color",(src->user_color== WHITE_PLAYER)?1:0);
}
void write_board_to_file(FILE* ofp,xml_game* src){
	fprintf(ofp,"\t<board>\n");
	//temp char* to hold the content of a row
	char tmp[GAME_BOARD_SIZE+1];
	//string terminator
	tmp[GAME_BOARD_SIZE]='\0';
	//write each line of the board, with two tab indentation, with upper row first
	for(int i=GAME_BOARD_SIZE;i>0;i--){
		for(int j=0;j<GAME_BOARD_SIZE;j++){
			tmp[j]=src->game->board[i-1][j];
		}
		write_numbered_str_tag_to_file(ofp,"\t\t","row_",tmp,i);
	}
	//close the board tag
	fprintf(ofp,"\t</board>\n");
}
void write_general_to_file(FILE* ofp,xml_game* src){
	fprintf(ofp,"\t<general>\n");
	char cas[5];
	cas[0] = src->game->short_castle_white == true? '1':'0';
	cas[1] = src->game->short_castle_black == true? '1':'0';
	cas[2] = src->game->long_castle_white == true? '1':'0';
	cas[3] = src->game->long_castle_black == true? '1':'0';
	cas[4] = '\0';
	write_str_tag_to_file(ofp,"\t\t","castling",cas);
	fprintf(ofp,"\t</general>\n");
}
int write_to_file(char* filename,xml_game* src){
	if(!src){
		return 0;
	}
	FILE* ofp = fopen(filename,"w+");
	if(!ofp){
		return -1;
	}
	//write the opening to the file.
	write_opening_to_file(ofp);
	//write parameters to file
	write_parameters_to_file(ofp,src);
	//write the board
	write_board_to_file(ofp,src);
	//write the <general>
	write_general_to_file(ofp,src);
	fprintf(ofp,"</game>\n");
	fclose(ofp);
	return 0;
}
int is_valid_content(char* str){
	//scan each char, if a "bad" one was found - return 0.
	while(str++){
		if(*str == '\n' || *str == '\r' || *str == '\t'){
			return 0;
		}
	}
	return 1;
}
void update_line(game_board* src, char* str, int line_num){
	if(!src){return;}
	for(int i = 0 ; i < GAME_BOARD_SIZE ; i++){
		src->board[line_num][i] = str[i];
	}
}
xml_game* load_from_file(char* filename){
	// get the files content into a string,verify success.
	char* cont = read_file(filename);
	char* orig_cont = cont;
	char* delim = "<>";
	if(!cont){
		return NULL;}
	//the board used in the output
	game_board* res_board =(game_board*) create_game(GAME_BOARD_HIST_SIZE);
	if(!res_board){
		printf("ERROR OCCURED WHILE CREATING BOARD");
		free(cont);
		return NULL;
	}
	//tokenizing the string with respect to the tags.
	char* curr_str = strtok(cont,delim);
	// search for "current_turn" tag:
	while (strcmp(curr_str,"current_turn")== 0 ? 0:1){
		curr_str = strtok(NULL,delim);
	}
	curr_str = strtok(NULL,delim);
	//get the current turn.
	int curr_turn = strtol(curr_str,NULL,10);
	res_board->curr_player = (curr_turn ==1 )?WHITE_PLAYER:BLACK_PLAYER;
	// search for "game_mode" tag:
	while (strcmp(curr_str,"game_mode") == 0?0:1){
		curr_str = strtok(NULL,delim);
	}
	curr_str = strtok(NULL,delim);
	//get the game mode
	int game_mode = strtol(curr_str,NULL,10);
	int difficulty = 2 ;
	int user_col = 1;
	//if this is the single player mode, search for user col and difficulty.
	if(game_mode == 1){
		// search for "difficulty" tag:
		while (strcmp(curr_str,"difficulty")==0 ? 0:1){
			curr_str = strtok(NULL,delim);
		}
		curr_str = strtok(NULL,delim);
		//get the difficulty.
		difficulty = strtol(curr_str,NULL,10);
		res_board->difficulty = difficulty;
		// search for "user_color" tag:
		while (strcmp(curr_str,"user_color") == 0? 0:1){
			curr_str = strtok(NULL,delim);
		}
		curr_str = strtok(NULL,delim);
		//get the user color
		user_col = strtol(curr_str,NULL,10);
	}
	char row[10] ;
	for(int i = GAME_BOARD_SIZE ; i >0 ; i--){
		sprintf(row,"row_%d",i);
		// search for "row_i" tag
		while (strcmp(curr_str,row) == 0? 0:1){
			curr_str = strtok(NULL,delim);
		}
		curr_str = strtok(NULL,delim);
		//get the user color
		update_line(res_board,curr_str,i-1);
	}
	//Retrieving castling parameters
	while (curr_str != NULL && (strcmp(curr_str,"castling") == 0? 0:1)){
		curr_str = strtok(NULL,delim);
	}
	curr_str = strtok(NULL,delim);
	if(curr_str != NULL){
		res_board->short_castle_white = strtol(&curr_str[0],NULL,10);
		res_board->short_castle_black = strtol(&curr_str[1],NULL,10);
		res_board->long_castle_white = strtol(&curr_str[2],NULL,10);
		res_board->long_castle_black = strtol(&curr_str[3],NULL,10);
	}else{
		res_board->short_castle_white = false;
		res_board->short_castle_black = false;
		res_board->long_castle_white = false;
		res_board->long_castle_black = false;
	}
	//tools have moved, update accordingly.
	update_tools_array(res_board);
	free(orig_cont);
	res_board->curr_player = (curr_turn ==1 )? WHITE_PLAYER : BLACK_PLAYER;
	//when the game is loaded, it's a humans turn.
	res_board->p_type = HUMAN ;
	xml_game* res = create_xml_game(res_board,difficulty,game_mode,user_col);
	if(!res){
		destroy_game(res_board);
		return NULL;
	}
	return res;
}

