/*
 * Button.c
 *
 *  Created on: 25 Aug 2017
 *      Author: GalL
 */
#include "Button.h"
#include <stdlib.h>
#include <stdio.h>
Widget* create_button(SDL_Renderer* renderer,SDL_Rect loc,const char* image_active,const char* image_inactive,void (*handle)(void*)){
	if(!renderer||!image_active || !image_inactive){return NULL;}
	Widget* res = (Widget*)(malloc(sizeof(Widget)));
	Button* button = (Button*)malloc(sizeof(Button));
	//textures for the active&inactive images.
	SDL_Surface* surf1 = SDL_LoadBMP(image_active);
	SDL_Texture* tx_active = SDL_CreateTextureFromSurface(renderer,surf1);
	SDL_Surface* surf2 = SDL_LoadBMP(image_inactive);
	SDL_Texture* tx_inactive = SDL_CreateTextureFromSurface(renderer,surf2);
	//make sure allocation and/or creation have succeeded:
	if(!res || !button || !surf1 || !tx_active || !surf2 || !tx_inactive){
		free(res);
		free(button);
		SDL_FreeSurface(surf1);
		SDL_DestroyTexture(tx_active);
		SDL_FreeSurface(surf2);
		SDL_DestroyTexture(tx_inactive);
		//announce a failure
		perror(SDL_GetError());
		return NULL;
	}
	//gettind rid of the surface which is now redundant.
	SDL_FreeSurface(surf1);
	SDL_FreeSurface(surf2);
	//configuring the button itself
	button->loc = loc;
	button->renderer = renderer;
	button->texture_active = tx_active;
	button->texture_inactive = tx_inactive;
	button->handle = handle;
	button->is_active = true;
	button->set_active = set_active;
	//wrapping it all up as a widget
	res->data = button;
	res->destroy = destroy_button;
	res->draw = draw_button;
	res->handle_event = handle_button_event;
	return res;
}
void destroy_button(Widget* src){
	if(!src){return;}
	//free the texture
	SDL_DestroyTexture(((Button*)src->data)->texture_active);
	SDL_DestroyTexture(((Button*)src->data)->texture_inactive);
	free(src->data);
	free(src);
}
void handle_button_event(Widget* src,SDL_Event* e,void* extra_data){
	//in case of NULL widget/event OR if the button is not active - return.
	if(!src||!e || !((Button*)src->data)->is_active){return;}
	//check if we are interested in this event - namely - this is
	// a mouse-lift event in this buttons location - we focus only on mouse up
	// to avoid double windows (if we react to up and down), and because users tend
	//to prefer a reaction AFTER they release the button and not before.
	if(e->type == SDL_MOUSEBUTTONUP ){
		SDL_Point p;
		p.x = e->button.x;
		p.y = e->button.y;
		if(SDL_PointInRect(&p,&((Button*)src->data)->loc) &&((Button*)src->data)->is_active){
			//invoke the specific function associated with this widget's button, pass the extra data.
			if(((Button*)src->data)->handle != NULL){
				((Button*)src->data)->handle(extra_data);
			}
		}
	}
}
GUI_MESSAGE draw_button(Widget* src){
	if(!src){
		return GUI_INVALID_ARGUMENT;
	}
	Button* b = (Button*)(src->data);
	//decide which texture we want
	SDL_Texture* tx = (b->is_active)? b->texture_active : b->texture_inactive;
	if(SDL_RenderCopy(b->renderer,tx,NULL,&b->loc)<0){
		fprintf(stderr,"ERROR: WHILE CREATING BUTTON %s\n",SDL_GetError());
		return GUI_SDL_FAILURE;
	}
	return GUI_SUCCESS;
}
GUI_MESSAGE set_active(Widget* src,bool is_active){
	if(!src){return GUI_INVALID_ARGUMENT;}
	((Button*)src->data)->is_active = is_active;
	return draw_button(src);
}
