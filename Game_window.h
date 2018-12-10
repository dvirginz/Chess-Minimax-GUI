/*
 * Game_window.h
 *
 *  Created on: 9 Sep 2017
 *      Author: GalL
 */

#ifndef GAME_WINDOW_H_
#define GAME_WINDOW_H_
#include "Window.h"
#include "GameBoard.h"
#include "suggest_move.h"
typedef enum dialog_choices_t{
	CONTINUE_SAVE = 0,
	CANCEL = 1 ,
	CONTINUE_NO_SAVE = 2
}dialog_choices;
typedef enum drag_drop_states_t{
	DRAGGING,
	RELEASED
}drag_drop_states;
typedef struct game_window_t game_window;
struct game_window_t{
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture** pieces_tx;
	game_board* board;
	//simple widgets
	Widget** widgets;
	int widget_num;
	bool save_needed;
	bool board_locked;
	bool game_waiting;
	bool render_needed;
	player_color initial_player;
	player_type initial_p_type;
	drag_drop_states drag_drop_state;
	Sint32 drag_start_x;
	Sint32 drag_start_y;
};
/**
 * Creates a Window, where the data field is a pointer to a game_window, and returns a pointer to that Window.
 * @param window - Pointer to an SDL_Windpw to be used.
 * @param renderer - Pointer to an SDL_Renderer to be used.
 * @param board -pointer to game_board, which will be used as the board for this game.
 * @return - on success - pointer to a Window. on failure - NULL.
 */
Window* create_game_window(SDL_Window* window,SDL_Renderer* r, game_board* board);
/**
 * Updates the board on the screen, with respect to the current tool locations.
 * @param gw - Pointer to a game_Window
 * @return - on sucess, . on failure - a negative number.
 */
int update_gw_grid(game_window* gw);
/**
 * Creates the textures for the tools. 2 for each tool type (blakc background, white background).
 * @param gw - Pointer to a game_Window
 * @return - on success - $ret complies with the following: for a piece with code n ( as defined in get_piece_code),
 * it holds that $ret[2n-1] is a pointer to an SDL_Texture Representing the piece when positioned on WHITE Background.
 * similarly, $ret[2n] is analog, only for DRAK Background.
 */
SDL_Texture** create_gw_textures(SDL_Renderer* r);
/**
 * draws a game window
 * @param w - Pointer to a Window, for which the data field points to game_window.
 * @return - if succesfully drawn all necessary items, GUI_SUCCESS, O.W - an appropriate GUI_MESSAGE,
 * informative of the kind of failure.
 */
GUI_MESSAGE draw_gw(Window* w );
/**
 * manages the event handling for the game_window, routing events to widgets and other functions as necessary.
 * @param w - Pointer to a Window, for which the data field points to game_window.
 * @param e - pointer to the SDL_Event that was fired.
 * @return - void
 */
void handle_gw_event(Window* w  ,SDL_Event* e);
/**
 * Updates the screen when drag&drop begins, during the drag, and in the end. when the drag ends -
 * if it represents a valid move, set_move is called to perform the move.
 * @param gw - Pointer to a game_window.
 * @param e - pointer to the SDL_Event that was fired.
 * @return - void
 */
void update_drag(game_window* gw ,SDL_Event* e);
/**
  * Destroys a game_window - NULL SAFE.
  * @param src  - Pointer to Window, for which the data field is a pointer to a game_window.
  * @return void
  * @action - the game_board is destroyed, the renderer and the SDL_Window are not destroyed!
  */
void destroy_gw(Window* w);
/**
 * Creates the widgets for the game_window.
 * @param r - Pointer to an SDL_Renderer
 * @return - on success - an "array" of pointers, each points to widget used in
 *  the game_window. note that the chess pieces themselves are not represented
 *   as widgets - this is to improve performance and reduce load on the Renderer.
 *   on failure - NULL
 */
Widget** create_gw_widgets(SDL_Renderer* r);
/**
 * Creates an SDL_Texture from a BMP file, and performs thresholding,
 * by calling clean_surface_by_threshold on the intermedite surface.
 * @param r - Pointer to an SDL_Renderer
 * @param image - name of BMP file to read from.
 * @param bg_r bg_g bg_b - RGB values for the background
 * @return - on success - pointer to an SDL_Texture, O,W - NULL.
 */
