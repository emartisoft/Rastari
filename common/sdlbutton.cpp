/*
 * Rastari - sdlbutton.cpp
 *
 * Copyright (C) 2018 emartisoft. All rights reserved.
 * Author: emarti, Murat Ozdemir <dtemarti@gmail.com>
 *
 * This file is distributed under the GNU General Public License, version 2
 * or at your option any later version. See <http://www.gnu.org/licenses/>.
 *
 */

#include "sdlbutton.h"

const int CLIP_MOUSEOVER = 0;
const int CLIP_MOUSEOUT = 1;
const int CLIP_MOUSEDOWN = 2;
const int CLIP_MOUSEUP = 3;
SDL_Rect clips[ 4 ];

void Button::set_clips(int w, int h)
{
    clips[CLIP_MOUSEOVER].x = 0;
    clips[CLIP_MOUSEOVER].y = 0;
    clips[CLIP_MOUSEOVER].w = w;
    clips[CLIP_MOUSEOVER].h = h;

    clips[CLIP_MOUSEOUT].x = w;
    clips[CLIP_MOUSEOUT].y = 0;
    clips[CLIP_MOUSEOUT].w = w;
    clips[CLIP_MOUSEOUT].h = h;

    clips[CLIP_MOUSEDOWN].x = 0;
    clips[CLIP_MOUSEDOWN].y = h;
    clips[CLIP_MOUSEDOWN].w = w;
    clips[CLIP_MOUSEDOWN].h = h;

    clips[CLIP_MOUSEUP].x = w;
    clips[CLIP_MOUSEUP].y = h;
    clips[CLIP_MOUSEUP].w = w;
    clips[CLIP_MOUSEUP].h = h;
}

Button::Button( int x, int y, int w, int h )
{
    box.x = x;
    box.y = y;
    box.w = w;
    box.h = h;
    clip = &clips[ CLIP_MOUSEOUT ];
}

void Button::handle_events(SDL_Event &event)
{
    int x = 0, y = 0;
    if( event.type == SDL_MOUSEMOTION )
    {
        x = event.motion.x;
        y = event.motion.y;
        if( ( x > box.x ) && ( x < box.x + box.w ) && ( y > box.y ) && ( y < box.y + box.h ) )
        {
            clip = &clips[ CLIP_MOUSEOVER ];
        }
        else
        {
            clip = &clips[ CLIP_MOUSEOUT ];
        }
    }
    if( event.type == SDL_MOUSEBUTTONDOWN )
    {
        if( event.button.button == SDL_BUTTON_LEFT )
        {
            x = event.button.x;
            y = event.button.y;
            if( ( x > box.x ) && ( x < box.x + box.w ) && ( y > box.y ) && ( y < box.y + box.h ) )
            {
                clip = &clips[ CLIP_MOUSEDOWN ];
            }
        }
    }
    if( event.type == SDL_MOUSEBUTTONUP )
    {
        if( event.button.button == SDL_BUTTON_LEFT )
        {
            x = event.button.x;
            y = event.button.y;
            if( ( x > box.x ) && ( x < box.x + box.w ) && ( y > box.y ) && ( y < box.y + box.h ) )
            {
                clip = &clips[ CLIP_MOUSEUP ];
            }
        }
    }
}

void Button::show(SDL_Surface* buttonSheet, SDL_Surface* screen, Uint8 alpha)
{
    SDL_Rect offset;
    offset.x = box.x;
    offset.y = box.y;
    set_clips(box.w, box.h);
    SDL_SetAlpha( buttonSheet, SDL_SRCALPHA, alpha );
    SDL_BlitSurface( buttonSheet, clip, screen, &offset );
}

bool Button::XYInRect( const SDL_Rect& rect, int x, int y )
{
    return ( ( x >= rect.x && x <= rect.x + rect.w ) && ( y >= rect.y && y <= rect.y + rect.h ) );
}

bool Button::clicked(SDL_Event &event)
{
    return XYInRect(box, event.button.x, event.button.y);
}

