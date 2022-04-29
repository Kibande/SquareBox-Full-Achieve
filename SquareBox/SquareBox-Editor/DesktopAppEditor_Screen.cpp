#include "DesktopAppEditor_Screen.h"
#include "ScreenIndices.h"
#include<glm/gtx/vector_angle.hpp>
DesktopAppEditor_Screen::DesktopAppEditor_Screen(SquareBox::RenderEngine::Window* window) :m_window(window)
{
}

DesktopAppEditor_Screen::~DesktopAppEditor_Screen()
{
}

int DesktopAppEditor_Screen::getNextScreenIndex() const
{
	return SCREEN_INDEX_NO_SCREEN;
}

int DesktopAppEditor_Screen::getPreviousScreenIndex() const
{
	return WELCOME_SCREEN_INDEX;
}

void DesktopAppEditor_Screen::build()
{
}

void DesktopAppEditor_Screen::destroy()
{
}

void DesktopAppEditor_Screen::onEntry()
{
	m_spriteBatch.init();
	m_debugRender.init();
	m_spriteFont.init("Assets/Fonts/Comfortaa-Bold.ttf", 32);

	//Init Shaders
	m_textureProgram.compileShaders("Assets/Shaders/colorShading.vert", "Assets/Shaders/colorShading.frag");
	m_textureProgram.addAttribute("vertexPosition");
	m_textureProgram.addAttribute("vertexColor");
	m_textureProgram.addAttribute("vertexUV");
	m_textureProgram.linkShaders();


	//the debugProgram
	m_debug_program.compileShaders("Assets/Shaders/debugShading.vert", "Assets/Shaders/debugShading.frag");
	m_debug_program.addAttribute("vertexPosition");
	m_debug_program.addAttribute("vertexColor");
	m_debug_program.linkShaders();

	//Init Cameras
	m_layer.camera.init(m_window->getScreenWidth(), m_window->getScreenHeight());
	m_layer.camera.setScale(162.0f);
	m_layer.camera.setPosition(glm::vec2(0));//Center the camera
	
	//input
	m_game_ptr->setProcessingInput(true);

	////Icon
	//auto  pixel_buffer = SquareBox::AssetManager::IOManager::getPixelDataFromImageFile("Assets/Textures/walls/codeblocks.png");
	//
	//  m_window->setWindowIcon(pixel_buffer.pixels, pixel_buffer.width, pixel_buffer.height);

	//  SquareBox::AssetManager::IOManager::freePixelData(pixel_buffer);

	//  //Window Opactity
	//  m_window->setWindowOpacity(0.1f);

	

	//setting up our box parameters
	box.height = 280.0f/162;
	box.width = 180.0f/162;
	box.is_alive = true;
	box.is_hidden = false;
	box.angle = 0.0f;

	//m_layer.tiling_system.init(-200, -200, 400, 400, 0.5f,SquareBox::LayerTilingEnum::FlatTiling);

	m_utilities.init();
}

void DesktopAppEditor_Screen::onExit()
{
	m_utilities.dispose();
	m_spriteBatch.dispose();
	m_spriteFont.dispose();
	m_textureProgram.dispose();
}

void DesktopAppEditor_Screen::update(const float & deltaTime_)
{
	m_window->update();
	m_layer.camera.update(m_window->getScreenWidth(), m_window->getScreenHeight());
	m_assistant.cameraControls(m_layer.camera, m_game_ptr);
	//Get the mouse coordinates
	glm::vec2 mouse_in_world = m_layer.camera.convertScreenToWorld(m_game_ptr->getInputManager()->getScreenLocations()[0].coordinates);
	box.position = mouse_in_world-glm::vec2(box.width,box.height)*0.5f;
}

void DesktopAppEditor_Screen::draw()
{
	//m_texture_program.use();

	//IGameScreen::preUpdateShader(&m_texture_program, "mySampler");

	//IGameScreen::uploadCameraInfo(&m_texture_program, &m_layer.camera, "P");

	//m_sprite_batch.begin();
	//char buffer[256];
	////m_sprite_font or m_sprite_batch draw
	//sprintf(buffer, "FPS %d", m_game_ptr->getFps());
	//m_sprite_font.draw(m_sprite_batch, buffer, m_layer.camera.getPosition(), glm::vec2(0.01f), 0.0f, SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::white));
	//m_sprite_batch.end();
	//m_sprite_batch.renderBatch();

	//m_texture_program.unuse();

	//m_debug_program.use();
	//m_debugRender.begin();
	////draw grid
	//if (m_layer.layer_grid.isInitialised() && m_layer.tiling == SquareBox::LayerTilingEnum::FlatTiling) {
	//	glm::vec2 camera_center = m_layer.camera.getPosition();
	//	float camera_width = m_layer.camera.getCameraDimensions().x;
	//	float camera_height = m_layer.camera.getCameraDimensions().y;
	//	glm::vec4 camera_destRect(camera_center - glm::vec2(camera_width, camera_height)*0.5f, glm::vec2(camera_width, camera_height));
	//	m_debugRender.drawBox(camera_destRect, SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::pink), 0.0f);
	//	
	//		
	//	auto cells_in_camera_view = m_layer.layer_grid.getAllCellsInBox(camera_destRect);

	//	for (auto it = cells_in_camera_view.begin(); it != cells_in_camera_view.end(); it++)
	//	{
	//		m_debugRender.drawBox(glm::vec4((*it).second->position, glm::vec2(m_layer.layer_grid.getCellSize())), SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::white), 0.0f);
	//	}


	//	auto cells_in_box = m_layer.layer_grid.getAllCellsInBox(glm::vec4(box.position, box.width, box.height));

	//	for (auto it = cells_in_box.begin(); it != cells_in_box.end(); it++)
	//	{
	//		m_debugRender.drawBox(glm::vec4((*it).second->position, glm::vec2(m_layer.layer_grid.getCellSize())), SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::orange), 0.0f);
	//	}

	//}


	////drawing the scaling area
	//m_debugRender.drawBox(glm::vec4(box.position, box.width, box.height), SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::green), 0.0f);
	//m_debugRender.end();
	//uploadCameraInfo(&m_debug_program, &m_layer.camera, "P");
	//m_debugRender.render();
	//m_debug_program.unuse();
}

