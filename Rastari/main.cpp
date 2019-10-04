/*
 * Rastari - main.cpp
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
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_mixer.h>
#include <string>
#include <stdio.h>
#include <dirent.h>
#include <linux/limits.h>
#include "../common/sdltimer.h"
#include "../common/sdlbutton.h"
#include "../common/ini.h"
#include "../common/common.h"

//#define VERSION "1.0.0"
#define LENGTH  34
#define MENUSTARTY  80

#define RASTARINI   "/tmp/rastari.ini"

//const char* caption = "Rastari";
#ifdef __arm__
const char* hatariConfigPath = "/root/.hatari/";
#else
const char* hatariConfigPath = "/home/emarti/.hatari/"; // While compiles on PC
#endif

static SDL_Surface *screen = NULL;
static SDL_Cursor  *cursor;

static SDL_Surface *rastariLogo = NULL;
static SDL_Surface *rastariText = NULL;
static SDL_Surface *nemutlu = NULL;
static SDL_Surface *nextButtonSheet = NULL;
static SDL_Surface *prevButtonSheet = NULL;
static SDL_Surface *autostartButtonSheet = NULL;
static SDL_Surface *runButtonSheet = NULL;
static SDL_Surface *okButtonSheet = NULL;
static SDL_Surface *topMenuSheet = NULL;
static SDL_Surface *separate1MenuSheet = NULL;
static SDL_Surface *separate2MenuSheet = NULL;
static SDL_Surface *separate3MenuSheet = NULL;
static SDL_Surface *rastariMenuButtonSheet = NULL;
static SDL_Surface *filemanagerMenuSheet = NULL;
static SDL_Surface *settingsMenuSheet = NULL;
static SDL_Surface *aboutMenuSheet = NULL;
static SDL_Surface *audioMenuSheet = NULL;
static SDL_Surface *updateMenuSheet = NULL;
static SDL_Surface *poweroffMenuSheet = NULL;
static SDL_Surface *mouseCable = NULL;
static SDL_Surface *rebootMenuSheet = NULL;
static SDL_Surface *selectwifiMenuSheet = NULL;
static SDL_Surface *autostartMenuSheet = NULL;
static SDL_Surface *aboutSheet = NULL;
static SDL_Surface *configInfoPanel = NULL;
static SDL_Surface *background = NULL;
static SDL_Surface *menuCaption = NULL;
static SDL_Surface *sModel = NULL;
static SDL_Surface *sCpu = NULL;
static SDL_Surface *sSpeed = NULL;
static SDL_Surface *sRam = NULL;
static SDL_Surface *sOs = NULL;
static SDL_Surface *cModel = NULL;
static SDL_Surface *cCpu = NULL;
static SDL_Surface *cSpeed = NULL;
static SDL_Surface *cRam = NULL;
static SDL_Surface *cOs = NULL;
static SDL_Surface *autostartConfigPanel = NULL;
static SDL_Surface *autostartTitle = NULL;
static SDL_Surface *autostartDisableText = NULL;
static SDL_Surface *autostartEnableText = NULL;
static SDL_Surface *autostartConfigText = NULL;
static SDL_Surface *returnButtonSheet = NULL;
static SDL_Surface *checkButtonSheet = NULL;
static SDL_Surface *uncheckButtonSheet = NULL;
static SDL_Surface *muteonSheet = NULL;
static SDL_Surface *muteoffSheet = NULL;

static TTF_Font *infoS = NULL;
static TTF_Font *infoC = NULL;
static TTF_Font *menuFont = NULL;

const int FRAMES_PER_SECOND = 25;

static TTF_Font *font = NULL;
SDL_Color textColor = { 0x00, 0x00, 0x00 };
SDL_Color atariBlue = { 0x33, 0x53, 0xb1 };
SDL_Color red = { 0xff, 0x00, 0x00 };

SDL_Event event;
int GetXCenteredSurface(SDL_Surface* surface);
bool init();
void clean_up();
bool load_files();
void nextConfig();
void prevConfig();
bool autostartConfig();
bool prepareScript(int cmd);
int substring(char *source, int from, int n, char *target);
void GetAutostartConfigName();

static Mix_Music *music = NULL;
bool mute;
int mutecountdown = 0;
static Mix_Chunk *click = NULL;
static Mix_Chunk *speech = NULL;

int menuCount;
int currentMenu;

int countdown = 0;
int titleIndex;
int titleCount = 0;
bool titleDirection;
char temp[LENGTH]={};

bool menuShowing=false;
bool aboutShowing=false;

const int flagWidth = 300;
const int flagHeight= 245;
static SDL_Surface *flagFull = NULL;
static SDL_Rect clipsFlag[25];
int frame;
void set_clipsFlag();
int flagY;

const int verWidth[11] = { 8,9,8,8,8,8,9,8,8,8,4};
static SDL_Surface *verFull = NULL;
static SDL_Rect verClip[11];
void set_verclips();

bool autostartShowing = false;
char autostartConfigName[256];
bool statusAutostart;
bool GetStatusProperty(const char *prop);

char nmtd[24]="Ne Mutlu T\x00fcrk\x00fcm Diyene!";
int nmtdCountDown=0;

struct Configuration
{
    char title[256];
    char model[LENGTH];
    char cpu[LENGTH];
    char speed[LENGTH];
    char ram[LENGTH];
    char os[LENGTH];
};

Configuration config;

// tos version
/*
usage:
    char *returned_str = tosVersion("TOS v1.04 (1989)(Atari Corp)(Mega ST)(UK).img");
    printf("TOS Version is %s\n",returned_str);
*/
char* tosVersion(char* tosImgFile)
{
    struct ver
    {
        int8_t major,minor;
    };

    char *str = (char *) malloc(sizeof(char) * 6); // 1.04E : ver1.04 EmuTOS, 2.06T : ver2.06 TOS
    FILE *tosFile;
    struct ver tosVer;
    size_t readed;
    char etosf[6];

    str[0] = '?';
    str[1] = '.';
    str[2] = '?';
    str[3] = '?';
    str[4] = ' ';
    str[5] = '\0';

    tosFile=fopen(tosImgFile,"rb");
    if (tosFile)
    {
        fseek(tosFile,0x02,SEEK_SET);
        readed=fread(&tosVer,sizeof(struct ver),1,tosFile);
        if((readed != 1) || (tosVer.major > 9) || (tosVer.minor > 9))
        {
            // special version 4.92
            if ((tosVer.major == 0x27) && (tosVer.minor == 0x00))
            {
                str[0] = '4';
                str[2] = '9';
                str[3] = '2';
            }
        }
        else
        {
            str[0]= 0x30 + tosVer.major;
            str[2] = '0';
            str[3] = 0x30 + tosVer.minor;
        }

        const char cETO[4]="ETO";
        fseek(tosFile,0x2C,SEEK_SET);
        readed=fread(&etosf,3,1,tosFile);
        if(readed==1)
        {
            if(strcmp(etosf, cETO)==0)
                str[4] = 'E'; // EmuTOS
            else
                str[4] = 'T'; // TOS
        }

        //  printf("%s - %s\n",str,etosf);
        //  printf("TOS Version is %d.",tosVersion.major);
        //  printf("%02d\n",tosVersion.minor);
        fclose(tosFile);
    }

    return str;
}

