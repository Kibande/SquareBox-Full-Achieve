#include "GameScreen.h"
std::vector<WavePoint>* Wave::surround;

//META PROGRAMMING   

template<typename T>
T print(T value) {
    std::cout << value << std:: endl;
    return value;
}

template<typename T, int N>
class Array {
private:
    T m_Array[N];
public:
    int GetSize(){ const return N };
};

int GameScreen::getNextScreenIndex() const
{
    print<std::string>("");
    Array<int, 20> arrays; 
    return m_nextScreenIndex;
}

int GameScreen::getPreviousScreenIndex() const
{
    return m_previousScreenIndex;
}

void GameScreen::build()
{

}

void GameScreen::onEntry()
{
    m_utilities.init();
    m_sprite_batch.init();

    m_texture_program.init();
    m_texture_program.compileDefaultTextureShaders();
    m_texture_program.addDefaultTextureAttributes();
    m_texture_program.linkShaders();
    glm::vec2 screen_dimensions = glm::vec2(m_game_ptr->getWindow()->getScreenWidth(), m_game_ptr->getWindow()->getScreenHeight());
    m_camera.init(screen_dimensions);

    m_camera.setPosition(screen_dimensions * 0.5f);
    m_camera.setScale(1.0f);

    snake = Snake(25, 23);

    life = snake.getLife();

    waves.push_front(Wave(25, 23, 30, 1));
    waves.front().ampl = 400;

    int m_input_audio_format_flags = static_cast<int>(SquareBox::AudioInputFormatEnum::MP3_FORMAT);
    m_input_audio_format_flags |= static_cast<int>(SquareBox::AudioInputFormatEnum::OGG_FORMAT);
    m_input_audio_format_flags |= static_cast<int>(SquareBox::AudioInputFormatEnum::MOD_FORMAT);
    m_input_audio_format_flags |= static_cast<int>(SquareBox::AudioInputFormatEnum::MID_FORMAT);
    m_input_audio_format_flags |= static_cast<int>(SquareBox::AudioInputFormatEnum::FLAC_FORMAT);
    m_input_audio_format_flags |= static_cast<int>(SquareBox::AudioInputFormatEnum::OPUS_FORMAT);
    m_audio_engine.init(m_input_audio_format_flags, MIX_DEFAULT_FREQUENCY, SquareBox::AudioOutputFormatEnum::S16_AUDIO_OUTPUT, SquareBox::AudioChannlesEnum::STEREO, 4096);

    SquareBox::AudioSystem::Music music = SquareBox::AudioSystem::Music("Assets/Audio/ambience.ogg");
    
    m_audio_engine.loadMusic(music);
    music.play(-1);

    bip_sound_bank.sound_effects.push_back(SquareBox::AudioSystem::SoundEffect("bip","Assets/Audio/bip.flac"));
    m_audio_engine.loadSoundBank(bip_sound_bank);


    boom_sound_bank.sound_effects.push_back(SquareBox::AudioSystem::SoundEffect("boom","Assets/Audio/boom.ogg"));
    m_audio_engine.loadSoundBank(boom_sound_bank);


    bouns_sound_bank.sound_effects.push_back(SquareBox::AudioSystem::SoundEffect("bonus","Assets/Audio/bonus.flac"));
    m_audio_engine.loadSoundBank(bouns_sound_bank);


    lose_sound_bank.sound_effects.push_back(SquareBox::AudioSystem::SoundEffect("lose","Assets/Audio/lose.flac"));
    m_audio_engine.loadSoundBank(lose_sound_bank);

    srand(time(NULL));

    for (int i(0); i < 50; ++i)
    {
        for (int j(0); j < 46; ++j) { map.push_back(WavePoint(i, j)); }
    }

    Wave::surround = &map;

    m_sprite_font.initWithName("font.ttf", 16);
    scoreText.setString(m_utilities.intToString(score));
    warning.setString("WARNING <!>");


    if (argc > 1)
        eventProvider = new ReplayInputs("replay");
    else
        eventProvider = new PlayerInputs();

    if (eventProvider->getBonusLocation(bonus.x, bonus.y))
    {
        bonus = Bonus(rand() % 50, rand() % 45);
    }

    replay.push_back(GameAction(tick, 1, bonus.x, bonus.y, 0, 0, 0));


}

