/*
 * Widget.h
 *
 *  Created on: 25 Aug 2017
 *      Author: GalL
 */

#ifndef WIDGET_H_
#define WIDGET_H_

#include "GUI_common.h"
//represents a widget
typedef struct widget_t Widget;
struct widget_t {
	// a data we may want to associate with the widget.
	void* data;
	GUI_MESSAGE (*draw)(Widget*);
	void (*handle_event)(Widget* ,SDL_Event*,void* extra_data); // handles event
	void (*destroy)(Widget* );//destroys the widget
} ;

/**
  * Destroys a Widget - NULL SAFE.
  * @param w  - Pointer to a Widget to destroy.
  * @return void
  * @action - frees resources used by the Widget, by means of calling it's destroy function.
  */
void destroy_widget(Widget* w);

/**
  * Destroys an "array" of widgets - NULL SAFE.
  * @param w  - Pointer to the first Widget
  * @param num - the number of widgets to destroy.
  * @return void
  * @action - frees resources used by w[i], for every i in {0...num-1}
  */
void destroy_widgets_list(Widget** w,int num);
#endif /* WIDGET_H_ */
