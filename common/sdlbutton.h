/*
 * Rastari - sdlbutton.h
 *
 * Copyright (C) 2018 emartisoft. All rights reserved.
 * Author: emarti, Murat Ozdemir <dtemarti@gmail.com>
 *
 * This file is distributed under the GNU General Public License, version 2
 * or at your option any later version. See <http://www.gnu.org/licenses/>.
 *
 */

#ifndef SDLBUTTON_H_INCLUDED
#define SDLBUTTON_H_INCLUDED

#include <SDL/SDL.h>

class Button
{
    private:
        bool XYInRect( const SDL_Rect& rect, int x, int y );
        void set_clips(int w, int h);
        SDL_Rect* clip;
    public:

        SDL_Rect box;
        Button( int x, int y, int w, int h );
        void handle_events(SDL_Event &event);
        void show(SDL_Surface* buttonSheet, SDL_Surface* screen, Uint8 alpha = SDL_ALPHA_OPAQUE);
        bool clicked(SDL_Event &event);
};

#endif // SDLBUTTON_H_INCLUDED
