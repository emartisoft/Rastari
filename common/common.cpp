#include "common.h"

/* XPM */
static const char *_mouseArrowData[] = {
  /* pixels */
  "X                               ",
  "XX                              ",
  "X.X                             ",
  "X..X                            ",
  "X...X                           ",
  "X....X                          ",
  "X.....X                         ",
  "X......X                        ",
  "X.......X                       ",
  "X........X                      ",
  "X.....XXXXX                     ",
  "X..X..X                         ",
  "X.X X..X                        ",
  "XX  X..X                        ",
  "X    X..X                       ",
  "     X..X                       ",
  "      X..X                      ",
  "      X..X                      ",
  "       XX                       ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                ",
  "                                "
};

/* Helper that creates a new mouse cursor from an XPM */
extern SDL_Cursor *_initArrowCursor()
{
  SDL_Cursor *cursor;
  int i, row, col;
  Uint8 data[4*32];
  Uint8 mask[4*32];

  i = -1;
  for ( row=0; row<32; ++row ) {
    for ( col=0; col<32; ++col ) {
      if ( col % 8 ) {
        data[i] <<= 1;
        mask[i] <<= 1;
      } else {
        ++i;
        data[i] = mask[i] = 0;
      }
      switch (_mouseArrowData[row][col]) {
        case 'X':
          data[i] |= 0x01;
          mask[i] |= 0x01;
          break;
        case '.':
          mask[i] |= 0x01;
          break;
        case ' ':
          break;
      }
    }
  }


  cursor = SDL_CreateCursor(data, mask, 32, 32, 0, 0);
  return cursor;
}

extern int systemf(const char *fmt, ...)
{
	char __sys[1024];
	int ret_code;
	va_list args;
	va_start(args, fmt);
        vsnprintf(__sys, 1023, fmt, args);
        ret_code = system(__sys);
	va_end(args);

	return ret_code;
}

// Load image
extern SDL_Surface *load_image(std::string filename, Uint8 R, Uint8 G, Uint8 B)
{
    SDL_Surface* loadedImage = NULL;
    SDL_Surface* optimizedImage = NULL;

    loadedImage = IMG_Load(filename.c_str());
    if(loadedImage != NULL)
    {
        optimizedImage = SDL_DisplayFormat(loadedImage);
        SDL_FreeSurface(loadedImage);
        if( optimizedImage != NULL )
        {
            //Color key surface
            SDL_SetColorKey( optimizedImage, SDL_SRCCOLORKEY, SDL_MapRGB( optimizedImage->format, R, G, B ) );
        }
    }
    return optimizedImage;
}

// Apply Surface
extern void apply_surface(int x, int y, SDL_Surface* source, SDL_Surface* destination, Uint8 alpha, SDL_Rect* clip)
{
    //Holds offsets
    SDL_Rect offset;

    //Get offsets
    offset.x = x;
    offset.y = y;

    //Blit
    SDL_SetAlpha( source, SDL_SRCALPHA, alpha );
    SDL_BlitSurface( source, clip, destination, &offset );
}

