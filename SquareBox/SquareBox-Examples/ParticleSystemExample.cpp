#include "ParticleSystemExample.h"
namespace SquareBox {
	namespace Example {
		ParticleSystemExample::ParticleSystemExample()
		{
			exampleTitle = "Particle System";
		}
		void ParticleSystemExample::onEntry(SquareBox::RenderEngine::Window* _window)
		{
			m_particle_engine.init();
			m_texture = SquareBox::AssetManager::TextureManager::getTextureByName("circle.png", _window->getDDPI());
			m_update_function = [](SquareBox::RenderEngine::Particle& particle_, float delta_time_) {
				//particles attributes that can be changed 
				particle_.color;
				particle_.life;
				particle_.position;
				particle_.velocity;
				particle_.width;

				particle_.position += particle_.velocity * delta_time_;
				particle_.color.a = (particle_.life * 255.0f);
			};

			m_particle_batch.init(m_max_particles, m_decay_rate, m_texture, m_update_function);

			m_particle_engine.addParticleBatch(&m_particle_batch);
			//particle defaults
			m_particle_velocity = glm::vec2(0.1f);
			m_particle_color = SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::red);

			isExampleInitialised = true;
			
		}

		void ParticleSystemExample::onUpdate(float delta_time_, SquareBox::IMainGame* m_game_ptr_, SquareBox::Camera::ParallelCamera& m_parallel_camera)
		{
			m_particle_engine.update(delta_time_);
			glm::vec2 mouse_in_world = m_parallel_camera.convertScreenToWorld(m_game_ptr_->getInputDevice()->getScreenLocations()[0].coordinates);

			if (m_game_ptr_->getInputDevice()->isInputIdBeingReceived(static_cast<int>(SquareBox::MouseEnum::LEFT_CLICK))) {
				for (unsigned i = 0; i < m_num_emitted_particles; i++)
				{
					m_particle_batch.addParticle(mouse_in_world, m_particle_velocity, m_particle_color, m_particle_width);
				}
			}
			m_fps = m_game_ptr_->getFps();
		}

		void ParticleSystemExample::onDraw(SquareBox::RenderEngine::SpriteBatch* _spriteBatch, SquareBox::RenderEngine::SpriteFont* _spriteFont, SquareBox::RenderEngine::DebugRenderer* _debugRenderer)
		{
			m_particle_engine.draw(_spriteBatch);
		}

		void ParticleSystemExample::imGuiControls(CustomAppConsole* _console)
		{
			//File Exposer
			if (m_file_dialog.showFileDialog("Open Texture File", imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(700, 310), ".png"))
			{
				m_texture = SquareBox::AssetManager::TextureManager::getTextureByFilePath(m_file_dialog.selected_path);
				m_texture.asset_file_path = m_file_dialog.selected_path;
				m_texture.display_name = m_file_dialog.selected_fn;
			}
			if (m_showOpenTextureFileDialog) {
				ImGui::OpenPopup("Open Texture File");
			}

			ImGui::Separator();
			ImGui::Text("Particle Batch Properties");
			ImGui::Text("Max Partciles   : "); ImGui::SameLine();
			ImGui::InputInt("##max_particles", &m_max_particles, 10, 100);
			ImGui::Text("Partcile Decay  : "); ImGui::SameLine();
			ImGui::InputFloat("###b_w", &m_decay_rate, 0.5f, 0, "%.10f");
			ImGui::Separator();


			ImGui::Text("Individual Partcile Properties ");
			ImGui::InputInt("Emitted Particles", &m_num_emitted_particles, 1, 10);
			ImGui::InputFloat("Width", &m_particle_width, 1.0f, 10.0f, "%.3f");
			ImGui::InputFloat("velocity_x", &m_particle_velocity.x, 0.01f, 0, "%.3f");
			ImGui::InputFloat("velocity_y", &m_particle_velocity.y, 0.01f, 0, "%.3f");
			glm::ivec4 particle_color_in_ivec4 = m_particle_color.getIVec4();
			m_imgui_particle_color[0] = SquareBox::MathLib::Float::divideAndGetFloat(particle_color_in_ivec4.x, 255);
			m_imgui_particle_color[1] = SquareBox::MathLib::Float::divideAndGetFloat(particle_color_in_ivec4.y, 255);
			m_imgui_particle_color[2] = SquareBox::MathLib::Float::divideAndGetFloat(particle_color_in_ivec4.z, 255);
			m_imgui_particle_color[3] = SquareBox::MathLib::Float::divideAndGetFloat(particle_color_in_ivec4.w, 255);
			ImGui::ColorEdit4("Color", m_imgui_particle_color);
			m_particle_color.r = m_imgui_particle_color[0] * 255;
			m_particle_color.g = m_imgui_particle_color[1] * 255;
			m_particle_color.b = m_imgui_particle_color[2] * 255;
			m_particle_color.a = m_imgui_particle_color[3] * 255;

			ImGui::Separator();

			ImGui::Text("Source:"); ImGui::SameLine();
			ImGui::Text(m_texture.asset_file_path.c_str());
			if (ImGui::Button("+ New")) {
				m_showOpenTextureFileDialog = true;
			}


			ImGui::Text("Controls");
			ImGui::Separator();
			if (ImGui::Button("Reset")) {
				reset();
			}
			ImGui::Text("FPS Counter: %.2f", static_cast<float>(m_fps));
			m_showOpenTextureFileDialog = false;
		}

		void ParticleSystemExample::onExit()
		{
			m_particle_batch.dispose();
			m_particle_engine.dispose();
		}

		void ParticleSystemExample::reset()
		{
			m_particle_engine.dispose();
			m_particle_batch.dispose();

			m_particle_engine.init();
			m_particle_batch.init(m_max_particles, m_decay_rate, m_texture, m_update_function);
			m_particle_engine.addParticleBatch(&m_particle_batch);
		}

	}
}
