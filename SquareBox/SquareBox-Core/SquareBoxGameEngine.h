#pragma once
#ifndef SQB_PLATFORM_ANDROID
#define SDL_MAIN_HANDLED
/*
since SDL_main.h is included automatically by SDL.h ,,
not having this causes the LNK2019 SDL Error on windows
but having it will prevent our android app from finding its entry point , out main

*/
#endif

#include "config.h"
namespace SquareBox {
	extern int init();
}