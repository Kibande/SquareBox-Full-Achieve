#pragma once
#include "ParticleBatch.h"
#include "SpriteBatch.h"
#include <vector>
namespace SquareBox {
	namespace RenderEngine {
		class ParticleEngine

		{
		public:
			ParticleEngine();
			~ParticleEngine();
			void init();
			//add a New Partilce Batch
			void addParticleBatch(ParticleBatch* particleBatch);
			void update(float deltaTime);
			void draw(SpriteBatch* spriteBatch);
			bool isInitialised() const { return m_is_initilised; }
			void dispose();
		private:
			std::vector<ParticleBatch*> m_batches;
			bool m_is_initilised = false;
		};
	}
}
