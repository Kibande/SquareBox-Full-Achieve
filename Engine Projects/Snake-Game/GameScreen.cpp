#include "GameScreen.h"
std::vector<WavePoint>* Wave::surround; //  used as a static member is wave so do not touch 

int GameScreen::getNextScreenIndex() const
{
	return m_nextScreenIndex;
}

int GameScreen::getPreviousScreenIndex() const
{
	return m_previousScreenIndex;
}

void GameScreen::build()
{
	SBX_INFO("Screen width is {}", m_game_ptr->getWindow()->getScreenWidth());
}

void GameScreen::onEntry()
{

	m_sprite_batch.init();

	m_texture_program.init();
	m_texture_program.compileDefaultTextureShaders();
	m_texture_program.addDefaultTextureAttributes();
	m_texture_program.linkShaders();
	glm::vec2 screen_dimensions(m_game_ptr->getWindow()->getScreenWidth(), m_game_ptr->getWindow()->getScreenHeight());
	m_camera.init(screen_dimensions);

	m_camera.setPosition(screen_dimensions * 0.5f);
	m_camera.setScale(1.0f);

	m_sprite_font.initWithName("font.ttf", 16);
	warning.setString("WARNING <!>");

	// set up the size of the world
	int world_width = m_game_ptr->getWindow()->getScreenWidth() * 0.1f;
	int world_height = m_game_ptr->getWindow()->getScreenHeight() * 0.1f;

	m_game_logic.init(m_game_ptr,0,0 + world_height*.32f, world_width, world_height *0.5f);

	//create some game textures on the fly
	const int width = 1000;
	const int height = 1000;
	const int length = (width * height) * 4;

	float* data = new float[length];
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

	auto  fileBuffer = std::pair<float*, int>(data, length);
	m_box_texture = SquareBox::AssetManager::TextureManager::createTextureFromFileBuffer(fileBuffer, width, height);

	delete[] data; // since the data is stored on the gpu

	//game stats gui
	m_snake_game_home_gui_elements_coordinates[m_central_button] = -1;
	m_snake_game_home_gui_elements_coordinates[m_right_arrow] = -1;
	m_snake_game_home_gui_elements_coordinates[m_left_arrow] = -1;
	m_snake_game_home_gui_elements_coordinates[m_up_arrow] = -1;
	m_snake_game_home_gui_elements_coordinates[m_down_arrow] = -1;

	m_snake_game_home_gui_elements_coordinates[m_menu_button] = -1;
	m_snake_game_home_gui_elements_coordinates[m_pause_play] = -1;
	m_snake_game_home_gui_elements_coordinates[m_shoot_button] = -1;
	m_snake_game_home_gui_elements_coordinates[m_score_board] = -1;

	m_snake_game_home_gui_elements_coordinates[score] = -1;
	m_snake_game_home_gui_elements_coordinates[life_bar_border] = -1;
	m_snake_game_home_gui_elements_coordinates[life_bar_body] = -1;

	m_snake_game_home_gui.init(m_game_ptr, m_snake_game_home_gui_elements_coordinates, "Assets/Guis/snake_game_home_gui.gui");


}