static int handler(void* user, const char* section, const char* name,
                   const char* value)
{
    Configuration* pconfig2 = (Configuration*)user;

    char strSection[9];
    char strMenuCount[5];
    strcpy(strSection, "Menu");
    sprintf(strMenuCount, "%d", currentMenu);
    strcat(strSection, strMenuCount);

    #define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    if (MATCH(strSection, "Title")) {
        strcpy(pconfig2->title, strdup(value));
    }
    else if (MATCH(strSection, "Model")) {
        strcpy(pconfig2->model, strdup(value));
    }
    else if (MATCH(strSection, "Cpu")) {
        strcpy(pconfig2->cpu, strdup(value));
    }
    else if (MATCH(strSection, "Speed")) {
        strcpy(pconfig2->speed, strdup(value));
    }
    else if (MATCH(strSection, "Ram")) {
        strcpy(pconfig2->ram, strdup(value));
    }
    else if (MATCH(strSection, "Os")) {
        strcpy(pconfig2->os, strdup(value));
    }
    else
    {
        return 0;  /* unknown section/name, error */
    }

    return 1;
}

static int hatariHandler(void* user, const char* section, const char* name,
                   const char* value)
{
    Configuration* pconfig = (Configuration*)user;

    #define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    if (MATCH("System", "nMachineType")) {
        switch(atoi(value))
        {
            case 0:
                strcpy(pconfig->model, "\x00e9 ATARI ST");
                break;
            case 1:
                strcpy(pconfig->model, "\x00e9 ATARI STE");
                break;
            case 2:
                strcpy(pconfig->model, "\x00e9 ATARI TT");
                break;
            case 3:
                strcpy(pconfig->model, "\x00e9 ATARI Falcon");
                break;
            default:
                strcpy(pconfig->model, "Unknown");
        }
    }
    else if (MATCH("System", "nCpuLevel")) {
        switch(atoi(value))
        {
            case 0:
                strcpy(pconfig->cpu, "Motorola MC68000");
                break;
            case 1:
                strcpy(pconfig->cpu, "Motorola MC68010");
                break;
            case 2:
                strcpy(pconfig->cpu, "Motorola MC68020");
                break;
            case 3:
                strcpy(pconfig->cpu, "Motorola MC68030 + FPU");
                break;
            case 4:
                strcpy(pconfig->cpu, "Motorola MC68040");
                break;
            default:
                strcpy(pconfig->cpu, "Unknown");
        }
    }
    else if (MATCH("System", "nCpuFreq")) {
        switch(atoi(value))
        {
            case 8:
                strcpy(pconfig->speed, "8 MHz");
                break;
            case 16:
                strcpy(pconfig->speed, "16 MHz");
                break;
            case 32:
                strcpy(pconfig->speed, "32 MHz");
                break;
            default:
                strcpy(pconfig->speed, "Unknown");
        }
    }
    else if (MATCH("Memory", "nMemorySize")) {
        switch(atoi(value))
        {
            case 0:
                strcpy(pconfig->ram, "512 Kbytes");
                break;
            case 1:
                strcpy(pconfig->ram, "1 Mbyte");
                break;
            case 2:
                strcpy(pconfig->ram, "2 Mbytes");
                break;
            case 4:
                strcpy(pconfig->ram, "4 Mbytes");
                break;
            case 8:
                strcpy(pconfig->ram, "8 Mbytes");
                break;
            case 14:
                strcpy(pconfig->ram, "14 Mbytes");
                break;
            default:
                strcpy(pconfig->ram, "Unknown");
        }
    }
    else if (MATCH("ROM", "szTosImageFileName")) {
        char os[34];
        char tver[6];
        char ver[5];
        strcpy(tver, tosVersion(strdup(value)));
        substring(tver,0,4,ver);
        if(tver[4]=='T')
        {
            // TOS 1.04 + GEM
            strcpy(os, "TOS ");
            strcat(os, ver);
            strcat(os, " + GEM");
        }
	else if(tver[4]=='E')
        {
            // emuTOS (Compatible With TOS 1.04)
            strcpy(os, "EmuTOS (Compatible With TOS ");
            strcat(os, ver);
            strcat(os, ")");
        }
	else
	{
	    // Unknown
	    strcpy(os, "Unknown");
	}

        strcpy(pconfig->os, os);
    }
    else
    {
        return 0;  /* unknown section/name, error */
    }
    return 1;
}

// prepare script file
bool prepareScript(int cmd)
{
    FILE *scriptCmd;
    char commandStr[PATH_MAX];

    scriptCmd = fopen(SHAREPATH("cmd.sh"), "w");
    if (scriptCmd==NULL)
    {
        return false;
    }

    fflush(scriptCmd);
    fputs("#!/bin/bash\n# This script file created by Rastari\n", scriptCmd);

    switch (cmd)
    {
        case 0:
            strcpy(commandStr, "hatari -c \"");
            strcat(commandStr, hatariConfigPath);
            strcat(commandStr, config.title);
            strcat(commandStr, ".cfg\"");
            break;
        case 1:
            strcpy(commandStr, "mc -ac /media/usb /root/rastari");
            break;
        case 2:
            strcpy(commandStr, "shutdown -h now");
            break;
        case 3:
            strcpy(commandStr, "setfont /usr/local/rastari/font/default8x16.psf.gz\n");
            strcat(commandStr, "raspi-config\n");
	    strcat(commandStr, "setfont /usr/local/rastari/font/atarist-normal.psf.gz");
            break;
        case 4:
            strcpy(commandStr, "reboot");
            break;
        case 5:
            strcpy(commandStr, "rm -f /etc/udev/rules.d/70-persistent-net.rules\n");
	    strcat(commandStr, "./SelectNetwork");
            break;
        case 6:
            strcpy(commandStr, "hatari");
            break;
        case 7:
            strcpy(commandStr, "./update.sh");
            break;
        case 8:
            strcpy(commandStr, "./AudioOutput");
            break;
    }


    fputs(commandStr, scriptCmd);

    fclose(scriptCmd);
    return true;
}

