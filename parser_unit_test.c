/*
 * parser_unit_test.c
 *
 *  Created on: Aug 10, 2017
 *      Author: dvginzbu
 */


#include "chess_parser.h"
#include "unit_test_util.h"

static bool chess_parser_test() {
	COMMAND_s cmd;
	cmd = chess_parser_parse_line("game_mode 1");
	ASSERT_TRUE(cmd.cmd == GAME_MODE && cmd.validArg && cmd.arg_num == 1);
	cmd = chess_parser_parse_line("default	17\n");
	ASSERT_TRUE(cmd.cmd == DEFAULT && cmd.validArg);

	cmd = chess_parser_parse_line("	move <7,2> to <3,C>\n");
	ASSERT_TRUE(!cmd.validArg);

	cmd = chess_parser_parse_line("	move <7,A> to <3,B>\n");

		ASSERT_TRUE(cmd.cmd == MOVE && cmd.validArg &&
				cmd.from.column  == 7 &&cmd.from.row  == 1 &&cmd.to.column  == 3 &&cmd.to.row  == 2);

	cmd = chess_parser_parse_line("get_moves <7,B> to <3,6>\n");
	ASSERT_TRUE(cmd.cmd == GET_MOVES && cmd.validArg &&
				cmd.from.column  == 7 &&cmd.from.row  == 2);

	cmd = chess_parser_parse_line("aDd_disc 17");
	ASSERT_TRUE(cmd.cmd == INVALID_LINE && !cmd.validArg);
	return true;
}
int main3() {
	RUN_TEST(chess_parser_test);
	return 0;
}
