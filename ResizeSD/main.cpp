/*
 * ResizeSD - main.cpp
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
#include "../common/common.h"
#include <string>

#define DELAY   30

const char* picPath = "/usr/local/rastari/resizeSDGUI.png";

static SDL_Surface *image = NULL;
static SDL_Surface *screen = NULL;

int main(int argc, char* args[])
{
    if(SDL_Init(SDL_INIT_EVERYTHING) == -1) return 0x01;

    SDL_ShowCursor(SDL_DISABLE);

    screen = SDL_SetVideoMode(SCREENWIDTH, SCREENHEIGHT, SCREENBPP, SDL_SWSURFACE);
    if(screen == NULL) return 0x02;

    image = load_image(picPath,0xff);
    if(image == NULL) return 0x03;

    SDL_FillRect( screen, &screen->clip_rect, SDL_MapRGB(screen->format, 0xFF, 0xFF, 0xFF));
    apply_surface(0,0,image,screen);

    if(SDL_Flip(screen) == -1) return 0x04;
    systemf("resizeSD.sh");
    systemf("sleep 3");

    SDL_FreeSurface(image);
    SDL_Quit();

    return 0x00;
}


