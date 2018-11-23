/*
 * AudioOutput - main.cpp
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
#include <SDL/SDL_mixer.h>
#include "../common/sdlbutton.h"
#include "../common/common.h"

#define jack    "amixer cset numid=3 1 > /dev/null"
#define hdmi    "amixer cset numid=3 2 > /dev/null"

static SDL_Cursor  *cursor;
static SDL_Surface *screen = NULL;
static SDL_Surface *hdmiSheet = NULL;
static SDL_Surface *jackSheet = NULL;
static SDL_Surface *okSheet = NULL;
static SDL_Surface *desk = NULL;

// The music that will be played
static Mix_Music *music = NULL;
//The sound effects that will be used
static Mix_Chunk *click = NULL;

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

    //Initialize SDL_mixer
    if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1 )
    {
        return false;
    }

    //Load the music
    music = Mix_LoadMUS(SHAREPATH("bensound-acousticbreeze.wav"));

    //If there was a problem loading the music
    if( music == NULL )
    {
        return false;
    }

    //If there is no music playing
    if( Mix_PlayingMusic() == 0 )
    {
        //Play the music
        if( Mix_PlayMusic( music, -1 ) == -1 )
        {
            return 1;
        }
    }

    //Load the sound effects
    click = Mix_LoadWAV(SHAREPATH("click.wav"));

    if( click == NULL )
    {
        return false;
    }

    //Set the window caption
    //SDL_WM_SetCaption("Select a network", NULL );

    /* Create a cursor */
    cursor = _initArrowCursor();//_mouseArrowData);
    if (cursor == NULL) {
        return false;
    }
    /* Set the arrow cursor */
    SDL_SetCursor(cursor);

    desk = load_image(SHAREPATH("audioOutputGUI.png"), 0xff, 0x00, 0xff);
    // Button Sheet
    okSheet = load_image(SHAREPATH("bOK.png"), 0xff, 0x00, 0xff);
    hdmiSheet = load_image(SHAREPATH("ForceHDMI.png"),   0xff, 0x00, 0xff);
    jackSheet = load_image(SHAREPATH("ForceJack.png"), 0xff, 0x00, 0xff);

    if ((okSheet==NULL)||(hdmiSheet==NULL)||(jackSheet==NULL)||(desk==NULL)) return 0x03;

    // Buttons
    Button okButton(452,419,98,26);
    Button hdmiButton(250,330,301,26);
    Button jackButton(250,368,301,26);

    bool quit = false;
    while (!quit)
    {
        apply_surface(0,0,desk,screen);
        okButton.show(okSheet, screen);
        hdmiButton.show(hdmiSheet, screen);
        jackButton.show(jackSheet, screen);

        SDL_UpdateRect(screen, 0, 0, SCREENWIDTH, SCREENHEIGHT);
        SDL_Delay(20);

        // Poll for events, and handle the ones we care about.
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {

            // button events
            okButton.handle_events(event);
            hdmiButton.handle_events(event);
            jackButton.handle_events(event);

            switch (event.type)
            {
                case SDL_MOUSEBUTTONUP:
                    if(Mix_PlayChannel( -1, click, 0 ) != -1)
                    {

                        if (hdmiButton.clicked(event))
                        {
                            systemf(hdmi);
                        }

                        if (jackButton.clicked(event))
                        {
                            systemf(jack);
                        }

                        if (okButton.clicked(event))
                        {
                            quit=true;
                        }
                    }

                default:
                    break;
            }
        }

    }

    // Quit
    SDL_FreeCursor(cursor);
    SDL_FreeSurface(okSheet);
    SDL_FreeSurface(hdmiSheet);
    SDL_FreeSurface(jackSheet);
    SDL_FreeSurface(desk);
    SDL_FreeSurface(screen);
    //Free the music
    Mix_FreeMusic( music );
    //Free the sound effects
    Mix_FreeChunk( click );

    //Quit SDL_mixer
    Mix_CloseAudio();

    SDL_Quit();

    return 0;

}

