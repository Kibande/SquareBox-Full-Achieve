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
	world_width = m_game_ptr->getWindow()->getScreenWidth() * 0.1f;
	world_height = m_game_ptr->getWindow()->getScreenHeight() * 0.1f;

	m_utilities.init();
	m_sprite_batch.init();

	m_texture_program.init();
	m_texture_program.compileDefaultTextureShaders();
	m_texture_program.addDefaultTextureAttributes();
	m_texture_program.linkShaders();
	glm::vec2 screen_dimensions(m_game_ptr->getWindow()->getScreenWidth(), m_game_ptr->getWindow()->getScreenHeight());
	m_camera.init(screen_dimensions);

	m_camera.setPosition(screen_dimensions * 0.5f);
	m_camera.setScale(1.0f);

	snake = Snake(world_width*0.5f, world_height*0.5f);

	waves.push_front(Wave(world_width * 0.5f, world_height * 0.5f, 30, 1));
	waves.front().ampl = 400;

	m_audio_engine.init();

	music = SquareBox::AudioSystem::Music("Assets/Audio/ambience.ogg");

	m_audio_engine.loadMusic(music);
	music.play(-1);

	bip_sound_bank.sound_effects.push_back(SquareBox::AudioSystem::SoundEffect("bip", "Assets/Audio/bip.flac"));
	m_audio_engine.loadSoundBank(bip_sound_bank);

	boom_sound_bank.sound_effects.push_back(SquareBox::AudioSystem::SoundEffect("boom", "Assets/Audio/boom.ogg"));
	m_audio_engine.loadSoundBank(boom_sound_bank);

	bouns_sound_bank.sound_effects.push_back(SquareBox::AudioSystem::SoundEffect("bonus", "Assets/Audio/bonus.flac"));
	m_audio_engine.loadSoundBank(bouns_sound_bank);

	lose_sound_bank.sound_effects.push_back(SquareBox::AudioSystem::SoundEffect("lose", "Assets/Audio/lose.flac"));
	m_audio_engine.loadSoundBank(lose_sound_bank);

	srand(time(NULL));

	for (int i=0; i < world_width; ++i)
	{
		for (int j=0; j < world_height; ++j) { map.push_back(WavePoint(i, j)); }
	}

	Wave::surround = &map;

	score = 0;
	m_sprite_font.initWithName("font.ttf", 16);
	scoreText.setString(m_utilities.intToString(score));
	warning.setString("WARNING <!>");

	bonus = Bonus(rand() % (int)(world_width * 0.8), rand() % (int)(world_height * 0.8));

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

	delete [] data; // since the data is stored on the gpu
}

