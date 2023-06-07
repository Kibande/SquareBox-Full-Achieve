#include "First_Screen.h"
#include <iostream>
#include <Windows.h>
#include <sapi.h>
/*
Order of Methods Calling
	Constructor
	Build
	OnEntry

	//game loop
	 (while ScreenState::RUNNING){
		 Update
		 Draw
	   }

	OnExit
	Destroy
	De-constructor

*/

First_Screen::First_Screen()
{
}

int First_Screen::getNextScreenIndex() const
{
	return m_next_screen_index;
}

int First_Screen::getPreviousScreenIndex() const
{
	return m_previous_screen_index;
}

void First_Screen::build()
{
	//Create our first layer
	m_layers.emplace_back();
	//get a reference to the last placed layer
	SquareBox::GWOM::Layer& layer = m_layers.back();
	layer.alive_cluster_objects.clear();
	//some cache friendliness
	layer.alive_cluster_objects.reserve(60);
	layer.single_textures.reserve(5);
	layer.tiled_textures.reserve(10);
	glm::vec2 screen_dimensions(m_game_ptr->getWindow()->getScreenWidth(), m_game_ptr->getWindow()->getScreenHeight());
	layer.camera.init(screen_dimensions.x, screen_dimensions.y);

	layer.camera.setScale(1.0f);

	layer.is_visible = true;
	layer.is_locked = false;
	m_active_layer_index = 0;
	m_utilities.init();

	//set up our first dummy character
	m_vec_of_world_clusters.emplace_back();
	m_vec_of_world_clusters.back().index = 0;
	m_vec_of_world_clusters.back().cluster_objects.emplace_back();
	SquareBox::GWOM::ClusterObject & active_cluster_object = m_vec_of_world_clusters.back().cluster_objects.back();
	active_cluster_object.cluster_index = 0;
	active_cluster_object.index = 0;
	//m_utilities.nameClusterObjectByGivenName(active_cluster_object, "targetBox");

	//send the house back as well
	m_vec_of_world_clusters.back().cluster_objects.emplace_back();
	SquareBox::GWOM::ClusterObject & house = m_vec_of_world_clusters.back().cluster_objects.back();
	house.cluster_index = 0;
	house.index = 1;
	m_utilities.nameClusterObjectByGivenName(house, "house");

	m_layers[m_active_layer_index].alive_cluster_objects.push_back(std::pair<int, int>(0, 0));
	m_layers[m_active_layer_index].alive_cluster_objects.push_back(std::pair<int, int>(0, 1));

	const int width = 1000;
	const int height = 1000;
	const int length = (width * height) * 4;

	float*  data = new float[length];
	int i = 0;
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {

			data[i] = 1.0f;
			data[i + 1] = 1.0f;
			data[i + 2] = 1.0f;
			data[i + 3] = 1.0f;
			i += 4;
		}
	}


	//SquareBox::AssetManager::GLTexture m_texture = SquareBox::AssetManager::TextureManager::getTextureByFilePath("Assets/Textures/nodpi/white background.png");
	auto  fileBuffer = std::pair<float*, int>(data, length);
	m_texture = SquareBox::AssetManager::TextureManager::createTextureFromFileBuffer(fileBuffer,width,height);
	
	delete [] data; // since the data is stored on the CPU

}

void First_Screen::onEntry()
{
	// this is where  stuff  the screen will use are  initialized from
	m_sprite_batch.init();
	m_automation.init();
	m_automation.loadAutomationScript(m_automation_script);
#ifdef SQB_PLATFORM_ANDROID
	m_sprite_font.initWithFilePath("fonts/Comfortaa-Bold.ttf", 32);
#else
	m_sprite_font.initWithFilePath("Assets/Fonts/Comfortaa-Bold.ttf", 32);
#endif

	m_texture_program.compileShaders("Assets/Shaders/colorShading.vert", "Assets/Shaders/colorShading.frag");
	m_texture_program.addAttribute("vertexPosition");
	m_texture_program.addAttribute("vertexColor");
	m_texture_program.addAttribute("vertexUV");
	m_texture_program.linkShaders();

	//this is where the screens default parameters are set
	glm::vec2 screen_dimensions(m_game_ptr->getWindow()->getScreenWidth(), m_game_ptr->getWindow()->getScreenHeight());
	m_layers[m_active_layer_index].camera.setPosition(glm::vec2(screen_dimensions.x / 2, screen_dimensions.y / 2));//Center the camera
}

void First_Screen::update(const float & deltaTime_)
{
	//Called once every game loop , and updates what will be drawn
	glm::vec2 screen_dimensions(m_game_ptr->getWindow()->getScreenWidth(), m_game_ptr->getWindow()->getScreenHeight());
	m_layers[m_active_layer_index].camera.update(screen_dimensions.x, screen_dimensions.y);
}

void First_Screen::draw()
{
	////Draw call
	m_texture_program.use();

	IGameScreen::preUpdateShader(&m_texture_program, "mySampler");
	//maintaining our backGround Color

	IGameScreen::uploadCameraInfo(&m_texture_program, &m_layers[m_active_layer_index].camera, "P");

	m_sprite_batch.begin();

	SquareBox::RenderEngine::ColorRGBA8 col = SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::green);
	glm::vec2 screen_dimensions(m_game_ptr->getWindow()->getScreenWidth(), m_game_ptr->getWindow()->getScreenHeight());
	m_sprite_batch.draw(glm::vec4(screen_dimensions*0.5f, screen_dimensions*0.1f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), m_texture.id, 1.0f, col);

	m_sprite_batch.end();
	m_sprite_batch.renderBatch();
	m_texture_program.unuse();
}

void First_Screen::onExit()
{
	//	//this is where objects that the screen used are destroyed from
	m_sprite_batch.dispose();
	m_sprite_font.dispose();
	m_texture_program.dispose();
}

void First_Screen::destroy()
{
	//Called when the Screen is removed from the screenList
}

First_Screen::~First_Screen()
{
}