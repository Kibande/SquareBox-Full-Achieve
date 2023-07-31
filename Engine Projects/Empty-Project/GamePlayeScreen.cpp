#include "GamePlayeScreen.h"

#define ASIO_STANDALONE
#include <asio/asio.hpp>
#include<asio/ts/buffer.hpp>
#include<asio/ts/internet.hpp>


std::vector<char> vBuffer(20 * 1024);

void GrabSomeData(asio::ip::tcp::socket& socket) {
	socket.async_read_some(asio::buffer(vBuffer.data(), vBuffer.size()),
		[&](std::error_code ec, std::size_t length) {

		if (!ec) {
			for (size_t i = 0; i < vBuffer.size(); i++)
			{
				std::cout << vBuffer[i];
			}

			GrabSomeData(socket);
		}
	});
}

GamePlayeScreen::GamePlayeScreen()
{
	

}

int GamePlayeScreen::getNextScreenIndex() const
{
	return 0;
}

int GamePlayeScreen::getPreviousScreenIndex() const
{
	return 0;
}

void GamePlayeScreen::build()
{
	m_camera.init(m_game_ptr->getWindow()->getScreenWidth(), m_game_ptr->getWindow()->getScreenHeight());
	m_camera.setScale(1.0f);
	m_camera.setPosition(glm::vec2(m_game_ptr->getWindow()->getScreenWidth() *0.5f, m_game_ptr->getWindow()->getScreenHeight()*0.5f));

	//Initialize our texture program
	m_texture_program.init();
	m_texture_program.compileDefaultTextureShaders();
	m_texture_program.addDefaultTextureAttributes();
	m_texture_program.linkShaders();

	m_sprite_batch.init();


	asio::error_code ec;

	//Create a context essentially the platform specific interface
	asio::io_context context;

	// Get the address of somewhere that we wish to connect
	asio::ip::tcp::endpoint endpoint(asio::ip::make_address("93.184.216.34", ec), 443);

	//Create a socket , the context will deliver the implementation
	asio::ip::tcp::socket socket(context);


	// ask the socket to try and connect to the endpoint
	socket.connect(endpoint, ec);

	if (!ec) {
		std::cout << "Connected!" << std::endl;
	}
	else {

		SBX_CORE_ERROR("Failed to connect to address: 93.184.216.34");
		SBX_CORE_ERROR("{} ", ec.message().c_str());
	}

	if (socket.is_open()) {


		std::string sRequest = 
			"GET /index.html HTTP/1.1\r\n"
			"Host: example.com\r\n"
			"Connection: close\r\n\r\n";

		socket.write_some(asio::buffer(sRequest.data(), sRequest.size()), ec);

		socket.wait(socket.wait_read);

		if (!ec) {
			std::cout << "Successfully sent out request!" << std::endl;
		}
		else {

			SBX_CORE_ERROR("{} ", ec.message().c_str());
		}

		size_t bytes = socket.available();

		std::cout << "Bytes Available: " << bytes << std::endl;

		if (bytes > 0) {

			std::vector<char> vBuffer(bytes);
			socket.read_some(asio::buffer(vBuffer.data(), vBuffer.size()), ec);

			for (size_t i = 0; i < vBuffer.size(); i++)
			{
				std::cout << vBuffer[i];
			}
		}

	
	}
}

void GamePlayeScreen::onEntry()
{

}



void GamePlayeScreen::update(const float & deltaTime_)
{


}

void GamePlayeScreen::draw()
{
	m_texture_program.use();
	preUpdateShader(&m_texture_program, "mySampler");
	uploadCameraInfo(&m_texture_program, &m_camera, "P");

	m_sprite_batch.begin();



	m_sprite_batch.end();
	m_sprite_batch.renderBatch();

	m_texture_program.unuse();

}

void GamePlayeScreen::onExit()
{

}

void GamePlayeScreen::destroy()
{
	m_texture_program.dispose();
	m_camera.dispose();
	m_sprite_batch.dispose();
}