// Create ini file
bool createIniFile()
{
    FILE *ini;
    menuCount = 0;
    char strMenuCount[5];
    char title[256];

    ini = fopen(RASTARINI, "w");
    if (ini==NULL)
    {
        return false;
    }

    fflush(ini);

    /*

    [Menu1]
    Title = Atari 1040 ST

    hatari 2.X
    ---------------------------------------------------------------------
    System/nModelType = n [ST, Mega ST, STE, Mega STE, TT, Falcon]
    System/nCpuLevel = n [68000, 10, 20 ,30, 40, 50 ,60]
    System/nCpuFreq = n [8, 16, 32]
    System/n_FPUType = n [0, 68881, 68882, 68040] 0=none, 68040=internal
    ROM/szTosImageFileName = char*, dosya yolu
    Memory/nMemorySize = n  bytes


    hatari 1.8
    ---------------------------------------------------------------------
    System/nMachineType = n [ST, STE, TT, Falcon]
    System/nCpuLevel = n [68000, 10, 20 ,30+FPU, 40]
    System/nCpuFreq = n [8, 16, 32]
    ROM/szTosImageFileName = char*, dosya yolu
    Memory/nMemorySize = n  [512KB, 1, 2, 4, 8, 14] 512KB=0

    */

    // list of files
    struct dirent** dirlist;
    int dircnt = scandir(hatariConfigPath, &dirlist, NULL, alphasort);
    bool error = (dircnt < 0);
    if (error)
    {
        printf("Can't read directory %s ...\n", hatariConfigPath);
    }
    else
    {
        for (int i = 0; i < dircnt; ++i)
        {
            struct dirent* dir = dirlist[i];
            const char* extension = strstr(dir->d_name, ".cfg");
            // check for file extension and make sure that is not only part of the filename
            if (extension != NULL && strlen(extension) == 4)
            {
                // [MenuX]
                fputs("[Menu", ini);
                sprintf(strMenuCount, "%d", menuCount);
                fputs(strMenuCount, ini);
                // Title = ...
                fputs("]\nTitle = ", ini);
                int title_len = extension - dir->d_name;
                strncpy(title, dir->d_name, title_len);
                title[title_len] = 0;
                fputs(title, ini);

                //read values from hatari cfg file
                char cfgFileName[PATH_MAX];

                strcpy(cfgFileName, hatariConfigPath);
                strcat(cfgFileName, title);
                strcat(cfgFileName,".cfg");

                Configuration cfg;

                if (ini_parse(cfgFileName, hatariHandler, &cfg) < 0) {
                    // don't leave function here as we still have to free memory
                    // and close the file
                    printf("Can't load cfg file %s ...", cfgFileName);
                    error = true;
                }
                else
                {
                    fputs("\nModel = ", ini);
                    fputs(cfg.model, ini);
                    fputs("\nCpu = ", ini);
                    fputs(cfg.cpu, ini);
                    fputs("\nSpeed = ", ini);
                    fputs(cfg.speed, ini);
                    fputs("\nRam = ", ini);
                    fputs(cfg.ram, ini);
                    fputs("\nOs = ", ini);
                    fputs(cfg.os, ini);
                }

                fputs("\n", ini);
                menuCount++;
            }
            free(dir);
        }
        free(dirlist);
    }

    fclose(ini);

    return !error;
}

bool loadMenu()
{
    if (currentMenu<0) currentMenu = menuCount - 1;
    currentMenu = currentMenu % menuCount;

    if (ini_parse(RASTARINI, handler, &config) < 0) {
        printf("Can't load 'rastari.ini'\n");
        return false;
    }

    if(strlen(config.title)>LENGTH)
    {
        substring(config.title,0,LENGTH,temp);
        titleDirection=false;
    }
    else
    {
        strcpy(temp, config.title);
        titleDirection=true;
    }

    menuCaption = TTF_RenderText_Blended( menuFont, temp, textColor );

    sModel = TTF_RenderText_Blended(infoS, config.model, textColor);
    sCpu   = TTF_RenderText_Blended(infoS, config.cpu, textColor);
    sSpeed = TTF_RenderText_Blended(infoS, config.speed, textColor);
    sRam   = TTF_RenderText_Blended(infoS, config.ram, textColor);
    sOs    = TTF_RenderText_Blended(infoS, config.os, textColor);

    countdown=LENGTH;
    titleIndex=0;
    return true;
}

void nextConfig()
{
    currentMenu++;
    //Play the scratch effect
    if( Mix_PlayChannel( -1, click, 0 ) != -1 )
    {
        loadMenu();
    }
}

void prevConfig()
{
    currentMenu--;
    //Play the scratch effect
    if( Mix_PlayChannel( -1, click, 0 ) != -1 )
    {
        loadMenu();
    }
}

bool autostartConfig()
{
    //Play the scratch effect
    if( Mix_PlayChannel( -1, click, 0 ) != -1 )
    {
        char cmdAutoStart[1024];
        sprintf(cmdAutoStart, "echo '%s' > autostart.cfg", config.title);

        if(systemf(cmdAutoStart))
        {
            return false;
        }
    }

    return true;
}

int substring(char *source, int from, int n, char *target)
{
    int length,i;
    //get string length
    for(length=0;source[length]!='\0';length++);

    if(from>length){
        printf("Starting index is invalid.\n");
        return 1;
    }

    if((from+n)>length){
        //get substring till end
        n=(length-from);
    }

    //get substring in target
    for(i=0;i<n;i++){
        target[i]=source[from+i];
    }
    target[i]='\0'; //assign null at last

    return 0;
}

