#pragma once
#include "App.h"
#include <Windows.h>
int main(int argc, char** argv) {
	App app;
	int windowFlags = 0;
	SquareBox::InputDevicesEnum input_device;
#ifdef SQB_PLATFORM_ANDROID
	input_device = SquareBox::InputDevicesEnum::TouchScreen;
#else
	input_device = SquareBox::InputDevicesEnum::KeyBoardAndMouse;
	FreeConsole();
#endif // SQB_PLATFORM_ANDROID
	windowFlags |= SquareBox::RenderEngine::WindowFlags::WINDOW_FULLSCREEN;
	windowFlags |= SquareBox::RenderEngine::WindowFlags::WINDOW_RESIZABLE;
	app.run("TileSheet Visualizer", 1300, 700, windowFlags, 60, 0.8f, input_device, SquareBox::VideoDecorderEnum::None, SquareBox::RenderEngine::ColorRGBA8(43, 43, 48, 255),"tilesheet_visualizer_logs.logs",true);

	exit(0);
}




	/* Arguments
		The Windows Title
		Window Dimensions
		Window flags
		Desired Game FPS
		Max Delta Time
		Input Device
		Video Decorder
		Background Color of The window

*/
//	the int argc, char** argv  main commandline agrugments are required by sdl
//  so they are must have

//
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
//


//#pragma once
//
//#ifdef SQB_PLATFORM_ANDROID
//#include <GLES3/gl3.h>
//#include <GLES3/gl3ext.h>
//#else
//#include<GL/glew.h>
//#endif
//
//#include <iostream>
//#include <string>
//#include <SDL\include\SDL.h>
//#include "Sprite.h"
//#include <Render-Engine/RenderingEngine.h>
//#include <Asset-Manager/Asset-Manager.h>
//Sprite test_sprite;
//bool run_game = true;
//SDL_Window* _window = nullptr;
//SquareBox::AssetManager::GLTexture player_texture;
//float _time = 0.0f;
//SquareBox::RenderEngine::GLSLProgram texture_program;
//void fatalError(std::string errorString) {
//	std::cout << errorString << std::endl;
//	std::cout << "Enter any key to quit....";
//		int tmp;
//	std::cin >> tmp;
//}
//
//
//void initSystems() {
//	SDL_Init(SDL_INIT_EVERYTHING);
//
//	 _window = SDL_CreateWindow("Game Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 400, 400, SDL_WINDOW_OPENGL);
//
//	if (_window == nullptr) {
//		fatalError("SDL Window could not be created");
//	}
//
//	//Create an opengle context
//
//	SDL_GLContext glContext = SDL_GL_CreateContext(_window);
//
//	if (glContext == nullptr) {
//		fatalError("SDL_GL context could not be created");
//	}
//
//	GLenum error =glewInit();
//	if (error != GLEW_OK) {
//		fatalError("Failed to set up do glew Init");
//	}
//
//	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
//
//	glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
//
//	SquareBox::Log::Init();
//	//do shader compilation and linking
//	
//	texture_program.compileShaders("Assets/Shaders/colorShading.vert", "Assets/Shaders/colorShading.frag");
//	texture_program.addAttribute("vertexPosition");//since we are using sprite batch we have to always set these attributes
//	texture_program.addAttribute("vertexColor");
//	texture_program.addAttribute("vertexUV");
//	texture_program.linkShaders();
//
//	player_texture = SquareBox::AssetManager::TextureManager::getTextureByFilePath("Assets/Textures/glass.png");
//}
//
//
//void drawGame() {
//
//	glClear(1.0f);
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//	texture_program.use();
//	_time += 0.3f;
//	//GLuint timeLocation = texture_program.getUnifromLocation("time");
//	//glUniform1f(timeLocation, _time);
//
//	//we wont be using multi texturing  here so , lets active only the first texture slot
//	glActiveTexture(GL_TEXTURE0);
//	//bind the texture
//	glBindTexture(GL_TEXTURE_2D, player_texture.id);
//	//Tell the sampler which texture unit its going to use
//	GLuint texture_location = texture_program.getUnifromLocation("mySampler");
//	glUniform1i(texture_location,0);///0 since we are using the first texture GL_TEXTURE0
//	test_sprite.draw();
//
//	//unbind
//	glBindTexture(GL_TEXTURE_2D, 0);
//	texture_program.unuse();
//
//	SDL_GL_SwapWindow(_window);//swap the buffer so that the next draw happens on the other buffer
//
//}
//
//
//void processInput() {
//	SDL_Event evnt;
//	while (SDL_PollEvent(&evnt))
//	{
//		switch (evnt.type)
//		{
//		case SDL_QUIT:
//			run_game = false;
//			break;
//		case SDL_MOUSEMOTION:
//			std::cout << evnt.motion.x << " " << evnt.motion.y <<std::endl;
//			break;
//		default:
//			break;
//		}
//	}
//}
//
// 
//void gameLoop() {
//	while (run_game)
//	{
//		processInput();
//		drawGame();
//	}
//}
//
//int main(int argc, char** argv) {
//	initSystems();
//	test_sprite.init(-1.0f,-1.0f,1.0f,1.0f);
//	gameLoop();
//	texture_program.dispose();
//	SDL_Quit();
//	return 0;
//}
