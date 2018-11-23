/*
 * Rastari - sdltimer.h
 *
 * Copyright (C) 2018 emartisoft. All rights reserved.
 * Author: emarti, Murat Ozdemir <dtemarti@gmail.com>
 *
 * This file is distributed under the GNU General Public License, version 2
 * or at your option any later version. See <http://www.gnu.org/licenses/>.
 *
 */

#ifndef SDLTIMER_H_INCLUDED
#define SDLTIMER_H_INCLUDED

#include <SDL/SDL.h>

class Timer
{
    private:
        int startTicks;
        int pausedTicks;
        bool paused;
        bool started;

    public:
        Timer();
        void start();
        void stop();
        void pause();
        void unpause();
        int get_ticks();
        bool is_started();
        bool is_paused();
};

#endif // SDLTIMER_H_INCLUDED
