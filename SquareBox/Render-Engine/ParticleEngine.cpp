#include "ParticleEngine.h"

namespace SquareBox {
	namespace RenderEngine {
		ParticleEngine::ParticleEngine()
		{
		}

		ParticleEngine::~ParticleEngine()
		{
		}
		void ParticleEngine::init(std::string engineName_) {
			engine_name=engineName_;
			m_is_initilised = true;
		}
		void ParticleEngine::dispose() {
			//delete all  the particleBatches

			m_batches.clear();
		}
		void ParticleEngine::addParticleBatch(ParticleBatch * particleBatch)
		{
			m_batches.push_back(particleBatch);
		}

		void ParticleEngine::update(float deltaTime)
		{
			for (auto&b : m_batches) {
				b->update(deltaTime);
			}
		}

		void ParticleEngine::draw(SpriteBatch * spriteBatch)
		{
			for (auto& b : m_batches) {
				//begin,end, and renderBarch must be called by the user of this function
				b->draw(spriteBatch);
			}
		}
	}
}