SDL_Texture* create_tx_from_bmp(SDL_Renderer* r,const char* image,Uint8 bg_r,Uint8 bg_g,Uint8 bg_b);
/**
 *  performs thresholding on and SDL_Surface
 * @param s - Pointer to an SDL_Surface
 * @param bg_r bg_g bg_b - RGB values to assign to pixels in the intermediate range.
 * @return - on failure, NULL, on success - pointer to an SDL_Surface for which:
 * for every pixel with GRAY_LEVEL over 220 - replaced with complete white,
 * for every pixel with GRAY_LEVEL below 65 - replaced with complete black,
 * pixels in the intermediate range are assigned the RGB values received as parameter,
 */
void clean_surface_by_threshold(SDL_Surface* s,Uint8 bg_r,Uint8 bg_g,Uint8 bg_b);
/**
 * Updates the screen in accordance with the game_window internal state
 * makes buttons active/inactive as necessary, updates the grid and updates
 * the state of the board by calling update_game_state
 * @param gw - Pointer to a game_window.
 * @return - on success, GUI_SUCCESS, else - an appropriate GUI_MESSAGE
 * informative of the failure.
 */
GUI_MESSAGE update_gw_status(game_window* gw);
/**
 * calls update_gw_status, if not successful -
 * fires a USER EVENT with code FAILURE.
 * @param gw - Pointer to a game_window.
 */
void update_gw_with_event_firing(game_window* gw);
/**
 * Shows the user a dialog (using a messagebox).
 * @param w - the Window to use.
 * @param choice - Pointer to an int, where the use'rs choice will be stored.
 * @return - on success, 1. O.W - a negative number.
 */
int gw_show_save_dialog(int* choice,SDL_Window* w);
/**
 * checks if the point (x,y) is  in the board && belongs to curr player.
 * @param gw - pointer to the game_window
 * @param x,y  - the coordinates of the point.
 * @return - true if the point (x,y) is  in the board && belongs to curr player
 * else - false.
 */
bool is_point_valid_self(game_window* gw,Sint32 x,Sint32 y);
/**
 * checks if the point (x,y) is in the board && belongs to the opponent or is an empty entry.
 * @param gw - pointer to the game_window
 * @param x,y  - the coordinates of the point.
 * @return - true if the point (x,y) is in the board && belongs to the opponent or is an empty entry.
 * else - false.
 */
bool is_point_valid_opponent(game_window* gw,Sint32 x,Sint32 y);
//this function is called when the drag ended.
/**
 * finalizes the drag and drop - interprets the begin and end locations,
 * and performs the move if they are legitimate.
 * @param gw - Pointer to a game_window.
 * @param start_x,start_y - coordinates where the drag began.
 * @param end_x,end_y - coordinates where the drag ended.
 * @return - void
 */
void finalize_drag_drop(game_window* gw,Sint32 start_x,Sint32 start_y,Sint32 end_x,Sint32 end_y);
/**
 * Updates the mouse location with a picture of the tool being dragged, centerd around the cursor.
 * @param gw - Pointer to a game_window.
 * @param x,y - position of the mouse.
 * @return - void
 */
void update_drag_drop_grid(game_window* gw,Sint32 x,Sint32 y);
//callback functions:
/**
 * callback function for the restart button.
 * @param gw - Pointer to a game_window.
 * @return - void
 * @action - restarts the game, updates the GUI.
 */
void gw_cb_restart(void* gw);
/**
 * callback function for the save button.
 * @param gw - Pointer to a game_window.
 * @return - void
 * @action -if gw != NULL, calls gw_save_current.
 */
void gw_cb_save(void* gw);
/**
 * callback function for the load button.
 * @param gw - Pointer to a game_window.
 * @return - void
 * @action - fires the LOAD_REQUESTED event.
 */
void gw_cb_load(void* gw);
/**
 * callback function for the undo button.
 * @param gw - Pointer to a game_window.
 * @return - void
 * @action - the button is active iff an undo is possible, so this
 * function by contract does not check validity of the undo operation,
 * only calls the undo_move function in GameBoard
 */
