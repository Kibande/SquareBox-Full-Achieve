#include "MobileAppEditor_Screen.h"
#include "ScreenIndices.h"

MobileAppEditor_Screen::MobileAppEditor_Screen()
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
	m_camera.init(m_game_ptr->getWindow()->getScreenWidth(), m_game_ptr->getWindow()->getScreenHeight());
	m_camera.setScale(1.0f);
	m_camera.setPosition(glm::vec2(m_game_ptr->getWindow()->getScreenWidth() *0.5f, m_game_ptr->getWindow()->getScreenHeight() * 0.5f));//Center the camera
	
	mo.setNumColumns(3);
	mo.setTitleJustification(SquareBox::JustificationEnum::MIDDLE);
	SquareBox::GWOM::ClusterObject menu_object;
	auto texture = SquareBox::AssetManager::TextureManager::getTextureByName("white background.png", m_game_ptr->getWindow()->getDDPI());
	menu_object.texture_info.texture_id = texture.id;
	menu_object.texture_info.uv_rect = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	menu_object.texture_info.texture_type = SquareBox::TextureEnum::SINGLE;
	//__debugbreak();
	auto color = SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::orange);
	menu_object.texture_info.color = color.getIVec4();
	mo.setBackGroundObject(menu_object);

	color = SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::green);
	menu_object.texture_info.color = color.getIVec4();
	mo.setObject(menu_object);

	color = SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::blue);
	menu_object.texture_info.color = color.getIVec4();
	mo.setOnHoverObject(menu_object);

	color = SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::lemon_chiffon);
	menu_object.texture_info.color = color.getIVec4();
	mo.setOnClickObject(menu_object);

	auto cell_size = glm::vec2(100, 36);
	mo.setCellSize(cell_size);
	mo.setChildrenPadding(cell_size *0.2f);
	mo.setTitlePadding(cell_size *0.3f);
	mo.setTitleJustification(SquareBox::JustificationEnum::MIDDLE);
	mo.setTextToBoxHeightScale(0.5f);
	mo["Magic"]["Black"].setNumColumns(2);
	mo["Magic"]["Black"].setTitleText("Black Title");
	mo["Magic"]["Black"]["Fire"].setID(101).enable(false);
	mo["Magic"]["Black"]["Ice"].setID(101).enable(false);
	//mo["Boys"].setID(102).enable(true);
	//mo["Yes"].setID(102).enable(true);
	//mo["MUST"].setID(102).enable(true);

	mo["Thrid"].setID(102).enable(true);
	mo["Makerere University of Science and Tecnology"].setID(102).enable(true);
	mo["Magic"].showText(true);
	mo["Boys"].setCellJustification(SquareBox::JustificationEnum::LEFT);
	mo["Yes"].setCellJustification(SquareBox::JustificationEnum::RIGHT);
	mo["MUST"].setCellJustification(SquareBox::JustificationEnum::MIDDLE);
	mo.showTitle(true);
	mo.Build(m_spriteFont,m_camera.getScale());
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

	m_camera.update(m_game_ptr->getWindow()->getScreenWidth(), m_game_ptr->getWindow()->getScreenHeight());

	if (m_game_ptr->getInputDevice()->isInputIdReceived((int)(SquareBox::KeyBoardEnum::BACKSAPCE))) {
		
		if (menu_manager_vec.size() > 1) {
			menu_manager_vec.pop_back();
		}
	}
	mo.Build(m_spriteFont, m_camera.getScale());
}

void MobileAppEditor_Screen::draw()
{
	glm::vec2 mouse_in_world = m_camera.convertScreenToWorld(m_game_ptr->getInputDevice()->getScreenLocations()[0].coordinates);


	m_textureProgram.use();

	IGameScreen::preUpdateShader(&m_textureProgram, "mySampler");

	IGameScreen::uploadCameraInfo(&m_textureProgram, &m_camera, "P");
	m_spriteBatch.end(); mo.Build(m_spriteFont, m_camera.getScale());
	m_spriteBatch.begin(SquareBox::RenderEngine::GlyphSortType::NONE);

	//m_sprite_font or m_sprite_batch draw

	auto returnded=menu_manager_vec.back()->drawSelf(m_game_ptr, mouse_in_world, m_spriteBatch, m_spriteFont,m_debugRenderer,m_camera.getScale(), glm::vec2(m_game_ptr->getWindow()->getScreenWidth(), m_game_ptr->getWindow()->getScreenHeight())*0.5f);
	if (returnded != nullptr) {
		menu_manager_vec.push_back(returnded);
	}
	m_spriteBatch.end();
	m_spriteBatch.renderBatch();

	m_textureProgram.unuse();


	m_debugProgram.use();

	IGameScreen::uploadCameraInfo(&m_debugProgram, &m_camera, "P");
	m_debugRenderer.end(); mo.Build(m_spriteFont, m_camera.getScale());
	m_debugRenderer.begin();
	auto returnded_debug = menu_manager_vec.back()->drawSelf(m_game_ptr, mouse_in_world, m_spriteBatch, m_spriteFont, m_debugRenderer, m_camera.getScale(), glm::vec2(m_game_ptr->getWindow()->getScreenWidth(), m_game_ptr->getWindow()->getScreenHeight()) * 0.5f);
	if (returnded_debug != nullptr) {
		menu_manager_vec.push_back(returnded_debug);
	}
	m_debugRenderer.end();
	m_debugRenderer.render();
	m_debugProgram.unuse();



	
}