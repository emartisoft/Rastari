/*
 * SelectNetwork - main.cpp
 *
 * Copyright (C) 2018 emartisoft. All rights reserved.
 * Author: emarti, Murat Ozdemir <dtemarti@gmail.com>
 *
 * This file is distributed under the GNU General Public License, version 2
 * or at your option any later version. See <http://www.gnu.org/licenses/>.
 *
 */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <string>
#include <SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include "../common/sdlbutton.h"
#include "../common/ini.h"
#include "../common/common.h"

#define ID (__LINE__)
#define CHRWIDTH    10
#define CHRHEIGHT   24
#define TEXTLENGTH  64

#define TEMPFILE            "/tmp/listOfESSID.txt"
#define NETWORKS            "/tmp/networks.ini"
#define FINDWIRELESS        "ifconfig -a | grep wl > /tmp/wlinterface.txt"
#define TEMPFINDWIRELESS    "/tmp/wlinterface.txt"

#ifdef __arm__
#define WPASUPPCONF "/etc/wpa_supplicant/wpa_supplicant.conf"
#else
// for test on PC (Ubuntu)
#define WPASUPPCONF "/tmp/wpa_supplicant.conf"
#endif

static SDL_Cursor  *cursor;

static SDL_Surface *screen = NULL;
static SDL_Surface *font = NULL;
static SDL_Surface *desk = NULL;
static SDL_Surface *rebSheet = NULL;
static SDL_Surface *sabSheet = NULL;
static SDL_Surface *cabSheet = NULL;
static SDL_Surface *lebSheet = NULL;
static SDL_Surface *ribSheet = NULL;
static SDL_Surface *scbSheet = NULL;
static SDL_Surface *okSheet = NULL;

bool saved = false;
int messageCountDown = 0;

// The music that will be played
static Mix_Music *music = NULL;
//The sound effects that will be used
static Mix_Chunk *click = NULL;

static SDL_Surface *whiteRect=NULL;
bool bConfiguration=false;

static char text[TEXTLENGTH];

struct WidgetState
{
  int mousex;
  int mousey;
  int mousedown;

  int hotitem;
  int activeitem;

  int kbditem;
  int keyentered;
  int keymod;
  int keychar;

  int lastwidget;
} widgetstate = {0,0,0,0,0,0,0,0,0};

struct Configuration
{
    char name[32]; // network name

};

Configuration config;
int currentMenu, menuCount;

char *trimwhitespace(char *str)
{
  char *end;
  while(isspace((unsigned char)*str))
    str++;
  if(*str == 0)
    return str;
  end = str + strlen(str) - 1;
  while(end > str && isspace((unsigned char)*end))
    end--;
  end[1] = '\0';

  return str;
}

int createListOfNetworks()
{
    // find wireless interface
    if(systemf(FINDWIRELESS))
    {
        return 0x09;
    }

    FILE *wl = fopen(TEMPFINDWIRELESS,"r");

    if (wl == NULL)
    {
        return 0x0a;
    }
    ssize_t read;
    char *interfaceline = NULL;
    size_t wlen = 0;
    read = getline(&interfaceline, &wlen, wl);

    char *interface;
    interface = strtok(interfaceline, " ");

    //printf("interface: %s\n", interface);

    char cmdScanWifi[1024];

    sprintf(cmdScanWifi, "iwlist %s scan | grep ESSID: > /tmp/listOfESSID.txt", interface);

    //printf("%s\n", cmdScanWifi);

    if(systemf(cmdScanWifi))
    {
        return 0x01;
    }

    FILE *fp;
    FILE *networkList;
    char *line = NULL;
    size_t len = 0;

    int ssidCount=0;

    fp = fopen(TEMPFILE, "r");
    if (fp == NULL)
        return 0x02;

    networkList = fopen(NETWORKS,"w");
    if(networkList==NULL)
        return 0x03;

    fflush(networkList);

    while ((read = getline(&line, &len, fp)) != -1) {
        char *p;
        p=strtok(line,"\"");
        while(p!=NULL)
        {
            p=trimwhitespace(p);
            if((strcmp(p, "ESSID:")!=0) && (strcmp(p, "")!=0))
            {
                fprintf(networkList, "[Network%d]\nName = %s\n", ssidCount, p);
                ssidCount++;
            }
            p=strtok(NULL,"\"");
        }
    }

    //fprintf(networkList, "[NetworkCount]\nCount = %d\n", ssidCount);
    menuCount=ssidCount;

    fclose(fp);
    fclose(networkList);
    if (line)
        free(line);

    return 0;
}

