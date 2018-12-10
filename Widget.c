/*
 * Widget.c
 *
 *  Created on: 25 Aug 2017
 *      Author: GalL
 */
#include "Widget.h"
void destroy_widget(Widget* w){
	if(!w){return;} // this makes the function NULL SAFE.
	w->destroy(w);
}
void destroy_widgets_list(Widget** w,int num){
	if(!w){return;}
	for(int i=0 ; i < num ; i++){
		if(w[i]){w[i]->destroy(w[i]);}
	}
}