void GameScreen::update(const float& deltaTime_)
{
    m_game_ptr->getWindow()->update();
    m_camera.update(m_game_ptr->getWindow()->getScreenWidth(), m_game_ptr->getWindow()->getScreenHeight());

    tick++;
    m_time = timer.Elapsed();

    glm::vec2& localPosition = m_camera.convertScreenToWorld(m_game_ptr->getInputDevice()->getScreenLocations()[0].coordinates);

    Move snakeMove = eventProvider->getMove(m_game_ptr);

    if (snakeMove == Move::LEFT)
    {
        if (!vx)
        {
            choosingVx = -1;
            choosingVy = 0;
        }
    }
    else if (snakeMove == Move::RIGHT)
    {
        if (!vx)
        {
            choosingVx = 1;
            choosingVy = 0;
        }
    }
    else if (snakeMove == Move::UP)
    {
        if (!vy)
        {
            choosingVx = 0;
            choosingVy = 1;
        }
    }
    else if (snakeMove == Move::DOWN)
    {
        if (!vy)
        {
            choosingVx = 0;
            choosingVy = -1;
        }
    }

    if (bonusClock.ElapsedMillis() >= 2000)
    {
        waves.push_front(Wave(bonus.x, bonus.y, 5, 1));
        waves.front().ampl = 50;

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
            Bomb b(0, 0);
            if (eventProvider->getBombLocation(b.x, b.y, b.delay))
            {
                b = Bomb(rand() % 50, rand() % 45);
            }
            bombs.push_back(b);
            replay.push_back(GameAction(tick, 2, b.x, b.y, 0, 0, b.delay));
        }

        danger = true;
    }

    for (Bomb& bomb : bombs)
    {
        if (bomb.ready)
        {
            //bomb.reset();
            waves.push_back(Wave(bomb.x, bomb.y, 10, 10)); waves.back().ampl = 250;
            waves.push_back(Wave(bomb.x, bomb.y, 40, 1)); waves.back().ampl = 200;
            boom_sound_bank.play();
            bouns_sound_bank.play();
            //if (snake.isDead()) return;
            const SnakeNode& head = snake.getHead();
            double vx = head.x - bomb.x;
            double vy = head.y - bomb.y;

            double distToSnake = sqrt(vx * vx + vy * vy);

            if (distToSnake < 15)
                snake.addLife(-100 / (distToSnake + 1));

 

            if (bombCount > 0)
            {
                Bomb b(0, 0);
                if (eventProvider->getBombLocation(b.x, b.y, b.delay))
                {
                    b = Bomb(rand() % 50, rand() % 45);
                }
                bombs.push_back(b);
                replay.push_back(GameAction(tick, 2, b.x, b.y, 0, 0, b.delay));
                bombCount--;
            }
        }
    }

    bombs.remove_if([](Bomb& b) { return b.ready; });

    if (bombs.size() == 0)
        danger = false;

    if (tick % 4 == 0 && !snake.isDead())
    {
        SnakeNode head = snake.getHead();

        if (vx != choosingVx || vy != choosingVy)
            replay.push_back(GameAction(tick, 0, 0, 0, choosingVx, choosingVy, 0));

        vx = choosingVx;
        vy = choosingVy;

        snake.setDirection(vx, vy);

        if (head.x + vx == bonus.x && head.y + vy == bonus.y)
        {
            if (!danger)
            {
                bombCount++;
            }
            else
            {
                score += dangerCount - 1;
            }

          

            bouns_sound_bank.play();

            waves.push_front(Wave(bonus.x, bonus.y, 10, 4));

            snake.addNode(bonus.x, bonus.y);
            snake.addLife(5);

            deadBonus.push_front(bonus);
            if (eventProvider->getBonusLocation(bonus.x, bonus.y))
            {
                bonus = Bonus(rand() % 48 + 1, rand() % 43 + 1);
            }

            replay.push_back(GameAction(tick, 1, bonus.x, bonus.y, 0, 0, 0));

            newScore = true;
            score++;

            scoreTransition.setString(m_utilities.intToString(score));
        }
        else
        {
            snake.progress();

            if (!(tick % 6))
            {
                waves.push_front(Wave(head.x, head.y, 11, 1));
                waves.front().ampl = 20;
            }
        }
    }
    else if (snake.isDead())
    {
        if (!gameOver)
            lose_sound_bank.play();

        for (const SnakeNode& sn : snake.getNodeList())
        {
            waves.push_front(Wave(sn.x, sn.y, 25, 2));
            waves.front().ampl = 20;
        }

        snake.kill();
        //ambience.setVolume(ambience.getVolume()*0.99);
        gameOver = true;
    }

    snake.update();

    for (Bomb& bomb : bombs)
        bomb.update();

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
            yScore += (heightScore - yScore) / 8.0;
            double alpha = 1 - (yScore / double(heightScore));
            scoreText.setColor(SquareBox::RenderEngine::ColorRGBA8(255, 255, 255, 255 * alpha));
            scoreTransition.setColor(SquareBox::RenderEngine::ColorRGBA8(255, 255, 255, 255 * (1 - alpha)));
        }
    }
    scoreText.setPosition(5, m_game_ptr->getWindow()->getScreenHeight()*0.9f - yScore - 8);
    scoreTransition.setPosition(5, m_game_ptr->getWindow()->getScreenHeight() *0.9f - yScore - heightScore - 8);

    // disparition des bonus morts
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
    int n_points = map.size();
    for (int i = 0; i < n_points; i++)
    {
        WavePoint& wp = map[i];
        SquareBox::RenderEngine::ColorRGBA8 col = SquareBox::RenderEngine::ColorRGBA8(wp.color, wp.color, wp.color,255);
        double x = wp.x * 10 - wp.delta;
        double y = wp.y * 10 - wp.delta + heightScore;
        int g = 1;
        if (wp.delta > 4) g = wp.delta / 4.0;

        SquareBox::AssetManager::GLTexture m_texture = SquareBox::AssetManager::TextureManager::getTextureByFilePath("Assets/Textures/nodpi/white background.png");
        m_sprite_batch.draw(glm::vec4(glm::vec2(x, y), glm::vec2(g)), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), m_texture.id, 1.0f, col);

    }

    for (Bonus& db : deadBonus)
    {
    
         double alpha = 1 - (db.radius) / 50.0;
         SquareBox::RenderEngine::ColorRGBA8 col = SquareBox::RenderEngine::ColorRGBA8(255, 255, 255, 255 * alpha);

         glm::vec2 position = glm::vec2(10 * db.x + 5, 10 * db.y + 5 + heightScore);
         float radius = db.radius;

         glm::vec2 orign = (position - glm::vec2(radius) * 0.5f);
         SquareBox::AssetManager::GLTexture m_texture = SquareBox::AssetManager::TextureManager::getTextureByFilePath("Assets/Textures/nodpi/white background.png");
         m_sprite_batch.draw(glm::vec4(orign, glm::vec2(radius)), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), m_texture.id, 1.0f, col);

    }

    snake.draw(m_sprite_batch, m_sprite_batch);

    SquareBox::RenderEngine::ColorRGBA8 col = SquareBox::RenderEngine::ColorRGBA8(255, 255, 0, 255);

    glm::vec2 position = glm::vec2(10 * bonus.x + 5, 10 * bonus.y + 5 + heightScore);
    float radius = 5;
    glm::vec2 orign(position - glm::vec2(radius) * 0.5f);
    SquareBox::AssetManager::GLTexture m_texture = SquareBox::AssetManager::TextureManager::getTextureByFilePath("Assets/Textures/nodpi/white background.png");
    m_sprite_batch.draw(glm::vec4(orign, glm::vec2(radius)), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), m_texture.id, 1.0f, col);


    
    SquareBox::RenderEngine::ColorRGBA8 new_col = SquareBox::RenderEngine::ColorRGBA8(255, 255, 100, 255);

    double bonusScale = 0.8 + 0.7 * pow(sin(3.14159 * m_time / 2), 2);
    radius= radius*bonusScale;
    m_sprite_batch.draw(glm::vec4(orign, glm::vec2(radius)), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), m_texture.id, 1.0f, new_col);


    for (Bomb& bomb : bombs)
    {
       
        SquareBox::RenderEngine::ColorRGBA8 boom_color = SquareBox::RenderEngine::ColorRGBA8(100, 0, 0, 255);
        glm::vec2 boom_position(bomb.x * 10 + 5, bomb.y * 10 + 5 + heightScore);
        float size = 10*1.2f;
        glm::vec2 boom_orign = (boom_position - glm::vec2(size) * 0.5f);
        m_sprite_batch.draw(glm::vec4(boom_orign, glm::vec2(size)), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), m_texture.id, 1.0f, boom_color);


        SquareBox::RenderEngine::ColorRGBA8 new_boom_color = SquareBox::RenderEngine::ColorRGBA8(150, 0, 0, 255);

        double bombScale = 1 + 0.8 * pow(sin(3.14159 * m_time * 2.0), 2.0);
        size = size * bombScale;
        m_sprite_batch.draw(glm::vec4(boom_orign, glm::vec2(size)), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), m_texture.id, 1.0f, new_boom_color);

        SquareBox::RenderEngine::ColorRGBA8 new_new_boom_color = SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::white);
        size = size * (bombScale/4.0);
        
        m_sprite_batch.draw(glm::vec4(boom_orign, glm::vec2(size)), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), m_texture.id, 1.0f, new_new_boom_color);

    }

    life += (snake.getLife() - life) / 8.0;

   
    glm::vec2 life_bar_dimensions(2 * life, 10);
    glm::vec2 life_bar_position(m_game_ptr->getWindow()->getScreenWidth() - life - 10, m_game_ptr->getWindow()->getScreenHeight()-16);
    SquareBox::RenderEngine::ColorRGBA8 life_bar_color(255 - 255 * life / 100.0, 255 * life / 100.0, 0, 255);

    glm::vec2 life_bar_orign = (life_bar_position - life_bar_dimensions * 0.5f);


    m_sprite_batch.draw(glm::vec4(life_bar_orign, life_bar_dimensions), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), m_texture.id, 1.0f, life_bar_color);


    //sf::vertexarray lifeframe(sf::linesstrip, 5);
    //lifeframe[0].position = glm::vec2(289, 9);
    //lifeframe[1].position = glm::vec2(289 + 203, 9);
    //lifeframe[2].position = glm::vec2(289 + 203, 9 + 13);
    //lifeframe[3].position = glm::vec2(288, 9 + 13);
    //lifeframe[4].position = glm::vec2(289, 9);
    //screen.draw(lifeframe);

    if (danger)
    {
        warning.setPosition(m_game_ptr->getWindow()->getScreenWidth()*0.5f, m_game_ptr->getWindow()->getScreenHeight()-5);
        double c = 50 * sin(m_time * 20);
        warning.setColor(SquareBox::RenderEngine::ColorRGBA8(255, 100 + c, 100 + c, 255));
        m_sprite_font.draw(m_sprite_batch, warning.text, warning.position,glm::vec2(1.0f),1.0f,SquareBox::RenderEngine::ColorRGBA8(warning.color));


        warning.setColor(SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::red));
        m_sprite_font.draw(m_sprite_batch, warning.text, warning.position, glm::vec2(1.0f), 1.0f, SquareBox::RenderEngine::ColorRGBA8(warning.color));

    }
    else if (waiting)
    {
        warning.setPosition(m_game_ptr->getWindow()->getScreenWidth() * 0.5f, m_game_ptr->getWindow()->getScreenHeight() - 5);
        double c = 50 * sin(m_time * 5);
        warning.setColor(SquareBox::RenderEngine::ColorRGBA8(50 + c, 10, 10, 255));
        m_sprite_font.draw(m_sprite_batch, warning.text, warning.position, glm::vec2(1.0f), 1.0f, SquareBox::RenderEngine::ColorRGBA8(warning.color));


        warning.setColor(SquareBox::RenderEngine::ColorRGBA8(50 + c, 0, 0, 255));
        m_sprite_font.draw(m_sprite_batch, warning.text, warning.position, glm::vec2(1.0f), 1.0f, SquareBox::RenderEngine::ColorRGBA8(warning.color));

    }


    m_sprite_font.draw(m_sprite_batch, scoreText.text, scoreText.position, glm::vec2(1.0f), 1.0f, SquareBox::RenderEngine::ColorRGBA8(scoreText.color));
   m_sprite_font.draw(m_sprite_batch, scoreTransition.text, scoreTransition.position, glm::vec2(1.0f), 1.0f, SquareBox::RenderEngine::ColorRGBA8(scoreTransition.color));
    m_sprite_font.draw(m_sprite_batch, warning.text, warning.position,glm::vec2(1.0f),1.0f,SquareBox::RenderEngine::ColorRGBA8(warning.color));
    m_sprite_batch.end();
   
    m_texture_program.use();
    IGameScreen::preUpdateShader(&m_texture_program, "mySampler");
    //maintaining our backGround Color
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
