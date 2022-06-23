#include "MobileAppEditor_Screen.h"
#include "ScreenIndices.h"

MobileAppEditor_Screen::MobileAppEditor_Screen(SquareBox::RenderEngine::Window* window) :m_window(window)
{
}

MobileAppEditor_Screen::~MobileAppEditor_Screen()
{
}

int MobileAppEditor_Screen::getNextScreenIndex() const
{
	return SCREEN_INDEX_NO_SCREEN;
}

int MobileAppEditor_Screen::getPreviousScreenIndex() const
{
	return WELCOME_SCREEN_INDEX;
}

void MobileAppEditor_Screen::build()
{
}

void MobileAppEditor_Screen::destroy()
{
}

void MobileAppEditor_Screen::onEntry()
{
	m_spriteBatch.init();
	m_spriteFont.init("Assets/Fonts/Comfortaa-Bold.ttf", 32);
	m_debugRenderer.init();

	//Init Shaders
	m_textureProgram.compileShaders("Assets/Shaders/colorShading.vert", "Assets/Shaders/colorShading.frag");
	m_textureProgram.addAttribute("vertexPosition");
	m_textureProgram.addAttribute("vertexColor");
	m_textureProgram.addAttribute("vertexUV");
	m_textureProgram.linkShaders();


	//Init Shaders
	m_debugProgram.compileShaders("Assets/Shaders/debugShading.vert", "Assets/Shaders/debugShading.frag");
	m_debugProgram.addAttribute("vertexPosition");
	m_debugProgram.addAttribute("vertexColor");
	m_debugProgram.linkShaders();

	//Init Cameras
	m_camera.init(m_window->getScreenWidth(), m_window->getScreenHeight());
	m_camera.setScale(1.0f);
	m_camera.setPosition(glm::vec2(m_window->getScreenWidth() / 2, m_window->getScreenHeight() / 2));//Center the camera
	
	mo.setTableArrangement(2, 3);
	mo.setTitleJustification(SquareBox::JustificationEnum::MIDDLE);
	mo.setSpriteFont(&m_spriteFont);
	SquareBox::GWOM::ClusterObject menu_object;
	auto texture = SquareBox::AssetManager::TextureManager::getTextureByName("white background.png", m_window->getDDPI());
	menu_object.texture_info.texture_id = texture.id;
	menu_object.texture_info.uv_rect = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	menu_object.texture_info.texture_type = SquareBox::TextureEnum::SINGLE;
	//__debugbreak();
	auto color = SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::orange);
	menu_object.color = color.getIVec4();
	mo.setBackGroundObject(menu_object);

	color = SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::green);
	menu_object.color = color.getIVec4();
	mo.setObject(menu_object);

	color = SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::blue);
	menu_object.color = color.getIVec4();
	mo.setOnHoverObject(menu_object);

	color = SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::lemon_chiffon);
	menu_object.color = color.getIVec4();
	mo.setOnClickObject(menu_object);

	mo.setTextScaling(glm::vec2(1.0f));
	auto cell_size = glm::vec2(222, 36);
	mo.setCellSize(cell_size);
	mo.setChildrenPadding(cell_size *0.2f);
	mo["Magic"]["Black"].setTableArrangement(1, 2);
	mo["Magic"]["Black"].setTableArrangement(1, 2);
	mo["Magic"]["Black"]["Fire"].setID(101).enable(false).setTableArrangement(2, 4);
	mo["Magic"]["Black"]["Ice"].setID(101).enable(false).setTableArrangement(2, 4);
	mo["Boys"].setID(102).enable(true).setTableArrangement(2,3);
	mo["Thrid"].setID(102).enable(true).setTableArrangement(2,3);
	mo["Yes"].setID(102).enable(true).setTableArrangement(2,3);
	mo["MUST"].setID(102).enable(true).setTableArrangement(2,3);
	mo["MUK"].setID(102).enable(true).setTableArrangement(2,3);

	mo["Boys"].setLeftRightJustificationPadding(3.1f);
	mo["Magic"].showText(true);

	mo["Yes"].setLeftRightJustificationPadding(3.1f);
	mo["Boys"].setCellJustification(SquareBox::JustificationEnum::LEFT);
	mo["Yes"].setCellJustification(SquareBox::JustificationEnum::RIGHT);
	mo["MUST"].setCellJustification(SquareBox::JustificationEnum::MIDDLE);





	mo.Build();
	menu_manager_vec.push_back(&mo);
	m_game_ptr->setProcessingInput(true);

}

void MobileAppEditor_Screen::onExit()
{
	m_spriteBatch.dispose();
	m_debugRenderer.dispose();
	m_spriteFont.dispose();
	m_textureProgram.dispose();
	m_debugProgram.dispose();
}

void MobileAppEditor_Screen::update(const float & deltaTime_)
{
	m_window->update();
	m_camera.update(m_window->getScreenWidth(), m_window->getScreenHeight());

	if (m_game_ptr->getInputDevice()->isInputIdReceived((int)(SquareBox::KeyBoardEnum::BACKSAPCE))) {
		
		if (menu_manager_vec.size() > 1) {
			menu_manager_vec.pop_back();
		}
	}
}

void MobileAppEditor_Screen::draw()
{
	glm::vec2 mouse_in_world = m_camera.convertScreenToWorld(m_game_ptr->getInputDevice()->getScreenLocations()[0].coordinates);


	m_textureProgram.use();

	IGameScreen::preUpdateShader(&m_textureProgram, "mySampler");

	IGameScreen::uploadCameraInfo(&m_textureProgram, &m_camera, "P");

	m_spriteBatch.begin(SquareBox::RenderEngine::GlyphSortType::NONE);

	//m_sprite_font or m_sprite_batch draw

	auto returnded=menu_manager_vec.back()->drawSelf(m_game_ptr, mouse_in_world, &m_spriteBatch, nullptr, glm::vec2(m_window->getScreenWidth() * 0.5, m_window->getScreenHeight() * 0.5));
	if (returnded != nullptr) {
		menu_manager_vec.push_back(returnded);
	}
	m_spriteBatch.end();
	m_spriteBatch.renderBatch();

	m_textureProgram.unuse();


	m_debugProgram.use();

	IGameScreen::uploadCameraInfo(&m_debugProgram, &m_camera, "P");

	m_debugRenderer.begin();
	 menu_manager_vec.back()->drawSelf(m_game_ptr, mouse_in_world, nullptr, &m_debugRenderer, glm::vec2(m_window->getScreenWidth()*0.5,m_window->getScreenHeight()*0.5));
	
	m_debugRenderer.end();
	m_debugRenderer.render();
	m_debugProgram.unuse();



	
}