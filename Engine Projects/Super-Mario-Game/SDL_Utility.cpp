#include "SDL_Utility.h"

#include "FileNotLoadedException.h"
#include<Asset-Manager/Asset-Manager.h>

void drawSurface(SDL_Surface* display, SDL_Surface* sprite, int x, int y)
{
	SDL_Rect destination;
	destination.x = x - (sprite->w / 2);
	destination.y = y - (sprite->h / 2);
	destination.w = sprite->w;
	destination.h = sprite->h;
	SDL_BlitSurface(sprite, nullptr, display, &destination);
}

void showFileErrorWindow(std::string errorText)
{
	errorText += "\nThe game will be closed. Please check if you have the correct file in the game directory.";
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Missing or corrupted file", errorText.c_str(), nullptr);
}

void showScreenErrorWindow()
{
	std::string errorText = "Some resources from SDL2 library couldn't be loaded."
		"Check if you have included all needed .lib and .dll files. The game will be closed.";
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", errorText.c_str(), nullptr);
}

void showSoundErrorWindow()
{
	std::string errorText = "Sound controller has not been properly initialized. "
		"You can still play the game, but no sounds will play.";
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Sound error", errorText.c_str(), nullptr);
}

SDL_Surface* loadBMP(std::string filename)
{
	const char* converted_path = filename.c_str();
	SDL_Surface* bitmap = SDL_LoadBMP(converted_path);

	if (bitmap != nullptr) {
		return bitmap;
	}
	else {
		std::string exceptionReason = std::string("The file ").append(filename)
			.append(" couldn't be loaded. Maybe this file is missing or corrupted.");

		throw FileNotLoadedException(exceptionReason);
	}
}

SDL_Surface* loadPNG(std::string filename, SDL_Surface* display)
{
	SDL_Surface* optimizedSurface = nullptr;

	Uint32 rmask = 0x000000ff;
	Uint32 gmask = 0x0000ff00;
	Uint32 bmask = 0x00ff0000;
	Uint32 amask = 0xff000000;
	auto pixelData = SquareBox::AssetManager::IOManager::getPixelDataFromImageFile(filename.c_str());
	SDL_Surface* loadedSurface = SDL_CreateRGBSurfaceFrom(pixelData.pixels, pixelData.width, pixelData.height, 32, pixelData.width * 4, rmask, gmask, bmask, amask);


	if (loadedSurface != nullptr) {
		optimizedSurface = SDL_ConvertSurface(loadedSurface, display->format, 0);
		SDL_FreeSurface(loadedSurface);
		return optimizedSurface;
	}
	else {
		std::string exceptionReason = std::string("The file ").append(filename)
			.append(" couldn't be loaded. Maybe this file is missing or corrupted.");

		throw FileNotLoadedException(exceptionReason);
	}
}