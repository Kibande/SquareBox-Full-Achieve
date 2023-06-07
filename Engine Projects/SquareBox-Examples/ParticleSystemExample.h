#pragma once
#include <SquareBox-Core/SquareBoxGameEngine.h>
#include "IExample.h"
#include <ImGUI/imgui.h>
#include <ImGui/ImGuiFileBrowser.h>

namespace SquareBoxExample {

		class ParticleSystemExample : public SquareBoxExample::IExample
		{
		public:
			ParticleSystemExample();
			// Inherited via IExample
			virtual void onEntry(SquareBox::RenderEngine::Window* _window) override;
			virtual void onUpdate(float delta_time_, SquareBox::IMainGame* m_game_ptr_, SquareBox::Camera::ParallelCamera& m_parallel_camera) override;
			virtual void onDraw(SquareBox::RenderEngine::SpriteBatch* _spriteBatch, SquareBox::RenderEngine::SpriteFont* _spriteFont, SquareBox::RenderEngine::DebugRenderer* _debugRenderer) override;
			virtual void imGuiControls(SquareBoxExample::CustomConsole* _console) override;
			virtual void onExit() override;
			virtual void reset() override;


		private:
			SquareBox::RenderEngine::ParticleEngine m_particle_engine;
			SquareBox::RenderEngine::ParticleBatch m_particle_batch;
			int m_max_particles = 1000;
			float m_decay_rate = 0.5f;
			SquareBox::AssetManager::GLTexture m_texture;
			std::function<void(SquareBox::RenderEngine::Particle&, float)> m_update_function;
			glm::vec2 m_particle_velocity;
			SquareBox::RenderEngine::ColorRGBA8 m_particle_color;
			float m_imgui_particle_color[4];
			float m_particle_width = 5.0f;
			bool m_fade_particle = true;
			// for getting a new texture for the particles
			bool m_showOpenTextureFileDialog = false;
			imgui_addons::ImGuiFileBrowser m_file_dialog; // For working with file dialogs
			int m_num_emitted_particles =1 ;
			float m_fps;
		};

	};