void gw_cb_undo(void* gw);
/**
 * callback function for the back to main button.
 * @param gw - Pointer to a game_window.
 * @return - void
 * @action - if a save is needed, prompts the user and acts accordinly,
 * if no save is needed - fire the BACK_TO_MAIN event.
 */
void gw_cb_back_to_main(void* gw);
//updates in case of CHECK/MATE/TIE.
/**
 * updates in case of CHECK/MATE/TIE.
 * @param gw - Pointer to a game_window.
 * @return - void
 * @action - shoes the banners, if one is relevant (check,mate,tie)
 */
void update_game_state(game_window* gw);
/**
 * callback function for the exit button.
 * @param gw - Pointer to a game_window.
 * @return - void
 * @action - if a save is needed, prompts the user and acts accordinly,
 * if no save is needed - fire the SDL_QUIT event.
 */
void gw_cb_exit(void* gw);
/**
 * maps a tool to a code.
 * @param tool - a game_tool.
 * @return - an int in {1...12} for legitimate tools. O/W - returns 16 (arbitrarily).
 * the mapping is arbitrary and used mainly to address the SDL_Textures we keep.
 */
int get_piece_code(game_tools tool);
/**
 * saves the current game to FS.
 * @param board - Pointer to a game_board.
 * @return - void
 * @action -saves the game, in aacordance with the specification given
 * the game is saved in ./svg/gui_saved_xml_1.xml
 * if file number MAX_FILES exists, it is deleted. all other files
 * are given an offset of 1 to their name.
 */
GUI_MESSAGE gw_save_current(game_board* board);
/**
 * handles the highlight moves feature (BONUS)
 * @param gw - Pointer to a game_window.
 * @param e - Pointer to an SDL_Event.
 * @return - void
 * @action - if the event is a right click on the current user's tool
 * and the game configuration matches the specification given, highlights
 * all the moves possible, with the following color configuration:
 * if a slot is catchable, it's filled with a grid, the color is given by CATCHABLE_RGB_{R,G,B}
 * if a slot houses an opponents piece, it's filled with a grid, the color is given by OPPONENT_RGB_{R,G,B}
 * if a slot corresponds to a castling move, it's filled with a grid, the color is given by CASTLE_RGB_{R,G,B}
 * else, it's filled with a grid, the color is given by NEUTRAL_RGB_{R,G,B}
 */
void highlight_moves(game_window* gw,SDL_Event* e);
/**
 * draws a grid in a square
 * @param gw - Pointer to a game_window.
 * @param r - Pointer to an SDL_Renderer.
 *  @param R,G,B - RGB values for the lines color (respectively).
 * @return -GUI_SUCCESS if successful -namely, was successful in drawing a covering the
 * slot (row,column), with each grid cell 5x5 pixels, with line colors given by the RGB mapping of
 * R,G,B respectively.
 * else - GUI_FAILURE.
 */
GUI_MESSAGE draw_lines_on_square(SDL_Renderer* r,int row,int column,Sint32 R,Sint32 G,Sint32 B);
/**
 * DEPRECATED - INCLUDED IN THE SOURCE FOR FUTURE USE, IF DESIRED.
 * shows a message box in case the game is over/tie.
 * @param board_stat - status of the board.
 * @param computer_move - weather the last move performed by the computer.
 * @return void
 */
void notify_finish(CHESS_STATUS board_stat,bool computer_move);
/**
 * shows a PAWN PROMOTION dialog, in which the user can choose the tool
 * he wants to change to.
 * @param w - Pointer to an SDL_Window
 * @param color - color of current player.
 * @return if successfuly received a choice from the user,
 * the chosen tool is returned. O/W - a pawn with the color of the current player.
 */
game_tools get_pawn_promotion(SDL_Window* w,player_color color);
/**
 * does the first move in case the computer is the white.
 * @param gw - Pointer to a game_window
 * @return GUI_SUCCESS , else - an appropriate GUI_MESSAGE informative
 * of the failure encountered
 */
GUI_MESSAGE init_move(game_window* gw);
//notify the user with a message box in case this is a pawn promotion.
/**
 * notify the user with a message box in case the computer
 * performed a pawn promotion
 * @param gw - Pointer to a game_window
 * @param move - the command associated with the move.
 * @return void
 */
void notify_promotion(game_window* gw,MOVE_COMMAND move);
#endif /* GAME_WINDOW_H_ */