void GameScreen::update(const float& deltaTime_)
{
	m_time = timer.Elapsed();
	m_camera.update(m_game_ptr->getWindow()->getScreenWidth(), m_game_ptr->getWindow()->getScreenHeight());

	m_snake_game_home_gui.update(m_camera,1.0f);

	//GUI
	SnakeGame::Move registred_snake_movement = SnakeGame::Move::NONE;

	if (show_mobile_controls) {
		//controls independent updates

		SquareBox::GWOM::GUIElement& gui_central_button = m_snake_game_home_gui.getGuiElementByIndex(m_snake_game_home_gui_elements_coordinates.find(m_central_button)->second);
		SquareBox::GWOM::GUIElement& gui_right_arrow = m_snake_game_home_gui.getGuiElementByIndex(m_snake_game_home_gui_elements_coordinates.find(m_right_arrow)->second);
		SquareBox::GWOM::GUIElement& gui_left_arrow = m_snake_game_home_gui.getGuiElementByIndex(m_snake_game_home_gui_elements_coordinates.find(m_left_arrow)->second);
		SquareBox::GWOM::GUIElement& gui_up_arrow = m_snake_game_home_gui.getGuiElementByIndex(m_snake_game_home_gui_elements_coordinates.find(m_up_arrow)->second);
		SquareBox::GWOM::GUIElement& gui_down_arrow = m_snake_game_home_gui.getGuiElementByIndex(m_snake_game_home_gui_elements_coordinates.find(m_down_arrow)->second);

		SquareBox::GWOM::GUIElement& gui_menu_button = m_snake_game_home_gui.getGuiElementByIndex(m_snake_game_home_gui_elements_coordinates.find(m_menu_button)->second);
		SquareBox::GWOM::GUIElement& gui_pause_play = m_snake_game_home_gui.getGuiElementByIndex(m_snake_game_home_gui_elements_coordinates.find(m_pause_play)->second);
		SquareBox::GWOM::GUIElement& gui_shoot_button = m_snake_game_home_gui.getGuiElementByIndex(m_snake_game_home_gui_elements_coordinates.find(m_shoot_button)->second);
		SquareBox::GWOM::GUIElement& gui_score_board = m_snake_game_home_gui.getGuiElementByIndex(m_snake_game_home_gui_elements_coordinates.find(m_score_board)->second);
		SquareBox::GWOM::GUIElement& gui_score = m_snake_game_home_gui.getGuiElementByIndex(m_snake_game_home_gui_elements_coordinates.find(score)->second);
		SquareBox::GWOM::GUIElement& gui_life_bar_border = m_snake_game_home_gui.getGuiElementByIndex(m_snake_game_home_gui_elements_coordinates.find(life_bar_border)->second);
		SquareBox::GWOM::GUIElement& gui_life_bar_body = m_snake_game_home_gui.getGuiElementByIndex(m_snake_game_home_gui_elements_coordinates.find(life_bar_body)->second);


		if (gui_right_arrow.state == SquareBox::GUIElementStateEnum::ACTIVE) {
			registred_snake_movement = SnakeGame::Move::RIGHT;
		}
		else if (gui_left_arrow.state == SquareBox::GUIElementStateEnum::ACTIVE) {
			registred_snake_movement = SnakeGame::Move::LEFT;
		}
		else if (gui_up_arrow.state == SquareBox::GUIElementStateEnum::ACTIVE) {
			registred_snake_movement = SnakeGame::Move::UP;
		}
		else if (gui_down_arrow.state == SquareBox::GUIElementStateEnum::ACTIVE) {
			registred_snake_movement = SnakeGame::Move::DOWN;
		}
	}
	else {
		if (m_game_ptr->getInputDevice()->isInputIdReceived(static_cast<int>(SquareBox::KeyBoardEnum::KEY_a)))
		{
			registred_snake_movement = SnakeGame::Move::LEFT;
		}
		else if (m_game_ptr->getInputDevice()->isInputIdReceived(static_cast<int>(SquareBox::KeyBoardEnum::KEY_d)))
		{
			registred_snake_movement = SnakeGame::Move::RIGHT;
		}
		else if (m_game_ptr->getInputDevice()->isInputIdReceived(static_cast<int>(SquareBox::KeyBoardEnum::KEY_w)))
		{
			registred_snake_movement = SnakeGame::Move::UP;
		}
		else if (m_game_ptr->getInputDevice()->isInputIdReceived(static_cast<int>(SquareBox::KeyBoardEnum::KEY_s)))
		{
			registred_snake_movement= SnakeGame::Move::DOWN;
		}

	}

	m_game_logic.update(deltaTime_, false, registred_snake_movement);


}

