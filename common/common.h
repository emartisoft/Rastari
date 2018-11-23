/*
 * Rastari - common.h
 *
 * Copyright (C) 2018 emartisoft. All rights reserved.
 * Author: emarti, Murat Ozdemir <dtemarti@gmail.com>
 *
 * This file is distributed under the GNU General Public License, version 2
 * or at your option any later version. See <http://www.gnu.org/licenses/>.
 *
 */

#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

#include <stdio.h>
#include <string>
#include <SDL.h>
#include <SDL/SDL_image.h>

#define SCREENWIDTH     800
#define SCREENHEIGHT    600
#define SCREENBPP      32

#define SHAREPATH(filename)   "/usr/local/rastari/" filename

extern SDL_Cursor *_initArrowCursor();
extern int systemf(const char *fmt, ...);
extern void apply_surface(int x, int y, SDL_Surface* source, SDL_Surface* destination, Uint8 alpha = SDL_ALPHA_OPAQUE, SDL_Rect* clip = NULL);
extern SDL_Surface *load_image(std::string filename, Uint8 R=0, Uint8 G=0, Uint8 B=0);

#endif // COMMON_H_INCLUDED
