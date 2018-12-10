/*
 * XML.h
 *
 *  Created on: 9 Aug 2017
 *      Author: GalL
 */

#ifndef XML_H_
#define XML_H_
#include <stdio.h>
#include "common.h"
#define BUFFER_SIZE 1024
typedef struct xml_game_t{
	game_board* game;
	int difficulty;
	int game_mode;
	int user_color;
}xml_game;

/**
 * creates a new xml_game, returns a pointer to it.
 * @param game  - the game_board
 * @param diff - the games difficulty.
 * @param mode - the game mode
 * @param user_col the users color
 * @return - on success - pointer to a new xml_game. on failure - NULL.
 */
xml_game* create_xml_game(game_board* game,int diff,int mode,int user_col);

/**
 * Destroys an xml_game
 * @param game  - pointer to an xml_game
 * @return -void
 * @action destroys this xml_game (i.e - frees the space), WITHOUT DESTROYING THE BOARD
 */
void destroy_xml_game(xml_game* game);

/**
 * Destroys an xml_game  AND IT'S ASSOCIATED BOARD
 * @param game  - pointer to an xml_game
 * @return -void
 * @action destroys this xml_game and it's board
 */
void destroy_xml_game_and_board(xml_game* game);

/**
 * Reads and interprets an xml_game from file.
 * @param filename  - name of (XML) file to read from
 * @return -on success - pointer to an xml_game created based on the input file. O/W - NULL
  */
xml_game* load_from_file(char* filename);

/**
 * Reads a file to memory
 * @param filename  - name of file to read (AN ASSUMPTION IS MADE THAT THE FILE CAN BE READ IN WHOLE TO MEMORY)
 * @return -pointer to a string with the file's content, NULL on failure
  */
char* read_file(char* filename);

/**
 * Updates a line to be identical to the input string.
 * @param src  - a game board
 * @param str - the lines desired content.
 * @param line_num - the lines number
 * @return -void
 * @action - chaanges the associated line in src's board to be identical to src. DOES NOT UPDATE THE TOOLS ARRAY!!!!!
 *
 * @pre - BY CONTRACT, NO CHECK IS PERFORMED ON str, IT'S LENGTH AND/OR IT'S CONTENT, the same for line_num.
  */
void update_line(game_board* src, char* str, int line_num);

/**
 * Writes an xml_game to file.
 * @param filename  - name of (XML) file to write to
 * @param src - a pointer to the xml_game to write,
 * @return -on success -0, on failure - a negative integer.
  */
int write_to_file(char* filename,xml_game* src);

/**
 * Writes the board's content to a file.
 * @param ofp  - POINTER to a FILE write to
 * @param src - a pointer to the xml_game from which the board is taken.
 * @return - void
 * @action - writes the board content to the file.
  */
void write_board_to_file(FILE* ofp,xml_game* src);
//input - POINTER to a FILE write to,and a pointer to an XML_GAME
// action: write the parameters (current player,diff level etc.)
/**
 * Writes the games paramteres to  a file.
 * @param ofp  - POINTER to a FILE write to
 * @param src - a pointer to the xml_game from which the board and it's parameters are taken.
 * @return - void
 * @action - writes the games parameters to the file.
  */
void write_parameters_to_file(FILE* ofp,xml_game* src);

/**
 * Writes the opening to the XML (version, game tag etc)
 * @param ofp  - POINTER to a FILE write to
 * @return - void
 *
 */
void write_opening_to_file(FILE* ofp);

/**
 * Writes a string tag to file.
 * @param ofp  - POINTER to a FILE write to
 * @param tab -tabing (if necessary)
 * @param tag_name - name of the tag
 * @param content - content of tag
 * @return - void
 * @action writes a tag, preceeded by the string pointed by "tab", with tag_name and content
 *
 */
void write_str_tag_to_file(FILE* ofp,char* tab,char* tag_name,char* content);

/**
 * Writes an int tag to file.
 * @param ofp  - POINTER to a FILE write to
 * @param tab -tabing (if necessary)
 * @param tag_name - name of the tag
 * @param content - content of tag, as int
 * @return - void
 * @action writes a tag, preceeded by the string pointed by "tab", with tag_name and content
 *
 */
void write_int_tag_to_file(FILE* ofp,char* tab, char* tag_name,int content);

/**
 * Writes a numbered int tag to file.
 * @param ofp  - POINTER to a FILE write to
 * @param tab -tabing (if necessary)
 * @param tag_name - name of the tag
 * @param content - content of tag, as int
 * @param number - the tag's number
 * @return - void
 * @action writes a tag, preceeded by the string pointed by "tab",
 *  with tag_name concatenated to the char form of "number" and content
 *
 */
void write_numbered_int_tag_to_file(FILE* ofp,char* tab, char* tag_name,int content,int number);

/**
 * Writes a numbered str tag to file.
 * @param ofp  - POINTER to a FILE write to
 * @param tab -tabing (if necessary)
 * @param tag_name - name of the tag
 * @param content - content of tag
 * @param number - the tag's number
 * @return - void
 * @action writes a tag, preceeded by the string pointed by "tab",
 *  with tag_name concatenated to the char form of "number" and content
 *
 */
void write_numbered_str_tag_to_file(FILE* ofp,char* tab, char* tag_name,char* content,int number);

/**
 * Writes the "general" tag to a file (in this Project, used for castling parameters).
 * @param ofp  - POINTER to a FILE write to
 * @param src - a pointer to the xml_game from which the board's castling paramters.
 * @return - void
 * @action writes the general tag to the file, with the castling sub tag.
 */
void write_general_to_file(FILE* ofp,xml_game* src);
#endif /* XML_H_ */
