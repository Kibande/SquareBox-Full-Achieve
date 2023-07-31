#pragma once

#include <iostream>
#include <string>
using namespace std;
#include <SDL/include/SDL.h>
#include <SDL/include/SDL_ttf.h>
#include <map>
#include <SDL/include/SDL_mixer.h>
#include "WAVAudio.h"

#define WIDTH 960
#define HEIGHT 640
#define TEXT_SIZE 26
#define TEXT_BORDER 2

class Layer;

class Game
{
public:
	Game();
	SDL_Texture* getTexture(string filename);
	map<string, SDL_Texture*> mapTextures;

	TTF_Font* getFont(string fontname, int size, int outlineSize = 0);
	map<string, TTF_Font*> mapFonsts;

	WAVAudio* getWAV(string filename, bool loop = false);
	map<string, WAVAudio*> mapWAVs;

	void loop();
	void loopStep();
	void changeLayer(Layer* layer);
	SDL_Window* window;
	SDL_Renderer* renderer;
	bool loopActive; 
	Layer* layer;

	int frameRate = 30;
};