static int handler(void* user, const char* section, const char* name,
                   const char* value)
{
    Configuration* pconfig = (Configuration*)user;

    char strSection[12];
    char strMenuCount[5];
    strcpy(strSection, "Network");
    sprintf(strMenuCount, "%d", currentMenu);
    strcat(strSection, strMenuCount);

    #define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    if (MATCH(strSection, "Name")) {
        strcpy(pconfig->name, strdup(value));
    }
   /* else if (MATCH("NetworkCount", "Count")) {
        menuCount=atoi(value);
    }*/
    else
    {
        return 0;  /* unknown section/name, error */
    }

    return 1;
}


bool loadMenu()
{
    if (currentMenu<0) currentMenu = menuCount - 1;
    currentMenu = currentMenu % menuCount;

    if (ini_parse(NETWORKS, handler, &config) < 0)
    {
        printf("Can't load 'networks.ini'\n");
        return false;
    }
    return true;
}

bool saveConfig(char* network, char* password)
{
    /*
        network={
        ssid="ssid"
        psk="password"
        }
    */
    FILE *nc = fopen(WPASUPPCONF, "w");
    if (nc == NULL) return false;
    fflush(nc);
    fprintf(nc, "network={\nssid=\"%s\"\npsk=\"%s\"\n}", network, password);
    fclose(nc);

    return true;

}

void drawchar(char ch, int x, int y)
{
  SDL_Rect src, dst;
  src.w = CHRWIDTH;
  src.h = CHRHEIGHT;
  src.x = 0;
  src.y = (ch - 32) * CHRHEIGHT;
  dst.w = CHRWIDTH;
  dst.h = CHRHEIGHT;
  dst.x = x;
  dst.y = y;
  SDL_BlitSurface(font, &src, screen, &dst);
}

void drawstring(const char * str, int x, int y)
{
  while (*str)
  {
    drawchar(*str,x,y);
    x += CHRWIDTH;
    str++;
  }
}

void drawrect(int x, int y, int w, int h, int color)
{
  SDL_Rect r;
  r.x = x;
  r.y = y;
  r.w = w;
  r.h = h;
  SDL_FillRect(screen, &r, color);
}

int regionhit(int x, int y, int w, int h)
{
  if (widgetstate.mousex < x ||
    widgetstate.mousey < y ||
    widgetstate.mousex >= x + w ||
    widgetstate.mousey >= y + h)
    return 0;
  return 1;
}

int textEdit(int id, int x, int y, char *buffer)
{
  int len = strlen(buffer);
  int changed = 0;

  if (regionhit(x-4, y-4, TEXTLENGTH*CHRWIDTH+8, CHRHEIGHT+8))
  {
    widgetstate.hotitem = id;
    if (widgetstate.activeitem == 0 && widgetstate.mousedown)
      widgetstate.activeitem = id;
  }

  if (widgetstate.kbditem == 0)
    widgetstate.kbditem = id;

  if (widgetstate.kbditem == id)
    drawrect(x-6, y-6, TEXTLENGTH*CHRWIDTH+12, CHRHEIGHT+12, 0x000000);

  if (widgetstate.activeitem == id || widgetstate.hotitem == id)
  {
    drawrect(x-4, y-4, TEXTLENGTH*CHRWIDTH+8, CHRHEIGHT+8, 0xeeeeee);
  }
  else
  {
    drawrect(x-4, y-4, TEXTLENGTH*CHRWIDTH+8, CHRHEIGHT+8, 0xffffff);
  }

  drawstring(buffer,x,y);

  if (widgetstate.kbditem == id && (SDL_GetTicks() >> 8) & 1)
    drawstring("|",x + len * CHRWIDTH, y);

  if (widgetstate.kbditem == id)
  {
    switch (widgetstate.keyentered)
    {
        case SDLK_TAB:
          widgetstate.kbditem = 0;
          if (widgetstate.keymod & KMOD_SHIFT)
            widgetstate.kbditem = widgetstate.lastwidget;
          widgetstate.keyentered = 0;
          break;
        case SDLK_BACKSPACE:
          if (len > 0)
          {
            len--;
            buffer[len] = 0;
            changed = 1;
          }
          break;
    }

    if (widgetstate.keychar >= 32 && widgetstate.keychar < 127 && len < TEXTLENGTH)
    {
      buffer[len] = widgetstate.keychar;
      len++;
      buffer[len] = 0;
      changed = 1;
    }
  }

  if (widgetstate.mousedown == 0 &&
    widgetstate.hotitem == id &&
    widgetstate.activeitem == id)
    widgetstate.kbditem = id;

  widgetstate.lastwidget = id;

  return changed;
}

