#include "ParticleBatch.h"

namespace SquareBox {
	namespace RenderEngine {
		ParticleBatch::ParticleBatch()
		{
			//Empty
		}

		ParticleBatch::~ParticleBatch()
		{
		}

		void ParticleBatch::init(int maxParticles, float decayRate, SquareBox::AssetManager::GLTexture texture,
			std::function<void(Particle&, float)> updateFunc /*  =defaultParticleUpdate */)
		{
			m_maxParticles = maxParticles;
			m_decayeRate = decayRate;
			m_texture = texture;
			m_particles = new Particle[m_maxParticles];//allocate an array on the heap
			m_updateFunc = updateFunc;
		}

		void ParticleBatch::update(float deltaTime)
		{
			//loop  through all our particles and update them
			for (int i = 0; i < m_maxParticles; i++)
			{
				//check if it is active
				if (m_particles[i].life > 0.0f) {
					m_updateFunc(m_particles[i], deltaTime);//call the particles custom update function
					m_particles[i].life -= m_decayeRate * deltaTime;//particle decays a little bit on each update
				}
			}
		}

		void ParticleBatch::draw(SpriteBatch* spriteBatch)
		{
			glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);

			//loop  through all our particles and draw them
			for (int i = 0; i < m_maxParticles; i++)
			{
				//check if it is active
				if (m_particles[i].life > 0.0f) {
					auto& p = m_particles[i];
					glm::vec4 destRect(p.position.x, p.position.y, p.width, p.width);
					spriteBatch->draw(destRect, uvRect, m_texture.id, 0.0f, p.color);
				}
			}
		}

		void ParticleBatch::dispose()
		{
			delete[] m_particles;
		}

		void ParticleBatch::addParticle(const glm::vec2 & position, const glm::vec2 & velocity, const ColorRGBA8 & color, float width)
		{
			/*
			 since we don't want be be called new and delete all the time, we
			 just want to have a big m_buffer of particle memory, that
			 we are constantly reducing

			*/
			int partcileIndex = findfreeParticle();

			auto&p = m_particles[partcileIndex];
			p.life = 1.0f;//we are staring life at one
			p.position = position;
			p.velocity = velocity;
			p.color = color;
			p.width = width;
		}

		int SquareBox::RenderEngine::ParticleBatch::findfreeParticle()
		{
			for (int i = m_lastFreeParticle; i < m_maxParticles; i++)
			{
				if (m_particles[i].life <= 0.0f) {
					m_lastFreeParticle = i;
					return i;
				}
			}

			for (int i = 0; i < m_lastFreeParticle; i++)
			{
				if (m_particles[i].life <= 0.0f) {
					m_lastFreeParticle = i;
					return i;
				}
			}
			//No Particles are free so overrite the first particles
			return 0;
		}
	}
}