void GameScreen::update(const float& deltaTime_)
{
	m_camera.update(m_game_ptr->getWindow()->getScreenWidth(), m_game_ptr->getWindow()->getScreenHeight());

	tick++;
	m_time = timer.Elapsed();

	Move snakeMove = player_inputs.getMove(m_game_ptr);

	if (snakeMove == Move::LEFT)
	{
		if (choosingVx!=1)
		{
			choosingVx = -1;
			choosingVy = 0;
		}
	}
	else if (snakeMove == Move::RIGHT)
	{
		if (choosingVx!=-1)
		{
			choosingVx = 1;
			choosingVy = 0;
		}
	}
	else if (snakeMove == Move::UP)
	{
		if (choosingVy!=-1)
		{
			choosingVx = 0;
			choosingVy = 1;
		}
	}
	else if (snakeMove == Move::DOWN)
	{
		if (choosingVy!=1)
		{
			choosingVx = 0;
			choosingVy = -1;
		}
	}

	if (bonusClock.ElapsedMillis() >= bouns_shock_wave_rate)
	{
		waves.push_front(Wave(bonus.x, bonus.y, 10, 2));
		waves.front().ampl = 100;

		bip_sound_bank.play();

		bonusClock.Reset();
	}

	if (bombCount == 10 && !waiting && !danger)
	{
		waiting = true;
		dangerClock.Reset();
	}

	if (dangerClock.Elapsed() > 5 && !danger && waiting)
	{
		waiting = false;

		dangerCount++;
		bombCount += dangerCount;

		int simultaneous = dangerCount;
		if (simultaneous > 3) simultaneous = 3;

		for (int i(0); i < simultaneous; ++i)
		{
			
			bombs.push_back(Bomb(rand() % (int)(world_width * 0.8), rand() % (int)(world_height * 0.8)));
		}

		danger = true;
	}

	for (Bomb& bomb : bombs)
	{
		if (bomb.ready)
		{
			waves.push_back(Wave(bomb.x, bomb.y, 10, 10)); waves.back().ampl = 250;
			waves.push_back(Wave(bomb.x, bomb.y, 40, 1)); waves.back().ampl = 200;
			boom_sound_bank.play();
			bouns_sound_bank.play();
			if (!snake.isDead()) {
				const SnakeNode& head = snake.getHead();
				int x_head_boom = head.x - bomb.x;
				int y_head_boom = head.y - bomb.y;

				double distToSnakeHead = sqrt(x_head_boom * x_head_boom + y_head_boom * y_head_boom);

				if (distToSnakeHead < 15) {
					snake.addLife(-100 / (distToSnakeHead + 1));
				}
			}

			if (bombCount > 0)
			{
				bombs.push_back(Bomb(rand() % (int)(world_width * 0.8), rand() % (int)(world_height * 0.8)));
				bombCount--;
			}
		}
	}

	bombs.remove_if([](Bomb& b) { return b.ready; });

	if (bombs.size() == 0) {
		danger = false;
	}

	if (tick % 4 == 0 && !snake.isDead())
	{
		SnakeNode head = snake.getHead();



		snake.setDirection(choosingVx, choosingVy);

		if (head.x + choosingVx == bonus.x && head.y + choosingVy == bonus.y)
		{
			if (!danger)
			{
				bombCount++;
			}
			else
			{
				score += dangerCount - 1; // the longer the game has been running , the  higher your gain
			}

			bouns_sound_bank.play();
			waves.push_front(Wave(bonus.x, bonus.y, 10, 4));

			snake.addNode(bonus.x, bonus.y);
			snake.addLife(5);

			deadBonus.push_front(bonus);
			
			bonus = Bonus(rand() % (int)(world_width *0.8), rand() % (int)(world_height *0.8));
			newScore = true;
			score++;
		}
		else
		{
			snake.progress(world_width, world_height);

			if (!(tick % 6))
			{
				waves.push_front(Wave(head.x, head.y, 11, 1));
				waves.front().ampl = 20;
			}
		}
	}
	else if (snake.isDead())
	{
		if (!gameOver) {
			lose_sound_bank.play();
		}

		for (const SnakeNode& sn : snake.getNodeList())
		{
			waves.push_front(Wave(sn.x, sn.y, 25, 2));
			waves.front().ampl = 20;
		}

		snake.kill();

		music.setVolume(0.6f);
		gameOver = true;
	}

	snake.update();

	for (Bomb& bomb : bombs)
		bomb.update();

	float magic_8 = 8.0f;
	if (newScore)
	{

		if (heightScore - yScore < 0.1)
		{
			scoreText.setColor(SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::white));
			newScore = false;
			yScore = 0;
			scoreText.setString(m_utilities.intToString(score));
		}
		else
		{
			yScore += (heightScore - yScore) / magic_8;
			double alpha = 1 - (yScore / double(heightScore));
			scoreText.setColor(SquareBox::RenderEngine::ColorRGBA8(255, 255, 255, 255 * alpha));
		}
	}
	scoreText.setPosition(5, m_game_ptr->getWindow()->getScreenHeight() * 0.9f - yScore - magic_8);

	for (Bonus& db : deadBonus) { if (db.radius < 50) db.radius += 2.5; }
	deadBonus.remove_if([](Bonus& b) { return b.radius >= 50; });

	for (WavePoint& wp : map)
	{
		wp.color = 25;
		wp.delta = 0;
	}

	for (Wave& wave : waves) { wave.apply(); }

	waves.remove_if([](Wave& v) { return v.done; });
}