int mutlak(int val)
{
    if(val<0)
        return -val;
    return val;
}

void LoadAndPlay(const char *musicfilename)
{
    if (music != NULL) Mix_FreeMusic(music);
    music = Mix_LoadMUS(musicfilename);
    if( music != NULL )
    {
        if( Mix_PlayingMusic() == 0 )
        {
            Mix_PlayMusic( music, -1 );
        }
    }
}

int main (int argc, char* argv[])
{
    //printf("\n---------------------------------------");
    //printf("\n  Rastari, V%s\n", VERSION);
    //printf("  Coded by emarti, Murat Ozdemir\n  Email: dtemarti@gmail.com");
    //printf("\n---------------------------------------\n");

    if(!createIniFile())
    {
        printf("Can't create ini file\n");
        return 1;
    }

    if(menuCount==0)
    {
	    prepareScript(6);
        printf("Hatari Configuration File(s) not found.\nPlease, create cfg file using Hatari.\nPress any key to run Hatari.");
        getchar();
        return 2; // CFG Files not found; run hatari only not Rastari. Please, create a config file from hatari.
    }

    bool quit = false;
    int bgX = 0, bgY = 0, bgV = -1;
    Timer fps;

    if(!init())
    {
        return 1;
    }

    if(!load_files())
    {
        return 1;
    }

    currentMenu = 0;

    if(!loadMenu())
    {
        return 1;
    }

    Button runButton(135, 290, 530, 67);
    Button nextButton(672, 290 + 16, 32, 32);
    Button prevButton(96, 290 + 16, 32, 32);
    Button autostartButton(626, 371, 32, 32);

    Button rastariMenuButton        (-5, 16, 66, 66);

    Button filemanagerMenuButton    (16, MENUSTARTY + topMenuSheet->h, 124, 24);
    //-------------------------------
    Button settingsMenuButton       (16, MENUSTARTY + topMenuSheet->h + separate1MenuSheet->h + aboutMenuSheet->h * 1/2, 124, 24);
    Button selectwifiMenuButton     (16, MENUSTARTY + topMenuSheet->h + separate1MenuSheet->h + aboutMenuSheet->h * 2/2, 124, 24);
    Button audioMenuButton          (16, MENUSTARTY + topMenuSheet->h + separate1MenuSheet->h + aboutMenuSheet->h * 3/2, 124, 24);
    //-------------------------------
    Button rebootMenuButton         (16, MENUSTARTY + topMenuSheet->h + separate1MenuSheet->h * 2 + aboutMenuSheet->h * 4/2, 124, 24);
    Button poweroffMenuButton       (16, MENUSTARTY + topMenuSheet->h + separate1MenuSheet->h * 2 + aboutMenuSheet->h * 5/2, 124, 24);
    //-------------------------------
    Button autostartMenuButton      (16, MENUSTARTY + topMenuSheet->h + separate1MenuSheet->h * 3 + aboutMenuSheet->h * 6/2, 124, 24);
    Button updateMenuButton         (16, MENUSTARTY + topMenuSheet->h + separate1MenuSheet->h * 3 + aboutMenuSheet->h * 7/2, 124, 24);
    Button aboutMenuButton          (16, MENUSTARTY + topMenuSheet->h + separate1MenuSheet->h * 3 + aboutMenuSheet->h * 8/2, 124, 24);


    /*

    MENU
    ====

    File Manager
    ------------
    Rpi Config
    Select Wi-Fi
    Audio Output
    ------------
    Reboot
    Power Off
    ------------
    Autostart
    Update
    About

    */

    Button okButton(screen->w - (60+24), screen->h - (34 + 24),60,34);
    Button returnButton(GetXCenteredSurface(autostartConfigPanel) + autostartConfigPanel->w - 60 - 32, 290 + autostartConfigPanel->h - 34 - 32, 60, 34);
    Button enableButton(170, 400, 35, 37);
    Button disableButton(170, 360, 35, 37);

    rastariText = TTF_RenderText_Blended( font, "RASTARI", atariBlue );

    cModel = TTF_RenderText_Blended(infoC, "Machine Type", atariBlue);
    cCpu   = TTF_RenderText_Blended(infoC, "CPU Type", atariBlue);
    cSpeed = TTF_RenderText_Blended(infoC, "CPU Frequency", atariBlue);
    cRam   = TTF_RenderText_Blended(infoC, "RAM", atariBlue);
    cOs    = TTF_RenderText_Blended(infoC, "Operating System", atariBlue);

    if((rastariText == NULL) || (cModel==NULL) || (cCpu==NULL) ||
            (cSpeed==NULL) || (cRam==NULL) || (cOs==NULL) || (menuCaption==NULL) ||
            (sModel==NULL) || (sCpu==NULL) || (sSpeed==NULL) || (sRam==NULL) || (sOs==NULL))
    {
        return 1;
    }

    LoadAndPlay(SHAREPATH("bensound-cute.wav"));

    set_clipsFlag();
    frame=0;

    set_verclips();

    GetAutostartConfigName();
    statusAutostart=GetStatusProperty(".autostart");
    mute=GetStatusProperty(".mute");

    while(!quit)
    {
        fps.start();
        while(SDL_PollEvent(&event))
        {
            runButton.handle_events(event);
            nextButton.handle_events(event);
            prevButton.handle_events(event);
            autostartButton.handle_events(event);

	        rastariMenuButton.handle_events(event);
            okButton.handle_events(event);
            enableButton.handle_events(event);
            disableButton.handle_events(event);
            returnButton.handle_events(event);

    	    filemanagerMenuButton.handle_events(event);
    	    aboutMenuButton.handle_events(event);
    	    settingsMenuButton.handle_events(event);
    	    updateMenuButton.handle_events(event);
    	    poweroffMenuButton.handle_events(event);
    	    rebootMenuButton.handle_events(event);
    	    selectwifiMenuButton.handle_events(event);
    	    audioMenuButton.handle_events(event);
            autostartMenuButton.handle_events(event);


            switch(event.type)
            {
                case SDL_QUIT:

                    quit = true;

                break;

		        case SDL_MOUSEBUTTONDOWN:
		        // open/close menu
                    if(rastariMenuButton.clicked(event))
                    {
                        if( Mix_PlayChannel( -1, click, 0 ) != -1 )
                        {
                                menuShowing = !menuShowing;
                        }
                    }

		        break;

                case SDL_MOUSEBUTTONUP:

                    if(okButton.clicked(event))
                    {
                        if(Mix_PlayChannel( -1, click, 0 ) != -1)
                        {
                            aboutShowing = false;
                            LoadAndPlay(SHAREPATH("bensound-cute.wav"));
                        }
                    }

                    if (aboutShowing) break;

                    if(!menuShowing)
                    {
                        if(!autostartShowing)
                        {
                            // Next Config
                            if(nextButton.clicked(event))
                            {
                                nextConfig();
                            }

                            // Previous Config
                            if(prevButton.clicked(event))
                            {
                                prevConfig();
                            }

                            // Autostart config
                            if(autostartButton.clicked(event))
                            {
                                if(autostartConfig()){
                                    //printf("AutoStart is enabled for %s\n", config.title);
                                    strcpy(autostartConfigName, config.title);
                                    autostartShowing = true;
                                }
                            }

                            // run hatari
                            if(runButton.clicked(event))
                            {
                                if( Mix_PlayChannel( -1, click, 0 ) != -1 )
                                {
                                    if(prepareScript(0)) quit=true;
                                }
                            }
                        }
                        else
                        {
                            // return button for autostart screen
                            if (returnButton.clicked(event))
                            {
                                if( Mix_PlayChannel( -1, click, 0 ) != -1 )
                                {
                                    autostartShowing = false;
                                }
                            }

                            // checkboxes
                            if ((disableButton.clicked(event))||(enableButton.clicked(event)))
                            {
                                if( Mix_PlayChannel( -1, click, 0 ) != -1 )
                                {
                                    statusAutostart = !statusAutostart;
                                    if(statusAutostart)
                                    {
                                        systemf("echo autostart=true > .autostart");
                                    }
                                    else
                                    {
                                        systemf("rm -f .autostart");
                                    }
                                }
                            }

                        }
		            }
        		    else
        		    {
        			    menuShowing = false;

        			    // run mc
        			    if(filemanagerMenuButton.clicked(event))
        			    {
            				if( Mix_PlayChannel( -1, click, 0 ) != -1 )
            				{
            					if(prepareScript(1)) quit=true;
            				}
        			    }

        			    // poweroff
        			    if(poweroffMenuButton.clicked(event))
        			    {
            				if( Mix_PlayChannel( -1, click, 0 ) != -1 )
            				{
            					if(prepareScript(2)) quit=true;
            				}
        			    }

        			    // settings
        			    if(settingsMenuButton.clicked(event))
        			    {
            				if( Mix_PlayChannel( -1, click, 0 ) != -1 )
            				{
            					if(prepareScript(3)) quit=true;
            				}
        			    }

        			    // reboot
        			    if(rebootMenuButton.clicked(event))
        			    {
            				if( Mix_PlayChannel( -1, click, 0 ) != -1 )
            				{
            					if(prepareScript(4)) quit=true;
            				}
        			    }

        			    // select wi-fi
        			    if(selectwifiMenuButton.clicked(event))
        			    {
            				if( Mix_PlayChannel( -1, click, 0 ) != -1 )
            				{
            					if(prepareScript(5)) quit=true;
            				}
        			    }

        			    // Audio Output
        			    if(audioMenuButton.clicked(event))
        			    {
            				if( Mix_PlayChannel( -1, click, 0 ) != -1 )
            				{
            					if(prepareScript(8)) quit=true;
            				}
        			    }

                        // autostart
                        if(autostartMenuButton.clicked(event))
                        {
                            if( Mix_PlayChannel( -1, click, 0 ) != -1 )
                            {
                                autostartShowing = true;
                            }
                        }

        			    // update
        			    if(updateMenuButton.clicked(event))
        			    {
            				if( Mix_PlayChannel( -1, click, 0 ) != -1 )
            				{
            					if(prepareScript(7)) quit=true;
            				}
        			    }

        			    // about rastari
        			    if(aboutMenuButton.clicked(event))
        			    {
            				if( Mix_PlayChannel( -1, click, 0 ) != -1 )
            				{
            					flagY = screen->h + 8;
            					menuShowing = false;
            					aboutShowing = true;
            					LoadAndPlay(SHAREPATH("HitnSmash.wav"));
            				}
        			    }

        		     }

                break;

                case SDL_KEYDOWN:

                    if (aboutShowing || autostartShowing) break;
                    switch(event.key.keysym.sym)
                    {
                        case SDLK_LEFT:

                            prevConfig();

                        break;

                        case SDLK_RIGHT:

                            nextConfig();

                        break;

                        case SDLK_RETURN:

                            if( Mix_PlayChannel( -1, click, 0 ) != -1 )
                            {
                                    if(prepareScript(0)) quit=true;
                            }

                        break;

                        case SDLK_m:
                        	if( Mix_PlayChannel( -1, click, 0 ) != -1 )
                            {
                            	mute = !mute;
                            	mutecountdown = 100;
                            	if(mute)
                            	{
                            		systemf("echo autostart=true > .mute");
                            	}
                            	else
                            	{
                            		systemf("rm -f .mute");
                            	}
                        	}

                        break;

                        default:
                        break;

                    }

                break;

                default:
                break;

            }

        }

        if(mute)
        {
            // pause music
            Mix_PauseMusic();
        }
        else
        {
            // resume music
            Mix_ResumeMusic();
        }

        // About
        //-----------------------------------------------------------------------------
        if(aboutShowing)
        {
            apply_surface(0,0,aboutSheet,screen);

            int u = strlen(argv[1]);
            int dx = 0;
            int c;

            for (int x=0;x<u;x++)
            {
                c = (int)argv[1][x] - 0x30;
                if(c<0) c=10;
                apply_surface( 438 + dx, 80, verFull, screen, 255, &verClip[c]);
                dx += verWidth[c];
            }

            if (flagY>121)
            {
                flagY -= 2;
                nmtdCountDown=0;
            }
            else
            {
		        if (nmtdCountDown == 0) Mix_PlayChannel(-1, speech, 0);
                nmtdCountDown++;
                if (nmtdCountDown<24)
                {
                    substring(nmtd,0,nmtdCountDown, temp);
                    nemutlu = TTF_RenderText_Blended( infoS, temp, red);
                }
                apply_surface(266,170,nemutlu,screen,64);

            }
            apply_surface(369,flagY,flagFull,screen,32, &clipsFlag[frame % 25]);
            frame++;
            okButton.show(okButtonSheet, screen, 192);
        }
        else
        {

            countdown--;
            if(countdown > 0)
            {
                substring(config.model,0, LENGTH - countdown, temp);
                sModel = TTF_RenderText_Blended(infoS, temp, textColor);

                substring(config.cpu,0, LENGTH - countdown, temp);
                sCpu   = TTF_RenderText_Blended(infoS, temp, textColor);

                substring(config.speed,0, LENGTH - countdown, temp);
                sSpeed = TTF_RenderText_Blended(infoS, temp, textColor);

                substring(config.ram,0, LENGTH - countdown, temp);
                sRam   = TTF_RenderText_Blended(infoS, temp, textColor);

                substring(config.os,0, LENGTH - countdown, temp);
                sOs    = TTF_RenderText_Blended(infoS, temp, textColor);
            }

            titleCount++;
            if ((strlen(config.title)>LENGTH) && (titleCount%8 == 0) && !menuShowing)
            {
                (!titleDirection) ? titleIndex++ : titleIndex--;
                if(titleIndex>mutlak(strlen(config.title)-LENGTH))
                    titleDirection=true;
                else if (titleIndex < 1) titleDirection=false;

                substring(config.title, titleIndex, LENGTH, temp);
                menuCaption = TTF_RenderText_Blended(menuFont, temp, textColor);
            }

            bgX -= 2;
            if(bgX <= -background->w )
            {
                bgX = 0;
            }

            bgY += bgV;


            if(( bgY <= -(background->h - screen->h) ) || (bgY >= 0) )
            {
                bgV = -bgV;
            }

            // Show the background
            //-----------------------------------------------------------------------------
            apply_surface( bgX, bgY, background, screen );
            apply_surface( bgX + background->w, bgY, background, screen );

            // Show rastari logo
            //-----------------------------------------------------------------------------
            apply_surface(GetXCenteredSurface(rastariLogo),64,rastariLogo,screen);
            apply_surface(GetXCenteredSurface(rastariText),64+rastariLogo->h,rastariText,screen);

            if(mutecountdown>0)
	        {
	        	if(mute)
	        	{
	        		apply_surface(SCREENWIDTH-128, 64, muteonSheet, screen, 192);
	        	}
	        	else
	        	{
	        		apply_surface(SCREENWIDTH-128, 64, muteoffSheet, screen, 192);
	        	}
	        	mutecountdown--;
	        }


            if (!autostartShowing)
            {
     	        // Show the button
                runButton.show(runButtonSheet, screen, 128);
                nextButton.show(nextButtonSheet, screen, 128);
                prevButton.show(prevButtonSheet, screen, 128);

                // Show the configNamePanel
                apply_surface(GetXCenteredSurface(menuCaption), 290 + (runButton.box.h - menuCaption->h)/2, menuCaption, screen);

                // Menu Button
                //-----------------------------------------------------------------------------
                rastariMenuButton.show(rastariMenuButtonSheet, screen, 192);
                if(menuShowing)
                {
                    // Menu Top
                    apply_surface(16,MENUSTARTY,topMenuSheet,screen, 192);
                    apply_surface(16,MENUSTARTY + topMenuSheet->h + aboutMenuSheet->h * 1/2,separate1MenuSheet,screen,192);
                    apply_surface(16,MENUSTARTY + topMenuSheet->h + aboutMenuSheet->h * 9/4,separate2MenuSheet,screen,192);
                    apply_surface(16,MENUSTARTY + topMenuSheet->h + aboutMenuSheet->h * 14/4,separate3MenuSheet,screen,192);
                    // Menus
                    filemanagerMenuButton.show(filemanagerMenuSheet, screen, 192);
                    aboutMenuButton.show(aboutMenuSheet, screen, 192);
    		        audioMenuButton.show(audioMenuSheet, screen, 192);
                    updateMenuButton.show(updateMenuSheet, screen, 192);
                    settingsMenuButton.show(settingsMenuSheet, screen, 192);
                    poweroffMenuButton.show(poweroffMenuSheet, screen, 192);
                    rebootMenuButton.show(rebootMenuSheet, screen, 192);
                    selectwifiMenuButton.show(selectwifiMenuSheet, screen, 192);
                    autostartMenuButton.show(autostartMenuSheet, screen, 192);

                }
                else
                {
                    apply_surface(-5,82,mouseCable,screen,192);
                }

                // Show the configInfoPanel
                apply_surface(GetXCenteredSurface(configInfoPanel), 290 + runButton.box.h, configInfoPanel, screen, 128);

                apply_surface(280 - cModel->w, 24 + 290 + runButton.box.h ,cModel, screen); // Model
                apply_surface(280 - cCpu->w,   52 + 290 + runButton.box.h ,cCpu,   screen); // CPU
                apply_surface(280 - cSpeed->w, 80 + 290 + runButton.box.h ,cSpeed, screen); // Speed
                apply_surface(280 - cRam->w,  108 + 290 + runButton.box.h ,cRam,   screen); // RAM
                apply_surface(280 - cOs->w,   136 + 290 + runButton.box.h ,cOs,    screen); // OS

                apply_surface(290,20  + 290 + runButton.box.h ,sModel, screen); // Model
                apply_surface(290,48  + 290 + runButton.box.h ,sCpu,   screen); // CPU
                apply_surface(290,76  + 290 + runButton.box.h ,sSpeed, screen); // Speed
                apply_surface(290,104 + 290 + runButton.box.h ,sRam,   screen); // RAM
                apply_surface(290,132 + 290 + runButton.box.h ,sOs,    screen); // OS

                // autostart button
                autostartButton.show(autostartButtonSheet, screen, 128);

            }
            else
            {
                //autostartShowing = false;
                //printf("%s\n", autostartConfigName);
                apply_surface(GetXCenteredSurface(autostartConfigPanel), 290, autostartConfigPanel, screen, 128);

                autostartTitle = TTF_RenderText_Blended(menuFont, "Autostart", textColor);
                apply_surface(GetXCenteredSurface(autostartTitle), 290 + 32, autostartTitle, screen);

                autostartDisableText = TTF_RenderText_Blended(infoS, "Disable autostart", textColor);
                apply_surface(220, 290 + 32 + 48, autostartDisableText, screen);

                autostartEnableText = TTF_RenderText_Blended(infoS, "Enable autostart for the selected configuration", textColor);
                apply_surface(220, 290 + 32 + 48 + 14 + autostartDisableText->h, autostartEnableText, screen);

                strcpy(temp, "\"");
                strcat(temp, autostartConfigName);
                strcat(temp, "\"");
                autostartConfigText = TTF_RenderText_Blended(infoC, temp, textColor);
                apply_surface(220, 290 + 32 + 48 + 10 + autostartDisableText->h + autostartEnableText->h, autostartConfigText, screen);

                if(statusAutostart)
                {
                    disableButton.show(uncheckButtonSheet, screen);
                    enableButton.show(checkButtonSheet, screen);
                }
                else
                {
                    disableButton.show(checkButtonSheet, screen);
                    enableButton.show(uncheckButtonSheet, screen);
                }

                returnButton.show(returnButtonSheet, screen);

            }


        }

        // Update the screen
        //-----------------------------------------------------------------------------
        if( SDL_Flip( screen ) == -1 )
        {
            return 1;
        }

        // Capture the frame rate
        //-----------------------------------------------------------------------------
        if( fps.get_ticks() < 1000 / FRAMES_PER_SECOND )
        {
            SDL_Delay( ( 1000 / FRAMES_PER_SECOND ) - fps.get_ticks() );
        }
    }

    clean_up();

    return 0;
}

