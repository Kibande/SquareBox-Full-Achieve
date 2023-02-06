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
	Destory
	Deconstructor

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
	//some cache friendlness
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


	// Initialize COM library
	CoInitialize(NULL);

	// Create voice object
	ISpVoice* pVoice = NULL;
	if (FAILED(::CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void**)&pVoice)))
	{
		std::cout << "Error creating voice object" << std::endl;
	}

	std::string text = "something of converting text to speech";
	wchar_t wtext[20];
	mbstowcs(wtext, text.c_str(), text.length());//includes null
	LPWSTR ptr = wtext;
	// Convert text to speech
	if (FAILED(pVoice->Speak(wtext, SPF_DEFAULT, NULL)))
	{
		std::cout << "Error converting text to speech" << std::endl;
	}

	// Release voice object
	pVoice->Release();
	pVoice = NULL;

	// Uninitialize COM library
	CoUninitialize();
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

																			  
	//m_window->resizable(false);//We are choosing to disable the windows ability to resize
	//The backGround Color is keep in IMainGame
	//m_game_ptr->setProcessingInput(false);

	//setting up my cluster object
	SquareBox::GWOM::ClusterObject & active_cluster_object = m_vec_of_world_clusters[0].cluster_objects[0];
	auto loaded_character_texture = SquareBox::AssetManager::TextureManager::getTextureByFilePath("Assets/Textures/nodpi/human.png");
	auto loaded_houses_texture = SquareBox::AssetManager::TextureManager::getTextureByFilePath("Assets/Textures/nodpi/blue_grey.png");
	SquareBox::AssetManager::TextureManager::setTextureTilingById(loaded_houses_texture.id, glm::vec2(5,2));
	active_cluster_object.texture_info.texture_id= loaded_character_texture.id;
	active_cluster_object.texture_info.texture_index = 0;
	active_cluster_object.texture_info.texture_type = SquareBox::TextureEnum::SINGLE;
	active_cluster_object.texture_info.tile_sheet_index = 0;
	active_cluster_object.texture_info.uv_rect = glm::vec4(0.0f,0.0f,1.0f,1.0f);

	active_cluster_object.is_alive = true;
	active_cluster_object.texture_info.color = SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::white).getIVec4();
	active_cluster_object.shape = SquareBox::BodyShapeEnum::Circle;

	
	SquareBox::GWOM::ClusterObject & house = m_vec_of_world_clusters[0].cluster_objects[1];
	house.texture_info.texture_id = loaded_houses_texture.id;
	house.texture_info.texture_type = SquareBox::TextureEnum::TILESHEET;
	house.texture_info.tile_sheet_index = 0;
	//extracting the appropriate uvRect
	house.texture_info.uv_rect = SquareBox::AssetManager::TextureManager::getTextureById(loaded_houses_texture.id).getUVReactAtIndex(0);

	house.shape = SquareBox::BodyShapeEnum::Box;
	house.is_alive = true;
	house.texture_info.color = SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::white).getIVec4();

	//cluster objects adaptive properties
	active_cluster_object.radius = screen_dimensions.x * 0.03;
	active_cluster_object.position = screen_dimensions	* glm::vec2(0.5);
	house.height = screen_dimensions.x * 0.08;
	house.width = screen_dimensions.x * 0.08;
	house.position = screen_dimensions*glm::vec2(0.2);

}

void First_Screen::update(const float & deltaTime_)
{
	//Called once every game loop , and updates what will be drawn
	glm::vec2 screen_dimensions(m_game_ptr->getWindow()->getScreenWidth(), m_game_ptr->getWindow()->getScreenHeight());
	m_layers[m_active_layer_index].camera.update(screen_dimensions.x, screen_dimensions.y);

	//m_automation.update(m_vec_of_world_clusters,m_layers[m_active_layer_index].alive_cluster_objects,deltaTime_);

	//cluster objects adaptive properties
	SquareBox::GWOM::ClusterObject & active_cluster_object = m_vec_of_world_clusters[0].cluster_objects[0];
	SquareBox::GWOM::ClusterObject & house = m_vec_of_world_clusters[0].cluster_objects[1];
	active_cluster_object.radius = screen_dimensions.x * 0.03;
	house.height = screen_dimensions.x * 0.08;
	house.width = screen_dimensions.x * 0.08;
	house.position = screen_dimensions * glm::vec2(0.2);

#ifdef SQB_PLATFORM_ANDROID
	//get screen motion coordinates from the input manager
	m_num_fingers_on_screen = m_game_ptr->getInputDevice()->getScreenLocations().size();

	if (m_num_fingers_on_screen > 0) {
		glm::vec2 point_on_screen = m_layers[m_active_layer_index].camera.convertScreenToWorld(m_game_ptr->getInputDevice()->getScreenLocations()[0].coordinates*screen_dimensions);
		active_cluster_object.position = point_on_screen;
	}
#else
	if (m_game_ptr->getInputDevice()->isInputIdBeingReceived((int)SquareBox::MouseEnum::LEFT_CLICK)) {
		glm::vec2 mouse_in_world = m_layers[m_active_layer_index].camera.convertScreenToWorld(m_game_ptr->getInputDevice()->getScreenLocations()[0].coordinates);
		active_cluster_object.position = mouse_in_world;

}
#endif // SQB_PLATFORM_ANDROID

}

void First_Screen::draw()
{
	
	////Draw call
	m_texture_program.use();

	IGameScreen::preUpdateShader(&m_texture_program, "mySampler");
	//maintaining our backGround Color

	IGameScreen::uploadCameraInfo(&m_texture_program, &m_layers[m_active_layer_index].camera, "P");

	m_sprite_batch.begin();
	//m_sprite_font or m_sprite_batch draw
#ifdef SQB_PLATFORM_ANDROID
	glm::vec2 screen_dimensions(std::max(m_game_ptr->getWindow()->getScreenWidth(), m_game_ptr->getWindow()->getScreenHeight()), std::min(m_game_ptr->getWindow()->getScreenWidth(), m_game_ptr->getWindow()->getScreenHeight()));
	std::ostringstream os;
	os << m_game_ptr->getFps();
	m_sprite_font.draw(m_sprite_batch, os.str().c_str(), glm::vec2(screen_dimensions.x*0.1 ,screen_dimensions.y*0.9), glm::vec2(1.0f), 1.0f, SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::lemon_chiffon), SquareBox::JustificationEnum::MIDDLE);

#else
	m_sprite_font.draw(m_sprite_batch, std::to_string(m_game_ptr->getFps()).c_str(), glm::vec2(m_game_ptr->getWindow()->getScreenWidth()*0.1, m_game_ptr->getWindow()->getScreenHeight()*0.9), glm::vec2(1.0f), 1.0f, SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::lemon_chiffon), SquareBox::JustificationEnum::MIDDLE);
#endif
	SquareBox::GWOM::ClusterObject & active_cluster_object = m_vec_of_world_clusters[0].cluster_objects[0];
	SquareBox::GWOM::ClusterObject & house = m_vec_of_world_clusters[0].cluster_objects[1];
	m_utilities.setCurrentShapePointer(active_cluster_object.shape,&m_shape_pointer);
	m_shape_pointer->draw(active_cluster_object, m_sprite_batch, 100);
	m_utilities.setCurrentShapePointer(house.shape,&m_shape_pointer);
	m_shape_pointer->draw(house, m_sprite_batch, 100);
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