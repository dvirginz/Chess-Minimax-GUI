CC = gcc
OBJS = chess_parser.o array.o Tools.o GameBoard.o main.o console_game_flow.o XML.o suggest_move.o evaluation.o GUI_common.o Main_window.o\
 Game_window.o Game_dialog.o Load_window.o gui_game_flow.o Button.o Window.o Widget.o
EXEC = chessprog
CC_COMP_FLAG = -ggdb  -std=c99 -Wall -Wextra \
-Werror -pedantic-errors
SDL_COMP_FLAG =  -I/usr/local/lib/sdl_2.0.5/include/SDL2 -D_REENTRANT
SDL_LIB = -L/usr/local/lib/sdl_2.0.5/lib -Wl,-rpath,/usr/local/lib/sdl_2.0.5/lib -Wl,--enable-new-dtags -lSDL2 -lSDL2main
COMP_FLAG = -ggdb -std=c99 -Wall -Wextra -g \
-Werror -pedantic-errors
$(EXEC): $(OBJS)
	$(CC) $(OBJS) $(SDL_LIB) -o $@
all: $(EXEC)
.PHONY:all
XML.o: XML.c XML.h GameBoard.h
	$(CC) $(COMP_FLAG) -c $*.c
main.o: main.c console_game_flow.h gui_game_flow.h
	$(CC) $(COMP_FLAG) $(SDL_COMP_FLAG) -c $*.c
array.o: array.c array.h Tools.c
	$(CC) $(COMP_FLAG) -c $*.c
Tools.o: Tools.c Tools.h common.h
	$(CC) $(COMP_FLAG) -c $*.c 
GameBoard.o: GameBoard.c GameBoard.h Tools.h
	$(CC) $(COMP_FLAG) -c $*.c
suggest_move.o: suggest_move.c suggest_move.h Tools.h GameBoard.h common.h evaluation.h
	$(CC) $(COMP_FLAG) -c $*.c
evaluation.o: evaluation.c evaluation.h GameBoard.h common.h
	$(CC) $(COMP_FLAG) -c $*.c
chess_parser.o: chess_parser.c chess_parser.h common.h
	$(CC) $(COMP_FLAG) -c $*.c
console_game_flow.o: console_game_flow.c console_game_flow.h common.h chess_parser.h GameBoard.h XML.h suggest_move.h
	$(CC) $(COMP_FLAG) -c $*.c
GUI_common.o: GUI_common.c GUI_common.h
	$(CC) $(COMP_FLAG) $(SDL_COMP_FLAG) -c $*.c
Main_window.o: Main_window.c Main_window.h Window.h Button.h Widget.h
	$(CC) $(COMP_FLAG) $(SDL_COMP_FLAG) -c $*.c
Game_window.o: Game_window.c Game_window.h Main_window.c Window.h Button.h suggest_move.h XML.h Tools.h GameBoard.h 
	$(CC) $(COMP_FLAG) $(SDL_COMP_FLAG) -c $*.c
Game_dialog.o: Game_dialog.c Game_dialog.h  Window.h Button.h GameBoard.h 
	$(CC) $(COMP_FLAG) $(SDL_COMP_FLAG) -c $*.c
Load_window.o: Load_window.c Load_window.h Game_dialog.h Window.h Widget.h Button.h XML.h
	$(CC) $(COMP_FLAG) $(SDL_COMP_FLAG) -c $*.c
gui_game_flow.o: gui_game_flow.c gui_game_flow.h Game_window.h Game_dialog.h Load_window.h Main_window.h 
	$(CC) $(COMP_FLAG) $(SDL_COMP_FLAG) -c $*.c
Button.o: Button.c Button.h Widget.h
	$(CC) $(COMP_FLAG) $(SDL_COMP_FLAG) -c $*.c
Window.o: Window.c Window.h Widget.h
	$(CC) $(COMP_FLAG) $(SDL_COMP_FLAG) $(SDL_LIB) -c $*.c
Widget.o: Widget.c Widget.h GUI_common.h
	$(CC) $(COMP_FLAG) $(SDL_COMP_FLAG) $(SDL_LIB) -c $*.c
clean:
	rm -f *.o $(EXEC)