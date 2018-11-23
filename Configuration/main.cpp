/*
 * Configuration - main.cpp
 *
 * Copyright (C) 2018 emartisoft. All rights reserved.
 * Author: emarti, Murat Ozdemir <dtemarti@gmail.com>
 *
 * This file is distributed under the GNU General Public License, version 2
 * or at your option any later version. See <http://www.gnu.org/licenses/>.
 *
 */

#include <stdio.h>
#include <string>
#include <SDL.h>
#include <SDL/SDL_image.h>
#include "../common/sdlbutton.h"
#include "../common/common.h"

static SDL_Cursor  *cursor;
static SDL_Surface *screen = NULL;
static SDL_Surface *startSheet = NULL;
static SDL_Surface *desk = NULL;

int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
        return 0x01;
    }

    atexit(SDL_Quit);
    screen = SDL_SetVideoMode(SCREENWIDTH, SCREENHEIGHT, 32, SDL_SWSURFACE);

    if (screen == NULL)
    {
        fprintf(stderr, "Unable to set up video: %s\n", SDL_GetError());
        return 0x02;
    }

    /* Create a cursor */
    cursor = _initArrowCursor();
    if (cursor == NULL) {
        return false;
    }
    /* Set the arrow cursor */
    SDL_SetCursor(cursor);

    desk = load_image(SHAREPATH("configurationGUI.png"), 0xff, 0x00, 0xff);
    // Button Sheet
    startSheet = load_image(SHAREPATH("bStart.png"), 0xff, 0x00, 0xff);

    if ((startSheet==NULL)||(desk==NULL)) return 0x03;

    // Buttons
    Button startButton(490,420,98,26);

    bool quit = false;
    while (!quit)
    {
        apply_surface(0,0,desk,screen);
        startButton.show(startSheet, screen);

        SDL_UpdateRect(screen, 0, 0, SCREENWIDTH, SCREENHEIGHT);
        SDL_Delay(20);

        // Poll for events, and handle the ones we care about.
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            // button events
            startButton.handle_events(event);

            switch (event.type)
            {
                case SDL_MOUSEBUTTONUP:
                        if (startButton.clicked(event))
                        {
                            quit=true;
                        }

                default:
                    break;
            }
        }

    }

    // Quit
    SDL_FreeCursor(cursor);
    SDL_FreeSurface(startSheet);
    SDL_FreeSurface(desk);
    SDL_FreeSurface(screen);

    SDL_Quit();

    return 0;

}

