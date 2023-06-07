#include "SplashScreen.h"

int SplashScreen::getNextScreenIndex() const
{
    return m_next_screen_index;
}

int SplashScreen::getPreviousScreenIndex() const
{
    return m_previous_screen_index;
}

void SplashScreen::build()
{
}

void SplashScreen::onEntry()
{
    // set up camera
    glm::vec2 screen_dimensions(m_game_ptr->getWindow()->getScreenWidth(), m_game_ptr->getWindow()->getScreenHeight());
    m_camera.init(screen_dimensions);
    m_camera.setPosition(screen_dimensions * 0.5f);
    m_camera.setScale(1.0f);

    // set up texture program
    m_texture_program.init();
    m_texture_program.compileDefaultTextureShaders();
    m_texture_program.addDefaultTextureAttributes();
    m_texture_program.linkShaders();

    // sprite batch
    m_sprite_batch.init();

    //load textures
   
    m_game_studio_logo = SquareBox::AssetManager::TextureManager::getTextureByName("Colony Protocol Icon.png", m_game_ptr->getWindow()->getDDPI());
    
    m_game_logo = SquareBox::AssetManager::TextureManager::getTextureByName("snake game logo transparent bg.png", m_game_ptr->getWindow()->getDDPI());
    m_timer.Reset();
    m_initial_recorded_time = m_timer.ElapsedMillis();



}

void SplashScreen::update(const float& deltaTime_)
{
    glm::vec2 screen_dimensions(m_game_ptr->getWindow()->getScreenWidth(), m_game_ptr->getWindow()->getScreenHeight());

    m_camera.update(screen_dimensions.x, screen_dimensions.y);
    
    float elapsed_milliseconds = m_timer.ElapsedMillis() - m_initial_recorded_time;


    if (elapsed_milliseconds > 2000) {
        m_current_texture_id = m_game_studio_logo.id;

        if (elapsed_milliseconds > 3000) {
            setScreenState(SquareBox::ScreenState::CHANGE_NEXT);
        }
    }
    else {
        m_current_texture_id = m_game_logo.id;
    }
}

void SplashScreen::draw()
{

    m_sprite_batch.begin();
    
    glm::vec2 screen_dimensions(m_game_ptr->getWindow()->getScreenWidth(), m_game_ptr->getWindow()->getScreenHeight());
    glm::vec2 position(screen_dimensions*0.5f);
    float size = std::min(screen_dimensions.x,screen_dimensions.y) * 0.3f;
    glm::vec2 orign(position - glm::vec2(size * 0.5f));




   
    m_sprite_batch.draw(glm::vec4(orign,size,size), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), m_current_texture_id, 1.0f, SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::white));
    m_sprite_batch.end();

    m_texture_program.use();
    IGameScreen::preUpdateShader(&m_texture_program, "mySampler");
    IGameScreen::uploadCameraInfo(&m_texture_program, &m_camera, "P");
    m_sprite_batch.renderBatch();
    m_texture_program.unuse();
}

void SplashScreen::onExit()
{
    m_texture_program.dispose();
    m_sprite_batch.dispose();
}

void SplashScreen::destroy()
{
}