bool init()
{
    if( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
    {
        return false;
    }

    screen = SDL_SetVideoMode( SCREENWIDTH, SCREENHEIGHT, SCREENBPP, SDL_SWSURFACE);

    if( screen == NULL )
    {
        return false;
    }

    if( TTF_Init() == -1 )
    {
        return false;
    }

    if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1 )
    {
        return false;
    }

    // SDL_WM_SetCaption(caption, NULL );

    cursor = _initArrowCursor();
    if (cursor == NULL) {
        return false;
    }
    SDL_SetCursor(cursor);

    return true;
}

bool load_files()
{
    background = load_image(SHAREPATH("scrollbackground.png"));
    rastariLogo = load_image(SHAREPATH("rastariLogo2.png"), 0xcc, 0xcc, 0xcc);
    configInfoPanel = load_image(SHAREPATH("infopanel.png"));
    aboutSheet = load_image(SHAREPATH("about.png"), 0xff, 0x00, 0x00);
    flagFull = load_image(SHAREPATH("Bayrak.png"));
    verFull = load_image(SHAREPATH("VerXXX.png"), 0xff, 0xff, 0xff);
    autostartConfigPanel = load_image(SHAREPATH("autostartPanel.png"), 0xcc, 0xcc, 0xcc);


    nextButtonSheet = load_image(SHAREPATH("next.png"), 0xcc, 0xcc, 0xcc);
    prevButtonSheet = load_image(SHAREPATH("prev.png"), 0xcc, 0xcc, 0xcc);
    autostartButtonSheet = load_image(SHAREPATH("bAutostart.png"), 0xcc, 0xcc, 0xcc);
    runButtonSheet = load_image(SHAREPATH("toppanel_button.png"), 0x00, 0x00, 0x00);
    okButtonSheet = load_image(SHAREPATH("OK.png"), 0xff, 0xff, 0xff);
    checkButtonSheet = load_image(SHAREPATH("checkedButton.png"), 0xff, 0xff, 0xff);
    uncheckButtonSheet = load_image(SHAREPATH("uncheckedButton.png"), 0xff, 0xff, 0xff);
    returnButtonSheet = load_image(SHAREPATH("return.png"), 0xff, 0xff, 0xff);

    rastariMenuButtonSheet = load_image(SHAREPATH("menuMouseButton.png"), 0xff, 0xff, 0xff);
    topMenuSheet = load_image(SHAREPATH("topMenuSheet.png"), 0x00, 0x00, 0x00);
    separate1MenuSheet = load_image(SHAREPATH("separateMenu.png"), 0x00, 0x00, 0x00);
    separate2MenuSheet = load_image(SHAREPATH("separateMenu.png"), 0x00, 0x00, 0x00);
    separate3MenuSheet = load_image(SHAREPATH("separateMenu.png"), 0x00, 0x00, 0x00);
    filemanagerMenuSheet = load_image(SHAREPATH("mFileManager.png"), 0x00, 0x00, 0x00);
    aboutMenuSheet = load_image(SHAREPATH("mAbout.png"), 0x00, 0x00, 0x00);
    audioMenuSheet = load_image(SHAREPATH("mAudioOutput.png"), 0x00, 0x00, 0x00);
    updateMenuSheet = load_image(SHAREPATH("mUpdate.png"), 0x00, 0x00, 0x00);
    poweroffMenuSheet = load_image(SHAREPATH("mPowerOff.png"), 0x00, 0x00, 0x00);
    settingsMenuSheet = load_image(SHAREPATH("mRpiconfig.png"), 0x00, 0x00, 0x00);
    rebootMenuSheet = load_image(SHAREPATH("mReboot.png"), 0x00, 0x00, 0x00);
    selectwifiMenuSheet = load_image(SHAREPATH("mSelectwifi.png"), 0x00, 0x00, 0x00);
    autostartMenuSheet = load_image(SHAREPATH("mAutostart.png"), 0x00, 0x00, 0x00);
    muteonSheet = load_image(SHAREPATH("muteon.png"), 0xcc, 0xcc, 0xcc);
    muteoffSheet = load_image(SHAREPATH("muteoff.png"), 0xcc, 0xcc, 0xcc);


    mouseCable = load_image(SHAREPATH("mousecable.png"),  0xbf, 0xbf, 0xbf);

    font = TTF_OpenFont(SHAREPATH("AtariLogoTR.ttf"),  28 );
    menuFont = TTF_OpenFont(SHAREPATH("AtariLogoTR.ttf"),  24 );
    infoC = TTF_OpenFont(SHAREPATH("AtariLogoTR.ttf"),  14 );
    infoS = TTF_OpenFont(SHAREPATH("AtariLogoTR.ttf"),  20 );

    click = Mix_LoadWAV(SHAREPATH("click.wav"));
    speech = Mix_LoadWAV(SHAREPATH("nemutluturkumdiyene.wav"));

    if( (nextButtonSheet == NULL ) || (prevButtonSheet == NULL) || (runButtonSheet == NULL) || (topMenuSheet == NULL) || (autostartButtonSheet == NULL) ||
        (filemanagerMenuSheet == NULL ) || (aboutMenuSheet == NULL ) ||(settingsMenuSheet == NULL ) ||(updateMenuSheet == NULL ) ||
        (poweroffMenuSheet == NULL ) || (mouseCable == NULL) || (separate1MenuSheet == NULL) || (okButtonSheet == NULL) ||
        (separate2MenuSheet == NULL) || (separate3MenuSheet == NULL) || (rebootMenuSheet == NULL) || (selectwifiMenuSheet == NULL) || (audioMenuSheet == NULL) ||
        (autostartMenuSheet == NULL) || (flagFull == NULL) || (rastariLogo == NULL) || (aboutSheet == NULL) || (configInfoPanel == NULL) || (background == NULL) ||
        (click == NULL) || (speech == NULL) || ( font == NULL ) || ( infoC == NULL) || (infoS == NULL) || (menuFont == NULL) || (verFull == NULL) || (autostartConfigPanel == NULL) ||
        (checkButtonSheet == NULL) || (uncheckButtonSheet == NULL) || (returnButtonSheet == NULL) || (muteonSheet == NULL) || (muteoffSheet == NULL)
    )
    {
        printf("Failed while loading files\n");
        return false;
    }

    return true;
}


