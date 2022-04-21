#pragma once
#include <SquareBox-Core/SquareBoxGameEngine.h>
class Fourth_Screen : public SquareBox::IGameScreen
{
public:
	Fourth_Screen(SquareBox::RenderEngine::Window *window_);
	~Fourth_Screen();
	// Inherited via IGameScreen
	virtual int getNextScreenIndex() const override;
	virtual int getPreviousScreenIndex() const override;
	virtual void build() override;
	virtual void onEntry() override;
	virtual void update(const float & deltaTime_) override;
	virtual void draw() override;
	virtual void onExit() override;
	virtual void destroy() override;

private:
	SquareBox::RenderEngine::Window * m_window_ptr = nullptr;
	SquareBox::RenderEngine::GLSLProgram m_texture_program;
	SquareBox::RenderEngine::GLSLProgram m_debug_program;
	SquareBox::InputManager::IInput * m_input_handler;
	SquareBox::RenderEngine::SpriteBatch m_sprite_batch;
	SquareBox::RenderEngine::DebugRenderer m_debug_renderer;
	SquareBox::RenderEngine::SpriteFont m_sprite_font;
	std::vector<SquareBox::GWOM::Layer> m_layers;
	std::vector<SquareBox::GWOM::WorldCluster> m_vec_of_world_clusters;
	int m_active_layer_index = 0;
	SquareBox::IShape * m_current_shape_ptr = nullptr;
	SquareBox::AudioSystem::AudioEngine * m_audio_engine_ptr_1 = nullptr;
	SquareBox::AudioSystem::AudioEngine * m_audio_engine_ptr_2 = nullptr;
	SquareBox::AudioSystem::Music m_music_1;
	SquareBox::AudioSystem::SoundBank m_sound_bank;
};