int main(int argc, char *argv[])
{
    if (argc > 1) bConfiguration = true; // Rastari Configuration mode

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
        return 0x01;
    }

    atexit(SDL_Quit);
    screen = SDL_SetVideoMode(SCREENWIDTH, SCREENHEIGHT, 32, SDL_SWSURFACE);

    // Enable keyboard repeat
    SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
    // Enable keyboard UNICODE processing for the text field.
    SDL_EnableUNICODE(1);

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
    cursor = _initArrowCursor();
    if (cursor == NULL) {
        return false;
    }
    /* Set the arrow cursor */
    SDL_SetCursor(cursor);

    font = load_image(SHAREPATH("result.png"), 0xff, 0xff, 0xff);
    desk = load_image(SHAREPATH("desk.png"), 0xff, 0x00, 0xff);

    // Button Sheet
    rebSheet = load_image(SHAREPATH("reboot.png"), 0xff, 0x00, 0xff);
    sabSheet = load_image(SHAREPATH("save.png"),   0xff, 0x00, 0xff);
    cabSheet = load_image(SHAREPATH("cancel.png"), 0xff, 0x00, 0xff);
    ribSheet = load_image(SHAREPATH("rightarrow.png"), 0xff, 0x00, 0xff);
    lebSheet = load_image(SHAREPATH("leftarrow.png"),  0xff, 0x00, 0xff);
    scbSheet = load_image(SHAREPATH("scan.png"),       0xff, 0x00, 0xff);
    okSheet  = load_image(SHAREPATH("bOK.png"), 0xff, 0x00, 0xff);

    if ((rebSheet==NULL)||(sabSheet==NULL)||(cabSheet==NULL)||(ribSheet==NULL)||(lebSheet==NULL)||(scbSheet==NULL)||(font==NULL)||(desk==NULL)||(okSheet==NULL)) return 0x03;


    // Buttons
    Button rebootButton(77,400,98,26);
    Button saveButton(632,400,98,26);
    Button cancelButton(511,400,98,26);
    Button leftButton(659,295,30,22);
    Button rightButton(700,295,30,22);
    Button scanButton(618,295,30,22);

    if(createListOfNetworks()!=0)
    {
        printf("Networks.ini file not created.\n");
        return 1;
    }

    currentMenu=0;
    loadMenu();

    whiteRect = SDL_CreateRGBSurface(0, 530, 25, 32, 0, 0, 0, 0);
    SDL_FillRect(whiteRect, NULL, SDL_MapRGB(whiteRect->format, 255, 255, 255));


    bool quit = false;
    while (!quit)
    {
        apply_surface(0,0,desk,screen);
        // render textedit
        widgetstate.hotitem = 0;
        textEdit(ID,83,357,text);

        if (widgetstate.mousedown == 0)
        {
            widgetstate.activeitem = 0;
        }
        else
        {
        if (widgetstate.activeitem == 0)
            widgetstate.activeitem = -1;
        }
        if (widgetstate.keyentered == SDLK_TAB)
            widgetstate.kbditem = 0;
        widgetstate.keyentered = 0;
        widgetstate.keychar = 0;
        // end of the render textedit

        if(!bConfiguration)
        {
            rebootButton.show(rebSheet, screen);

            if (saved)
            {
                cancelButton.show(okSheet, screen);
            }
            else
            {
                cancelButton.show(cabSheet, screen);
            }
        }
        else
        {
            // hide text "After save configu......" for rastari configuration
            apply_surface(75, 430, whiteRect, screen);
        }
        saveButton.show(sabSheet, screen);
        rightButton.show(ribSheet, screen);
        leftButton.show(lebSheet, screen);
        scanButton.show(scbSheet, screen);

        drawstring(config.name, 85, 295); // print network

        messageCountDown--;
        if (messageCountDown<1)
        {
            messageCountDown = 0;
        }
        else if (messageCountDown>0)
        {
            drawstring("Configuration is saved", (SCREENWIDTH-22*10)/2, 480);
        }

        SDL_UpdateRect(screen, 0, 0, SCREENWIDTH, SCREENHEIGHT);
        SDL_Delay(20);

        // Poll for events, and handle the ones we care about.
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {

            // button events
            if(!bConfiguration)
            {
                rebootButton.handle_events(event);
                cancelButton.handle_events(event);
            }


            saveButton.handle_events(event);
            leftButton.handle_events(event);
            rightButton.handle_events(event);
            scanButton.handle_events(event);

            switch (event.type)
            {

                case SDL_MOUSEMOTION:
                    // update mouse position
                    widgetstate.mousex = event.motion.x;
                    widgetstate.mousey = event.motion.y;
                    break;

                case SDL_MOUSEBUTTONDOWN:
                    // update button down state if left-clicking
                    if (event.button.button == 1)
                      widgetstate.mousedown = 1;
                    break;

                case SDL_MOUSEBUTTONUP:
                    // update button down state if left-clicking
                    if (event.button.button == 1)
                      widgetstate.mousedown = 0;

                    if(Mix_PlayChannel( -1, click, 0 ) != -1)
                    {

                        if (!bConfiguration)
                        {
                            if (rebootButton.clicked(event))
                            {
                                systemf("reboot");
                            }

                            if (cancelButton.clicked(event))
                            {
                                quit=true;
                            }
                        }

                        if (saveButton.clicked(event))
                        {
                            saved=true;
                            if (saveConfig(config.name, text))
                            {
                               messageCountDown = 100;
                            }
                            if (bConfiguration) quit=true;

                        }

                        if (scanButton.clicked(event))
                        {
                            if(createListOfNetworks()!=0)
                            {
                                drawstring("No Networks", 85, 295);
                            }
                            else
                            {
                                loadMenu();
                            }

                        }

                        if (rightButton.clicked(event))
                        {
                            currentMenu++;
                            loadMenu();
                        }

                        if (leftButton.clicked(event))
                        {
                            currentMenu--;
                            loadMenu();
                        }
                    }

                    break;



                case SDL_KEYDOWN:
                    if(Mix_PlayChannel( -1, click, 0 ) != -1)
                    {
                        // If a key is pressed, report it to the widgets
                        widgetstate.keyentered = event.key.keysym.sym;
                        widgetstate.keymod = event.key.keysym.mod;
                        // if key is ASCII, accept it as character input
                        if ((event.key.keysym.unicode & 0xFF80) == 0)
                          widgetstate.keychar = event.key.keysym.unicode & 0x7f;
                     }

                    break;

                case SDL_KEYUP:
                    switch (event.key.keysym.sym)
                    {
                        case SDLK_ESCAPE:
                            quit = true;
                            break;
/*
                        case SDLK_RETURN:
                            printf("%s\n", text);
                            break;
*/
                        default:
                            break;
                    }
                    break;

                case SDL_QUIT:
                    quit= true;
                    break;

                default:
                    break;
            }
        }

    }


    // Quit
    SDL_FreeCursor(cursor);
    SDL_FreeSurface(rebSheet);
    SDL_FreeSurface(sabSheet);
    SDL_FreeSurface(cabSheet);
    SDL_FreeSurface(ribSheet);
    SDL_FreeSurface(lebSheet);
    SDL_FreeSurface(scbSheet);
    SDL_FreeSurface(okSheet);
    SDL_FreeSurface(font);
    SDL_FreeSurface(desk);
    SDL_FreeSurface(screen);
    SDL_FreeSurface(whiteRect);
    //Free the music
    Mix_FreeMusic( music );
    //Free the sound effects
    Mix_FreeChunk( click );

    //Quit SDL_mixer
    Mix_CloseAudio();

    SDL_Quit();

    return 0;
}

