/*
 * Test_XML.c

 *
 *  Created on: 10 Aug 2017
 *      Author: GalL
 */
#include "XML.h"
#include "GameBoard.h"
#include <stdio.h>
#include <stdlib.h>
int main1(int argc,char** argv){
	if(argc<3){
		exit(0);
	}
	game_board* gal = create_game(10);
	xml_game* x_game = create_xml_game(gal,1,1,1);
	write_to_file(argv[1],x_game);
	xml_game* tmp = load_from_file(argv[1]);
	if(!tmp){
		printf("ERROR");
	}
	write_to_file(argv[2],tmp);
	destroy_xml_game_and_board(tmp);
	destroy_xml_game_and_board(x_game);
	return 0;
}
