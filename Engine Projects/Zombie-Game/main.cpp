
//#define TESTMODE 1

#ifdef TESTMODE

#ifdef SQB_PLATFORM_ANDROID


#include <SDL.h>
#include <SDL_ttf.h>

int main(int argc, char** argsv)
{
	//put your SDL / game code here

	SDL_Window* window;                    // Declare a pointer

	SDL_Init(SDL_INIT_VIDEO);              // Initialize SDL2
	TTF_Init();
	// Create an application window with the following settings:
	window = SDL_CreateWindow(
		"An SDL2 window",                  // window title
		SDL_WINDOWPOS_UNDEFINED,           // initial x position
		SDL_WINDOWPOS_UNDEFINED,           // initial y position
		640,                               // width, in pixels
		480,                               // height, in pixels
		SDL_WINDOW_OPENGL                  // flags - see below
	);

	// Check that the window was successfully created
	if (window == NULL) {
		// In the case that the window could not be made...
		SDL_Log("Could not create window: %s\n", SDL_GetError());
		return 1;
	}

	SDL_Renderer* renderer = NULL;
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	TTF_Font* font = TTF_OpenFontRW(SDL_RWFromFile("fonts/OpenSans-Regular.ttf", "rb"), 1, 24);
	if (!font)
	{
		SDL_Log("Could not load font %s\n", SDL_GetError());
	}
	SDL_Color textColor = { 255, 255, 255, 0 };
	SDL_Surface* surface = TTF_RenderText_Solid(font, "Kibande", textColor);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_Rect rect_tex = { 200, 200, surface ? surface->w : 0, surface ? surface->h : 0 };
	SDL_FreeSurface(surface);

	while (1) {
		SDL_Event e;
		if (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				break;
			}
		}

		SDL_SetRenderDrawColor(renderer, 0x0, 0x0, 0x0, 0xff);
		SDL_RenderClear(renderer);
		SDL_SetRenderDrawColor(renderer, 0xff, 0x00, 0x00, 0xff);
		SDL_Rect rect = { 100, 100, 200, 200 };
		SDL_RenderFillRect(renderer, &rect);
		if (texture)
			SDL_RenderCopy(renderer, texture, NULL, &rect_tex);
		SDL_RenderPresent(renderer);
	}
	if (texture)
		SDL_DestroyTexture(texture);
	if (renderer)
		SDL_DestroyRenderer(renderer);

	// Close and destroy the window
	SDL_DestroyWindow(window);

	// Clean up
	TTF_Quit();
	SDL_Quit();

	return 0;
}

#endif // SQB_PLATFORM_ANDROID



#else

#pragma once
#include "App.h"
unsigned windowFlags = 0;
int main(int argc, char** argv) {
	App app;
	SquareBox::InputDevicesEnum input_device;
#ifdef SQB_PLATFORM_ANDROID
	input_device = SquareBox::InputDevicesEnum::TouchScreen;
	//windowFlags |= SquareBox::RenderEngine::WindowFlags::WINDOW_FULLSCREEN;
	//windowFlags |= SquareBox::RenderEngine::WindowFlags::WINDOW_BORDERLESS;
	//windowFlags |= SquareBox::RenderEngine::WindowFlags::WINDOW_FULLSCREEN_DESKTOP;
	//windowFlags |= SquareBox::RenderEngine::WindowFlags::WINDOW_MAXIMIZED;

#else
	input_device = SquareBox::InputDevicesEnum::KeyBoardAndMouse;
	windowFlags |= SquareBox::RenderEngine::WindowFlags::WINDOW_RESIZABLE;
	windowFlags |= SquareBox::RenderEngine::WindowFlags::WINDOW_MAXIMIZED;
#endif // SQB_PLATFORM_ANDROID
	
	app.run("Zombie Game", 800, 600, windowFlags, 60, 0.8f, input_device, SquareBox::VideoDecorderEnum::None, SquareBox::RenderEngine::ColorRGBA8(43, 43, 48, 255), "test/SquareBox-Editor.txt", true);
}

#endif // TESTMODE















//#define SERVER = 1;
//#ifdef SERVER
//#include <SquareBox-Core/SquareBoxGameEngine.h>
//
//enum class CustomMsgTypes : uint32_t
//{
//	ServerAccept,
//	ServerDeny,
//	ServerPing,
//	MessageAll,
//	ServerMessage,
//};
//
//
//
//class CustomServer : public SquareBox::net::server_interface<CustomMsgTypes>
//{
//public:
//	CustomServer(uint16_t nPort) : SquareBox::net::server_interface<CustomMsgTypes>(nPort)
//	{
//
//	}
//
//protected:
//	virtual bool OnClientConnect(std::shared_ptr<SquareBox::net::connection<CustomMsgTypes>> client)
//	{
//		SquareBox::net::message<CustomMsgTypes> msg;
//		msg.header.id = CustomMsgTypes::ServerAccept;
//		client->Send(msg);
//		return true;
//	}
//
//	// Called when a client appears to have disconnected
//	virtual void OnClientDisconnect(std::shared_ptr<SquareBox::net::connection<CustomMsgTypes>> client)
//	{
//		std::cout << "Removing client [" << client->GetID() << "]\n";
//	}
//
//	// Called when a message arrives
//	virtual void OnMessage(std::shared_ptr<SquareBox::net::connection<CustomMsgTypes>> client, SquareBox::net::message<CustomMsgTypes>& msg)
//	{
//		switch (msg.header.id)
//		{
//		case CustomMsgTypes::ServerPing:
//		{
//			std::cout << "[" << client->GetID() << "]: Server Ping\n";
//
//			// Simply bounce message back to client
//			client->Send(msg);
//		}
//		break;
//
//		case CustomMsgTypes::MessageAll:
//		{
//			std::cout << "[" << client->GetID() << "]: Message All\n";
//
//			// Construct a new message and send it to all clients
//			SquareBox::net::message<CustomMsgTypes> msg;
//			msg.header.id = CustomMsgTypes::ServerMessage;
//			msg << client->GetID();
//			MessageAllClients(msg, client);
//
//		}
//		break;
//		}
//	}
//};
//
//int main()
//{
//	CustomServer server(60000);
//	server.Start();
//
//	while (1)
//	{
//		server.Update(-1, true);
//	}
//
//
//
//	return 0;
//}
//
//#endif
