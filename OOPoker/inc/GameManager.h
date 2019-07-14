#pragma once
/*
OOPoker

Copyright (c) 2010 Lode Vandevenne
All rights reserved.

This file is part of OOPoker.

OOPoker is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OOPoker is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with OOPoker.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
Linux compile command:
g++ *.cpp -W -Wall -Wextra -ansi -O3
g++ *.cpp -W -Wall -Wextra -ansi -g3
*/


/*
OOPoker, or "Object Oriented Poker", is a C++ No-Limit Texas Hold'm engine meant
to be used to implement poker AIs for entertainment  or research purposes. These
AIs can be made to battle each other, or a single human can play against the AIs
for his/her enjoyment.
*/

//In all functions below, when cards are sorted, it's always from high to low

#include <vector>
#include <string>
#include <algorithm>
#include <iostream>

#include "../inc/ai.h"
#include "../inc/ai_blindlimp.h"
#include "../inc/ai_call.h"
#include "../inc/ai_checkfold.h"
#include "../inc/ai_human.h"
#include "../inc/ai_raise.h"
#include "../inc/ai_random.h"
#include "../inc/ai_smart.h"
#include "../inc/ai_Blossom.h"
#include "../inc/card.h"
#include "../inc/combination.h"
#include "../inc/game.h"
#include "../inc/host_terminal.h"
#include "../inc/info.h"
#include "../inc/io_terminal.h"
#include "../inc/observer.h"
#include "../inc/observer_terminal.h"
#include "../inc/observer_terminal_quiet.h"
#include "../inc/observer_log.h"
#include "../inc/pokermath.h"
#include "../inc/random.h"
#include "../inc/table.h"
#include "../inc/tools_terminal.h"
#include "../inc/unittest.h"
#include "../inc/util.h"

class GameManager
{
    public:
        GameManager();
        void DoGame();

    private:
};
