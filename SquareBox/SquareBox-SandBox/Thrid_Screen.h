#include "ScreenIndices.h"
#include "SquareBox-Editor/Editor_Assistant.h"
#include <SquareBox-Core/SquareBoxGameEngine.h>
class Thrid_Screen : public SquareBox::IGameScreen
{

public:
	Thrid_Screen();
	~Thrid_Screen();
	// Inherited via IGameScreen
	virtual int getNextScreenIndex() const override;
	virtual int getPreviousScreenIndex() const override;
	virtual void build() override;
	virtual void onEntry() override;
	virtual void update(const float& deltaTime_) override;
	virtual void draw() override;
	virtual void onExit() override;
	virtual void destroy() override;

	SquareBox::RenderEngine::SpriteBatch m_sprite_batch;
	SquareBox::RenderEngine::GLSLProgram m_texture_program;
	SquareBox::RenderEngine::GLSLProgram m_debug_program;

	SquareBox::RenderEngine::SpriteFont m_sprite_font;
	SquareBox::RenderEngine::DebugRenderer m_debug_renderer;

	SquareBox::GWOM::Layer m_layer;
	SquareBox::Utilities m_utilities;

	SquareBox::GWOM::ClusterObject m_placing_object;
	SquareBox::GWOM::ClusterObject m_hovering_object;
	SquareBox::IShape * m_current_shape_ptr =nullptr;
	int m_nextScreenIndex = SECOND_SCREEN_INDEX;
	int m_previousScreenIndex = SCREEN_INDEX_NO_SCREEN;

	SquareBoxEditor::Editor_Assistant m_editor_assitant;
	std::vector<SquareBox::GWOM::WorldCluster> m_world_cluster;
};

