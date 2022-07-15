#pragma once
#include "LayerTypeInterface.h"
class IsometricLayer: public LayerTypeInterface
{
public:
	IsometricLayer();
	~IsometricLayer();
	// Inherited via LayerTypeInterface
	virtual void onInit() override;
	virtual void onLayerInit(SquareBox::GWOM::Layer& layer_) override;
	virtual int onLayerInitGui(SquareBox::GWOM::Layer& layer_) override;
	virtual void onPrepareForStorage(std::vector<SquareBox::GWOM::Layer>& layers_, int active_layer_index_) override;
	virtual void onLoadFromStorage(std::vector<SquareBox::GWOM::Layer>& layers_, int active_layer_index_) override;
	virtual void onFocus(SquareBox::GWOM::Layer& layer_, EditorModeEnum editor_mode_) override;
	virtual void onOutOfFocus(SquareBox::GWOM::Layer& layer_) override;
	virtual void onLayerIndexChange(SquareBox::GWOM::Layer& active_layer_, int new_layer_index_) override;
	virtual void onUpdateProcessingInput(float deltaTime_, std::vector<SquareBox::GWOM::Layer>& layers_, int active_layer_index_, SquareBox::IMainGame* game_ptr_, EditorModeEnum editor_mode_) override;
	virtual void onUpdateWithOrWithoutProcessing(float deltaTime_, std::vector<SquareBox::GWOM::Layer>& layers_, int active_layer_index_, SquareBox::IMainGame* game_ptr_, EditorModeEnum editor_mode_) override;
	virtual void onContentDraw(SquareBox::GWOM::Layer& layer_, SquareBox::RenderEngine::SpriteBatch& sprite_batch_) override;
	virtual void onDebugDraw(SquareBox::GWOM::Layer& layer_, SquareBox::RenderEngine::DebugRenderer& debug_renderer_) override;
	virtual void onOutsideDebugModeDebugDraw(SquareBox::GWOM::Layer& layer_, SquareBox::IMainGame* game_ptr_, SquareBox::RenderEngine::DebugRenderer& debug_renderer_, EditorModeEnum editor_mode_) override;
	virtual void onLightDraw(SquareBox::GWOM::Layer& layer_, SquareBox::RenderEngine::SpriteBatch& sprite_batch_) override;
	virtual void onGUILeftPanelDraw(std::vector<SquareBox::GWOM::Layer>& layers_, int active_layer_index_, EditorModeEnum editor_mode_) override;
	virtual void onLayerPropertiesGUIDraw(std::vector<SquareBox::GWOM::Layer>& layers_, int active_layer_index_, EditorModeEnum editor_mode_) override;
	virtual void onObjectPropertiesGUIDraw(std::vector<SquareBox::GWOM::Layer>& layers_, int active_layer_index_, EditorModeEnum editor_mode_) override;
	virtual void onDispose() override;
	virtual void onLayerDispose(SquareBox::GWOM::Layer& layer_) override;
	
};

