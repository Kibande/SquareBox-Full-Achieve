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
	m_sprite_batch.init();
	m_debug_render.init();
	m_sprite_font.init("Assets/Fonts/Comfortaa-Bold.ttf", 32);

	//Init Shaders
	m_texture_program.compileShaders("Assets/Shaders/colorShading.vert", "Assets/Shaders/colorShading.frag");
	m_texture_program.addAttribute("vertexPosition");
	m_texture_program.addAttribute("vertexColor");
	m_texture_program.addAttribute("vertexUV");
	m_texture_program.linkShaders();


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
	m_sprite_batch.dispose();
	m_sprite_font.dispose();
	m_texture_program.dispose();
}

void DesktopAppEditor_Screen::update(const float & deltaTime_)
{
	m_layer.camera.update(m_window->getScreenWidth(), m_window->getScreenHeight());
	m_assistant.cameraControls(m_layer.camera, m_game_ptr);
	//Get the mouse coordinates
	glm::vec2 mouse_in_world = m_layer.camera.convertScreenToWorld(m_game_ptr->getInputDevice()->getScreenLocations()[0].coordinates);
	box.position = mouse_in_world-glm::vec2(box.width,box.height)*0.5f;
}

void DesktopAppEditor_Screen::draw()
{
	m_texture_program.use();

	IGameScreen::preUpdateShader(&m_texture_program, "mySampler");

	IGameScreen::uploadCameraInfo(&m_texture_program, &m_layer.camera, "P");

	m_sprite_batch.begin();
	char buffer[256];
	//m_sprite_font or m_sprite_batch draw
	sprintf(buffer, "FPS %d", m_game_ptr->getFps());
	m_sprite_font.draw(m_sprite_batch, buffer, m_layer.camera.getPosition(), glm::vec2(0.01f), 0.0f, SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::white));
	
	glm::vec4 dest_rect = glm::vec4(box.position - glm::vec2(box.width, box.height) * 0.5f, box.width, box.height);
	m_sprite_batch.draw(dest_rect, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), box.texture_info.texture_id, 1.0f, box.color);
	m_sprite_batch.end();
	
	m_sprite_batch.renderBatch();
	m_texture_program.unuse();

	m_debug_program.use();
	m_debug_render.begin();
	//draw grid
	

	//drawing the scaling area
	m_debug_render.drawBox(dest_rect, SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::green),0.0f);
	m_debug_render.end();
	uploadCameraInfo(&m_debug_program, &m_layer.camera, "P");
	m_debug_render.render();
	m_debug_program.unuse();
}

