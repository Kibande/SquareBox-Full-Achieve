#include "GamePlayScreen.h"

int GamePlayScreen::getNextScreenIndex() const
{
	return 0;
}

int GamePlayScreen::getPreviousScreenIndex() const
{
	return 0;
}

void GamePlayScreen::build()
{
}

void GamePlayScreen::onEntry()
{
	m_texture_program.init();
	m_texture_program.compileShaders("Assets/Shaders/colorShading.vert", "Assets/Shaders/colorShading.frag");
	m_texture_program.addAttribute("vertextPosition");
	m_texture_program.addAttribute("vertextColor");
	m_texture_program.addAttribute("vertextUV");
	m_texture_program.linkShaders();

	m_sprite_batch.init();
	m_layer.camera.init(m_window_ptr->getScreenWidth(), m_window_ptr->getScreenHeight());
	m_layer.camera.setPosition(glm::vec2(m_window_ptr->getScreenWidth(), m_window_ptr->getScreenHeight())*0.5f);


	nLevelWidth = 64;
	nLevelHeight = 16;
	
	sLevel.append("................................................................");
	sLevel.append("................................................................");
	sLevel.append(".......ooooo....................................................");
	sLevel.append("........ooo.....................................................");
	sLevel.append(".......................########.................................");
	sLevel.append(".....BB?BBBB?BB.......###..............#.#......................");
	sLevel.append("....................###................#.#......................");
	sLevel.append("...................####.........................................");
	sLevel.append("GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG.##############.....########");
	sLevel.append("...................................#.#...............###........");
	sLevel.append("........................############.#............###...........");
	sLevel.append("........................#............#.........###..............");
	sLevel.append("........................#.############......###.................");
	sLevel.append("........................#................###....................");
	sLevel.append("........................#################.......................");
	sLevel.append("................................................................");
}

void GamePlayScreen::update(const float & deltaTime_)
{
	int nTileWidth = 16;
	int nTileHeight = 16;

	int nVisibleTilesX = m_window_ptr->getScreenWidth() / nTileWidth;
	int nVisibleTilesY = m_window_ptr->getScreenHeight() / nTileHeight;

	 

	m_layer.camera.update(m_window_ptr->getScreenWidth(), m_window_ptr->getScreenHeight());
	m_window_ptr->update();
}

void GamePlayScreen::draw()
{
}

void GamePlayScreen::onExit()
{
	m_sprite_batch.begin();
	m_sprite_batch.end();

	//sprite draws should be done here

	m_texture_program.use();
	preUpdateShader(&m_texture_program,"mySampler");
	uploadCameraInfo(&m_texture_program, &m_layer.camera, "P");
	m_sprite_batch.renderBatch();
	m_texture_program.unuse();
}

void GamePlayScreen::destroy()
{
	m_texture_program.dispose();
	m_sprite_batch.dispose();
}
