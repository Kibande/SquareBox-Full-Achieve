///*
//	0775 642626 Patrick mwaka
//	0706 260228 christophere oloya
//*/

//#define TESTMODE 1

#ifdef TESTMODE

#ifdef SQB_PLATFORM_ANDROID



#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "SDL/include/SDL.h"

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <string>

using namespace std;

class Graphics
{
public:
	void update() {

	}
};

int main(int argc, char* argv[]) {
	SDL_Window* window = 0;
	SDL_GLContext gl = 0;

	if (0 != SDL_Init(SDL_INIT_VIDEO)) {
		fprintf(stderr, "\nUnable to initialize SDL: %s\n", SDL_GetError());
	}

	string s = "This is on stl::string";

	SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);


	SDL_DisplayMode mode;
	SDL_GetDisplayMode(0, 0, &mode);
	int width = mode.w;
	int height = mode.h;

	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	window = SDL_CreateWindow(NULL, 0, 0, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN | SDL_WINDOW_RESIZABLE);

	if (window == 0) {
		SDL_Quit();
		return 1;
	}

	gl = SDL_GL_CreateContext(window);

	/* Main render loop*/
	uint done = 0;
	SDL_Event event;
	int count = 0;
	while (!done) {

		glClearColor((rand() % 256) / 256.0f, (rand() % 256) / 256.0f, (rand() % 256) / 256.0f, (rand() % 256) / 256.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		SDL_GL_SwapWindow(window);
		SDL_Delay(10);
	}

	exit(0);
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
	windowFlags |= SquareBox::RenderEngine::WindowFlags::WINDOW_FULLSCREEN;
	//windowFlags |= SquareBox::RenderEngine::WindowFlags::WINDOW_BORDERLESS;
	//windowFlags |= SquareBox::RenderEngine::WindowFlags::WINDOW_FULLSCREEN_DESKTOP;
	//windowFlags |= SquareBox::RenderEngine::WindowFlags::WINDOW_MAXIMIZED;

#else
	input_device = SquareBox::InputDevicesEnum::KeyBoardAndMouse;
	//windowFlags |= SquareBox::RenderEngine::WindowFlags::WINDOW_RESIZABLE;
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
