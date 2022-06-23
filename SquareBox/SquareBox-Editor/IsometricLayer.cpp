#include "IsometricLayer.h"

IsometricLayer::IsometricLayer()
{
}

IsometricLayer::~IsometricLayer()
{
}

void IsometricLayer::onInit()
{
	layer_type = SquareBox::LayerTypeEnum::IsometricLayer;
	m_utilities.init();
}

void IsometricLayer::onLayerInit(SquareBox::GWOM::Layer& layer_)
{
	layer_.layer_type = SquareBox::LayerTypeEnum::IsometricLayer;

}

int IsometricLayer::onLayerInitGui(SquareBox::GWOM::Layer& layer_)
{
	return -1;
}

void IsometricLayer::onPrepareForStorage(std::vector<SquareBox::GWOM::Layer>& layers_, int active_layer_index_)
{
}

void IsometricLayer::onLoadFromStorage(std::vector<SquareBox::GWOM::Layer>& layers_, int active_layer_index_)
{
}

void IsometricLayer::onFocus(SquareBox::GWOM::Layer& layer_, EditorModeEnum editor_mode_)
{
}

void IsometricLayer::onLayerIndexChange(SquareBox::GWOM::Layer& active_layer_, int new_layer_index_)
{
	active_layer_.index = new_layer_index_;
}

void IsometricLayer::onUpdateProcessingInput(float deltaTime_, std::vector<SquareBox::GWOM::Layer>& layers_, int active_layer_index_,SquareBox::IMainGame* game_ptr_, EditorModeEnum editor_mode_)
{
}

void IsometricLayer::onUpdateWithOrWithoutProcessing(float deltaTime_, std::vector<SquareBox::GWOM::Layer>& layers_, int active_layer_index_, SquareBox::IMainGame* game_ptr_, EditorModeEnum editor_mode_)
{
}

void IsometricLayer::onContentDraw(SquareBox::GWOM::Layer& layer_, SquareBox::RenderEngine::SpriteBatch& sprite_batch_)
{
}

void IsometricLayer::onDebugDraw(SquareBox::GWOM::Layer& layer_, SquareBox::RenderEngine::DebugRenderer& debug_renderer_)
{
}

void IsometricLayer::onOutsideDebugModeDebugDraw(SquareBox::GWOM::Layer& layer_, SquareBox::IMainGame* game_ptr_, SquareBox::RenderEngine::DebugRenderer& debug_renderer_, EditorModeEnum editor_mode_)
{
}

void IsometricLayer::onLightDraw(SquareBox::GWOM::Layer& layer_, SquareBox::RenderEngine::SpriteBatch& sprite_batch_)
{
}

void IsometricLayer::onGUILeftPanelDraw(std::vector<SquareBox::GWOM::Layer>& layers_, int active_layer_index_, EditorModeEnum editor_mode_)
{
}


void IsometricLayer::onLayerPropertiesGUIDraw(std::vector<SquareBox::GWOM::Layer>& layers_, int active_layer_index_, EditorModeEnum editor_mode_)
{
}

void IsometricLayer::onObjectPropertiesGUIDraw(std::vector<SquareBox::GWOM::Layer>& layers_, int active_layer_index_, EditorModeEnum editor_mode_)
{
}

void IsometricLayer::onDispose()
{
	m_utilities.dispose();
}

void IsometricLayer::onLayerDispose(SquareBox::GWOM::Layer& layer_)
{
}
