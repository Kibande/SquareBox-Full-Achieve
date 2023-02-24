#include "Game.h"
#include "IntroLayer.h"
#include<Asset-Manager/Asset-Manager.h>
#ifdef __EMSCRIPTEN__
	#include <emscripten.h>
#endif

void loopCallback(void* arg)
{
  static_cast<Game*>(arg)->loopStep();
}

Game::Game() {
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		cout << "Error SDL_Init" << SDL_GetError() << endl;
	}
	if (SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer) < 0) {
		cout << "Error Window y Renderer" << SDL_GetError() << endl;
	}

	auto pixel_buffer = SquareBox::AssetManager::IOManager::getPixelDataFromImageFile("res/icon.png");
	Uint32 rmask = 0x000000ff;
	Uint32 gmask = 0x0000ff00;
	Uint32 bmask = 0x00ff0000;
	Uint32 amask = 0xff000000;
	SDL_Surface* iconSurface = SDL_CreateRGBSurfaceFrom(pixel_buffer.pixels, pixel_buffer.width, pixel_buffer.height, 32, pixel_buffer.width * 4, rmask, gmask, bmask, amask);

	SDL_SetWindowIcon(window, iconSurface);
	SDL_SetWindowTitle(window, "Meanwhile In Russia");
	SquareBox::AssetManager::IOManager::freePixelData(pixel_buffer);

	// Escalado de im�genes de calidad 
	// https://wiki.libsdl.org/SDL_HINT_RENDER_SCALE_QUALITY
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

	// Audio
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
	Mix_VolumeMusic(50);

	// fuentes
	TTF_Init();


	auto pixel_buffer_2 = SquareBox::AssetManager::IOManager::getPixelDataFromImageFile("res/crosshair.png");
	SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(pixel_buffer_2.pixels, pixel_buffer_2.width, pixel_buffer_2.height, 32, pixel_buffer_2.width * 4, rmask, gmask, bmask, amask);

	SDL_Cursor* cursor = SDL_CreateColorCursor(surface, 0, 0);
	SDL_SetCursor(cursor);
	SquareBox::AssetManager::IOManager::freePixelData(pixel_buffer_2);

	changeLayer(IntroLayer::instance(this));

	loopActive = true; // bucle activo
	loop();
}

void Game::loop() {
	#ifdef __EMSCRIPTEN__
		emscripten_set_main_loop_arg(&loopCallback, this, 0, 1);
	#else
		while (loopActive) {
			loopStep();
		}
		Mix_CloseAudio();
	#endif
}

void Game::loopStep() {
	int initTick = SDL_GetTicks();

	// Controles
	layer->processControls();
	// Actualizar elementos
	layer->update();
	// Dibujar
	layer->draw();
	
	int endTick = SDL_GetTicks();
	int differenceTick = endTick - initTick;
	
	if (differenceTick < (1000 / frameRate)) {
		SDL_Delay((1000 / frameRate) - differenceTick);
	}
}

void Game::changeLayer(Layer* layer) {
	layer->init();
	this->layer = layer;
}

SDL_Texture* Game::getTexture(string filename) {
	if (mapTextures.find(filename) == mapTextures.end()) {
		auto pixel_buffer = SquareBox::AssetManager::IOManager::getPixelDataFromImageFile(filename.c_str());
		Uint32 rmask = 0x000000ff;
		Uint32 gmask = 0x0000ff00;
		Uint32 bmask = 0x00ff0000;
		Uint32 amask = 0xff000000;
		SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(pixel_buffer.pixels, pixel_buffer.width, pixel_buffer.height, 32, pixel_buffer.width * 4, rmask, gmask, bmask, amask);

		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
		SquareBox::AssetManager::IOManager::freePixelData(pixel_buffer);
		mapTextures[filename] = texture;
	}

	return mapTextures[filename];
}

TTF_Font* Game::getFont(string fontname, int size, int outlineSize) {
	string fontnameAndSize = fontname + to_string(size) + "_" + to_string(outlineSize);
	if (mapFonsts.find(fontnameAndSize) == mapFonsts.end()) {
		TTF_Font* font = TTF_OpenFont(fontname.c_str(), size);
		if (outlineSize > 0) TTF_SetFontOutline(font, outlineSize);
		mapFonsts[fontnameAndSize] = font;
	}

	return mapFonsts[fontnameAndSize];
}

WAVAudio* Game::getWAV(string filename, bool loop) {
	string filenameWithLoop = filename + to_string(loop);
	if (mapWAVs.find(filenameWithLoop) == mapWAVs.end()) {
		mapWAVs[filenameWithLoop] = new WAVAudio(filename, loop);
	}

	return mapWAVs[filenameWithLoop];
}
