#pragma once
#include <glm/glm.hpp>
#include "Vertex.h"
#include "SpriteBatch.h"
#include <Asset-Manager/Asset-Manager.h>
#include <functional> //This will give us the ability to have functional pointers
namespace SquareBox {
	namespace RenderEngine {
		struct Particle {
			glm::vec2 position = glm::vec2(0.0f);
			glm::vec2 velocity = glm::vec2(0.0f);
			ColorRGBA8 color;
			float life = 0.0f;
			float width = 0.0f;//assuming that all our particles are squares
		};

		inline void defaultParticleUpdate(Particle& particle, float deltaTime)
		{
			//This is just incase we dont want to customise the particles motion

			//The inline key word is used when we randomly declare a function like this in the header file'
			//yet it is going to be used by multiple files
			particle.position += particle.velocity * deltaTime;
		}

		class ParticleBatch
		{
		public:
			friend class ParticleEngine;
			ParticleBatch();
			~ParticleBatch();
			void init(int maxParticles,
				float decayRate,
				SquareBox::AssetManager::GLTexture texture,
				std::function<void(Particle&, float)> updateFunc = defaultParticleUpdate);
			//after adding a particle batch,the particle engine becomes responsible for deallocation
			void addParticle(const glm::vec2& position, const glm::vec2& velocity, const ColorRGBA8& color, float width);

			void dispose();
		private:
			void update(float deltaTime);
			void draw(SpriteBatch* spriteBatch);
			std::function<void(Particle&, float)> m_updateFunc;//function pointer to the update function
			int findfreeParticle();
			float m_decayeRate = 0.1f;
			Particle* m_particles = nullptr;//a dynamic array of particles

			int m_maxParticles = 0;//max number of particles we can have

			SquareBox::AssetManager::GLTexture m_texture;

			int m_lastFreeParticle = 0;
		};
	}
}