void clean_up()
{
    SDL_FreeSurface(configInfoPanel);
    SDL_FreeSurface(background);
    SDL_FreeSurface(nextButtonSheet);
    SDL_FreeSurface(prevButtonSheet);
    SDL_FreeSurface(autostartButtonSheet);
    SDL_FreeSurface(runButtonSheet);
    SDL_FreeSurface(okButtonSheet);
    SDL_FreeSurface(topMenuSheet);
    SDL_FreeSurface(rastariMenuButtonSheet);
    SDL_FreeSurface(menuCaption);
    SDL_FreeSurface(rastariLogo);
    SDL_FreeSurface(rastariText);
    SDL_FreeSurface(nemutlu);
    SDL_FreeSurface(mouseCable);
    SDL_FreeSurface(cModel);
    SDL_FreeSurface(sModel);
    SDL_FreeSurface(cCpu);
    SDL_FreeSurface(sCpu);
    SDL_FreeSurface(cRam);
    SDL_FreeSurface(sRam);
    SDL_FreeSurface(cOs);
    SDL_FreeSurface(sOs);
    SDL_FreeSurface(cSpeed);
    SDL_FreeSurface(sSpeed);
    SDL_FreeSurface(settingsMenuSheet);
    SDL_FreeSurface(filemanagerMenuSheet);
    SDL_FreeSurface(aboutMenuSheet);
    SDL_FreeSurface(audioMenuSheet);
    SDL_FreeSurface(updateMenuSheet);
    SDL_FreeSurface(poweroffMenuSheet);
    SDL_FreeSurface(separate1MenuSheet);
    SDL_FreeSurface(separate2MenuSheet);
    SDL_FreeSurface(separate3MenuSheet);
    SDL_FreeSurface(rebootMenuSheet);
    SDL_FreeSurface(selectwifiMenuSheet);
    SDL_FreeSurface(autostartMenuSheet);
    SDL_FreeSurface(aboutSheet);
    SDL_FreeSurface(flagFull);
    SDL_FreeSurface(verFull);
    SDL_FreeSurface(autostartConfigPanel);
    SDL_FreeSurface(autostartTitle);
    SDL_FreeSurface(autostartDisableText);
    SDL_FreeSurface(autostartEnableText);
    SDL_FreeSurface(autostartConfigText);
    SDL_FreeSurface(returnButtonSheet);
    SDL_FreeSurface(checkButtonSheet);
    SDL_FreeSurface(uncheckButtonSheet);
    SDL_FreeSurface(muteonSheet);
    SDL_FreeSurface(muteoffSheet);

    SDL_FreeCursor(cursor);

    TTF_CloseFont(font);
    TTF_CloseFont(infoC);
    TTF_CloseFont(infoS);
    TTF_CloseFont(menuFont);

    TTF_Quit();

    Mix_FreeMusic(music);
    Mix_FreeChunk(click);
    Mix_FreeChunk(speech);
    Mix_CloseAudio();

    SDL_Quit();
}