void GameScreen::draw()
{
	m_sprite_batch.begin(SquareBox::RenderEngine::GlyphSortType::NONE);

	for (int i = 0; i < map.size(); i++)
	{
		WavePoint& wp = map[i];
		SquareBox::RenderEngine::ColorRGBA8 col = SquareBox::RenderEngine::ColorRGBA8(wp.color, wp.color, wp.color, 255);
		double x = wp.x * 10.0 - wp.delta;
		double y = wp.y * 10.0 - wp.delta + heightScore;
		int g = 1;
		if (wp.delta > 4) g = wp.delta / 4.0;
		m_sprite_batch.draw(glm::vec4(glm::vec2(x, y), glm::vec2(g)), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), m_box_texture.id, 1.0f, col);
	}

	for (Bonus& db : deadBonus)
	{
		double alpha = 1 - (db.radius) / 50.0;
		SquareBox::RenderEngine::ColorRGBA8 col = SquareBox::RenderEngine::ColorRGBA8(255, 255, 255, 255 * alpha);

		glm::vec2 position = glm::vec2(10 * db.x + 5, 10 * db.y + 5 + heightScore);
		float radius = db.radius;

		glm::vec2 orign = (position - glm::vec2(radius) * 0.5f);
		m_sprite_batch.draw(glm::vec4(orign, glm::vec2(radius)), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), m_box_texture.id, 1.0f, col);

	}

	snake.draw(m_sprite_batch, heightScore,m_box_texture.id);

	// draw the current active bouns
	const glm::vec2 position = glm::vec2(10 * bonus.x + 5, 10 * bonus.y + 5 + heightScore);

	const  float radius = 5;
	SquareBox::RenderEngine::ColorRGBA8 new_col = SquareBox::RenderEngine::ColorRGBA8(255, 255, 100, 255);
	double bonusScale = 0.8 + 0.7 * pow(sin(3.14159 * m_time / 2), 2);
	float big_radius = radius * bonusScale;
	glm::vec2 big_orign = glm::vec2((position - glm::vec2(big_radius) * 0.5f));
	m_sprite_batch.draw(glm::vec4(big_orign, glm::vec2(big_radius)), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), m_box_texture.id, 1.0f, new_col);

	SquareBox::RenderEngine::ColorRGBA8 col = SquareBox::RenderEngine::ColorRGBA8(255, 255, 0, 255);
	glm::vec2 orign(position - glm::vec2(radius) * 0.5f);
	m_sprite_batch.draw(glm::vec4(orign, glm::vec2(radius)), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), m_box_texture.id, 1.0f, col);

	for (Bomb& bomb : bombs)
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


	float snake_life = snake.getLife();
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

	if (danger)
	{
		warning.setPosition(m_game_ptr->getWindow()->getScreenWidth() * 0.5f, m_game_ptr->getWindow()->getScreenHeight() * 0.9f);
		double c = 50 * sin(m_time * 20);
		warning.setColor(SquareBox::RenderEngine::ColorRGBA8(255, 100 + c, 100 + c, 255));
		m_sprite_font.draw(m_sprite_batch, warning.text, warning.position, glm::vec2(1.0f), 1.0f, SquareBox::RenderEngine::ColorRGBA8(warning.color), SquareBox::JustificationEnum::MIDDLE);
	
	}
	else if (waiting)
	{
		warning.setPosition(m_game_ptr->getWindow()->getScreenWidth() * 0.5f, m_game_ptr->getWindow()->getScreenHeight() * 0.9f);
		double c = 50 * sin(m_time * 5);
		warning.setColor(SquareBox::RenderEngine::ColorRGBA8(50 + c, 10, 10, 255));
		m_sprite_font.draw(m_sprite_batch, warning.text, warning.position, glm::vec2(1.0f), 1.0f, SquareBox::RenderEngine::ColorRGBA8(warning.color));
	}

	m_sprite_font.draw(m_sprite_batch, scoreText.text, scoreText.position, glm::vec2(1.0f), 1.0f, SquareBox::RenderEngine::ColorRGBA8(scoreText.color));
	scoreTransition.setString(m_utilities.intToString(m_game_ptr->getFps()));
	m_sprite_font.draw(m_sprite_batch, scoreTransition.text, glm::vec2(scoreText.position.x+ m_game_ptr->getWindow()->getScreenWidth() *0.1f, scoreText.position.y), glm::vec2(1.0f), 1.0f, SquareBox::RenderEngine::ColorRGBA8(scoreText.color));

	m_sprite_batch.end();

	m_texture_program.use();
	IGameScreen::preUpdateShader(&m_texture_program, "mySampler");
	IGameScreen::uploadCameraInfo(&m_texture_program, &m_camera, "P");
	m_sprite_batch.renderBatch();
	m_texture_program.unuse();
}

void GameScreen::onExit()
{
}

void GameScreen::destroy()
{
	m_sprite_batch.dispose();
	m_texture_program.dispose();
	m_utilities.dispose();
	m_sprite_font.dispose();
}