void GameScreen::draw()
{
	m_sprite_batch.begin(SquareBox::RenderEngine::GlyphSortType::NONE);

	for (int i = 0; i < m_game_logic.getWorldMap().size(); i++)
	{
		const WavePoint& wp = m_game_logic.getWorldMap()[i];
		SquareBox::RenderEngine::ColorRGBA8 col = SquareBox::RenderEngine::ColorRGBA8(wp.color, wp.color, wp.color, 255);
		double x = wp.x * 10.0 - wp.delta;
		double y = wp.y * 10.0 - wp.delta + heightScore;
		int g = 1;
		if (wp.delta > 4) g = wp.delta / 4.0;
		m_sprite_batch.draw(glm::vec4(glm::vec2(x, y), glm::vec2(g)), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), m_box_texture.id, 1.0f, col);
	}

	for (const Bonus& db : m_game_logic.getWorldDeadBonus())
	{
		double alpha = 1 - (db.radius) / 50.0;
		SquareBox::RenderEngine::ColorRGBA8 col = SquareBox::RenderEngine::ColorRGBA8(255, 255, 255, 255 * alpha);

		glm::vec2 position = glm::vec2(10 * db.x + 5, 10 * db.y + 5 + heightScore);
		float radius = db.radius;

		glm::vec2 orign = (position - glm::vec2(radius) * 0.5f);
		m_sprite_batch.draw(glm::vec4(orign, glm::vec2(radius)), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), m_box_texture.id, 1.0f, col);

	}
	
	m_game_logic.getSnake().draw(m_sprite_batch, heightScore, m_box_texture.id);

	// draw the current active bouns
	const glm::vec2 position = glm::vec2(10 * m_game_logic.getActiveBouns().x + 5, 10 * m_game_logic.getActiveBouns().y + 5 + heightScore);

	const  float radius = 5;
	SquareBox::RenderEngine::ColorRGBA8 new_col = SquareBox::RenderEngine::ColorRGBA8(255, 255, 100, 255);
	double bonusScale = 0.8 + 0.7 * pow(sin(3.14159 * m_time / 2), 2);
	float big_radius = radius * bonusScale;
	glm::vec2 big_orign = glm::vec2((position - glm::vec2(big_radius) * 0.5f));
	m_sprite_batch.draw(glm::vec4(big_orign, glm::vec2(big_radius)), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), m_box_texture.id, 1.0f, new_col);

	SquareBox::RenderEngine::ColorRGBA8 col = SquareBox::RenderEngine::ColorRGBA8(255, 255, 0, 255);
	glm::vec2 orign(position - glm::vec2(radius) * 0.5f);
	m_sprite_batch.draw(glm::vec4(orign, glm::vec2(radius)), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), m_box_texture.id, 1.0f, col);

	for (const Bomb& bomb : m_game_logic.getWorldBooms())
	{
		SquareBox::RenderEngine::ColorRGBA8 boom_color = SquareBox::RenderEngine::ColorRGBA8(100, 0, 0, 255);
		glm::vec2 boom_position(bomb.x * 10 + 5, bomb.y * 10 + 5 + heightScore);
		float size = 10 * 1.2f;
		glm::vec2 boom_orign = (boom_position - glm::vec2(size) * 0.5f);
		m_sprite_batch.draw(glm::vec4(boom_orign, glm::vec2(size)), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), m_box_texture.id, 1.0f, boom_color);

		SquareBox::RenderEngine::ColorRGBA8 new_boom_color = SquareBox::RenderEngine::ColorRGBA8(150, 0, 0, 255);

		double bombScale = 1 + 0.8 * pow(sin(3.14159 * m_time * 2.0), 2.0);
		size = size * bombScale;
		boom_orign = (boom_position - glm::vec2(size) * 0.5f);
		m_sprite_batch.draw(glm::vec4(boom_orign, glm::vec2(size)), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), m_box_texture.id, 1.0f, new_boom_color);

		SquareBox::RenderEngine::ColorRGBA8 new_new_boom_color = SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::white);
		size = size * (bombScale / 4.0);
		boom_orign = (boom_position - glm::vec2(size) * 0.5f);
		m_sprite_batch.draw(glm::vec4(boom_orign, glm::vec2(size)), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), m_box_texture.id, 1.0f, new_new_boom_color);
	}

	float life_bar_height = 10;
	float life_bar_width = m_game_ptr->getWindow()->getScreenWidth() * 0.2f;
	glm::vec2 life_bar_dimensions(life_bar_width, life_bar_height);
	glm::vec2 life_bar_position(m_game_ptr->getWindow()->getScreenWidth() - life_bar_width - 10, m_game_ptr->getWindow()->getScreenHeight() - 16);
	SquareBox::RenderEngine::ColorRGBA8 life_bar_color(SquareBox::Color::grey);
	glm::vec2 life_bar_orign = (life_bar_position - life_bar_dimensions * 0.5f);
	m_sprite_batch.draw(glm::vec4(life_bar_orign, life_bar_dimensions), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), m_box_texture.id, 1.0f, life_bar_color);


	float snake_life = m_game_logic.getSnake().getLife();
	float energy_bar_width = life_bar_width * (snake_life / 100.0f);
	glm::vec2 energy_level_dimensions(energy_bar_width, life_bar_height);
	glm::vec2 energy_level_position(m_game_ptr->getWindow()->getScreenWidth() - energy_bar_width - 10, m_game_ptr->getWindow()->getScreenHeight() - 16);
	SquareBox::RenderEngine::ColorRGBA8 energy_level_color(255 - 255 * (snake_life / 100.0), 255 * (snake_life / 100.0), 0, 255);
	glm::vec2 energy_level_bar_orign = (energy_level_position - energy_level_dimensions * 0.5f);
	m_sprite_batch.draw(glm::vec4(energy_level_bar_orign, energy_level_dimensions), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), m_box_texture.id, 1.0f, energy_level_color);


	glm::vec2 life_bar_frame_orign;
	glm::vec2 life_bar_frame_dimensions(life_bar_dimensions.x * 0.01f, life_bar_dimensions.y);
	SquareBox::RenderEngine::ColorRGBA8 life_bar_frame_color(SquareBox::Color::grey);

	life_bar_frame_orign = life_bar_orign;
	life_bar_frame_orign.x = life_bar_orign.x + life_bar_dimensions.x * 0.2f;
	m_sprite_batch.draw(glm::vec4(life_bar_frame_orign, life_bar_frame_dimensions), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), m_box_texture.id, 1.0f, life_bar_frame_color);

	life_bar_frame_orign.x = life_bar_orign.x + life_bar_dimensions.x * 0.4f;
	m_sprite_batch.draw(glm::vec4(life_bar_frame_orign, life_bar_frame_dimensions), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), m_box_texture.id, 1.0f, life_bar_frame_color);


	life_bar_frame_orign.x = life_bar_orign.x + life_bar_dimensions.x * 0.6f;
	m_sprite_batch.draw(glm::vec4(life_bar_frame_orign, life_bar_frame_dimensions), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), m_box_texture.id, 1.0f, life_bar_frame_color);


	life_bar_frame_orign.x = life_bar_orign.x + life_bar_dimensions.x * 0.8f;
	m_sprite_batch.draw(glm::vec4(life_bar_frame_orign, life_bar_frame_dimensions), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), m_box_texture.id, 1.0f, life_bar_frame_color);

	if (m_game_logic.isDanger())
	{
		warning.setPosition(m_game_ptr->getWindow()->getScreenWidth() * 0.5f, m_game_ptr->getWindow()->getScreenHeight() * 0.9f);
		double c = 50 * sin(m_time * 20);
		warning.setColor(SquareBox::RenderEngine::ColorRGBA8(255, 100 + c, 100 + c, 255));
		m_sprite_font.draw(m_sprite_batch, warning.text, warning.position, glm::vec2(1.0f), 1.0f, SquareBox::RenderEngine::ColorRGBA8(warning.color), SquareBox::JustificationEnum::MIDDLE);
	
	}
	else if (m_game_logic.isWaiting())
	{
		warning.setPosition(m_game_ptr->getWindow()->getScreenWidth() * 0.5f, m_game_ptr->getWindow()->getScreenHeight() * 0.9f);
		double c = 50 * sin(m_time * 5);
		warning.setColor(SquareBox::RenderEngine::ColorRGBA8(50 + c, 10, 10, 255));
		m_sprite_font.draw(m_sprite_batch, warning.text, warning.position, glm::vec2(1.0f), 1.0f, SquareBox::RenderEngine::ColorRGBA8(warning.color));
	}

	m_sprite_batch.end();

	m_texture_program.use();
	IGameScreen::preUpdateShader(&m_texture_program, "mySampler");
	IGameScreen::uploadCameraInfo(&m_texture_program, &m_camera, "P");
	m_sprite_batch.renderBatch();
	m_texture_program.unuse();


	//drawing Gui
	if (show_mobile_controls) {
		m_sprite_batch.begin(SquareBox::RenderEngine::GlyphSortType::NONE);
		m_snake_game_home_gui.draw(m_camera, m_sprite_batch);
		m_sprite_batch.end();
		m_texture_program.use();
		preUpdateShader(&m_texture_program, "mySampler");
		uploadCameraInfo(&m_texture_program, &m_camera, "P");
		m_sprite_batch.renderBatch();
		m_texture_program.unuse();
	}

}

void GameScreen::onExit()
{
}

void GameScreen::destroy()
{
	m_game_logic.dispose();
	m_sprite_batch.dispose();
	m_texture_program.dispose();
	m_sprite_font.dispose();
}