int GetXCenteredSurface(SDL_Surface* surface)
{
    return (screen->w - surface->w)/2;
}

void set_clipsFlag()
{
    for (int f=0;f<25;f++)
    {
        clipsFlag[f].x = f * flagWidth;
        clipsFlag[f].y = 0;
        clipsFlag[f].w = flagWidth;
        clipsFlag[f].h = flagHeight;
    }

}

void set_verclips()
{
    int stX = 0;
    for (int n=0;n<11;n++)
    {
        verClip[n].x = stX;
        verClip[n].y = 0;
        verClip[n].w = verWidth[n];
        verClip[n].h = 16;
        stX += verWidth[n];
    }
}

void GetAutostartConfigName()
{
    FILE *f;
    char str[256];
    f = fopen(SHAREPATH("autostart.cfg"), "r");
    if (f == NULL)
    {
        if(autostartConfig())
        {
            strcpy(autostartConfigName, config.title);
        }
    }
    else
    {
        while (fgets(str, 256, f) != NULL)
        {
            str[strlen(str)-1] = '\0'; // remove 0x0a (\n char)
            strcpy(autostartConfigName, str);

        }
        fclose(f);
    }


}

// if file exists, return true
bool GetStatusProperty(const char *prop)
{
    FILE *f;
    char filePath[50];
    strcpy(filePath, "/usr/local/rastari/");
    strcat(filePath, prop);
    f = fopen(filePath, "r");
    if (f==NULL)
    {
        return false;
    }
    else
    {
        fclose(f);
        return true;
    }
}
