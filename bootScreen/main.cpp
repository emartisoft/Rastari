/*
 * BootScreen - main.cpp
 *
 * Copyright (C) 2018 emartisoft. All rights reserved.
 * Author: emarti, Murat Ozdemir <dtemarti@gmail.com>
 *
 * This file is distributed under the GNU General Public License, version 2
 * or at your option any later version. See <http://www.gnu.org/licenses/>.
 *
 */

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <string>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int SCREEN_BPP = 32;

#define DELAY   30

const char* picPath = "/usr/local/rastari/start.png";
const char* sndPath = "/usr/local/rastari/rastari.wav";

static SDL_Surface *image = NULL;
static SDL_Surface *screen = NULL;
static Mix_Chunk *speech = NULL;

SDL_Surface *load_image(std::string filename)
{
    SDL_Surface* loadedImage = NULL;
    SDL_Surface* optimizedImage = NULL;
    loadedImage = IMG_Load(filename.c_str());
    if(loadedImage != NULL)
    {
        optimizedImage = SDL_DisplayFormat(loadedImage);
        SDL_FreeSurface(loadedImage);
    }
    return optimizedImage;
}

int main(int argc, char* args[])
{
    if(SDL_Init(SDL_INIT_EVERYTHING) == -1) return 0x01;

    SDL_ShowCursor(SDL_DISABLE);

    screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE);
    if(screen == NULL) return 0x02;

    //Initialize SDL_mixer
    if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1 )
    {
        return 0x04;
    }

    speech = Mix_LoadWAV(sndPath);

    if( speech == NULL )
    {
        return 0x05;
    }

    //SDL_WM_SetCaption("Boot Screen for Rastari", NULL);

    image = load_image(picPath);
    if(image == NULL) return 0x03;

    SDL_Rect offset;
    offset.x = (screen->w - image->w)/2;
    offset.y = (screen->h - image->h)/2;

    int countdown = 142;
    int alpha = 0;
    int step = 5;

    while(countdown>0)
    {
        SDL_FillRect( screen, &screen->clip_rect, SDL_MapRGB(screen->format, 0xFF, 0xFF, 0xFF));

        if (alpha>350)
	{ 
		step = -step;
		Mix_PlayChannel(-1, speech, 0);
	}
        alpha += step;

        SDL_SetAlpha(image, SDL_SRCALPHA, (alpha>255)?255:alpha);
        SDL_BlitSurface(image, NULL, screen, &offset);

        if(SDL_Flip(screen) == -1) return 0x04;

        SDL_Delay(DELAY);
        countdown--;
    }

    SDL_FreeSurface(image);
    Mix_FreeChunk(speech);
    Mix_CloseAudio();
    SDL_Quit();

    return 0x00;
}


