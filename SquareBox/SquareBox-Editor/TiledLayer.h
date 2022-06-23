#pragma once
#include <SquareBox-Core/SquareBoxGameEngine.h>
#include <ImGUI/imgui.h>
#include "LayerTypeInterface.h"
#include "Editor_Assistant.h"
enum TiledLayerSelectModeEnum {
	SINGLETILE = 0,		//	???
	MULTIPLETILES = 1	//	???
};

class TiledLayer :
    public LayerTypeInterface
{
public:
	TiledLayer();
	~TiledLayer();
	TiledLayerSelectModeEnum m_tiled_layer_selection_mode_index = TiledLayerSelectModeEnum::SINGLETILE;

	SquareBox::RenderEngine::ColorRGBA8 m_place_mode_sketch_color = SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::antique_white);
	SquareBox::RenderEngine::ColorRGBA8 m_selected_body_color = SquareBox::RenderEngine::ColorRGBA8(231, 238, 10, 255);

	bool m_show_grid_lines = false;
	bool m_show_shift_tiles_to_another_layer_dialog = false;
	int m_target_shift_to_layer_index = 0;
	std::vector<std::pair<int, int>> m_selected_tiles; // Used for tiling layer layout (layer_index_tile_index)

	SquareBox::RenderEngine::ColorRGBA8 m_layer_grid_color = SquareBox::RenderEngine::ColorRGBA8(SquareBox::Color::white);

	int tiled_layer_place_mode_fill_tile = static_cast<int>(SquareBox::MouseEnum::LEFT_CLICK);
	/* Selection Mode */
	int tiled_layer_select_mode_select_tile = static_cast<int>(SquareBox::MouseEnum::LEFT_CLICK);

	//deletion key
	int delete_selection_input_key = static_cast<int>(SquareBox::KeyBoardEnum::KEYBOARD_DELETE);

	SquareBoxEditor::Editor_Assistant m_editor_assitant;

	int m_active_sub_texture_key = 0;//This is the texture that is being applied to the next tile when we click
	// leave the default set to 0 so that we can have some black sub textures if we want to and we do

	float m_layer_width = 0.0f;
	float m_layer_height = 0.0f;
	float m_layer_tile_size = 0.0f;
	// Inherited via LayerTypeInterface
	virtual void onInit() override;
	virtual void onLayerInit(SquareBox::GWOM::Layer& layer_) override;
	virtual int onLayerInitGui(SquareBox::GWOM::Layer& layer_) override;
	virtual void onPrepareForStorage(std::vector<SquareBox::GWOM::Layer>& layers_, int active_layer_index_) override;
	virtual void onLoadFromStorage(std::vector<SquareBox::GWOM::Layer>& layers_, int active_layer_index_) override;
	virtual void onFocus(SquareBox::GWOM::Layer& layer_, EditorModeEnum editor_mode_) override;
	virtual void onLayerIndexChange(SquareBox::GWOM::Layer& active_layer_, int new_layer_index_) override;
	virtual void onUpdateProcessingInput(float deltaTime_, std::vector<SquareBox::GWOM::Layer>& layers_, int active_layer_index_,SquareBox::IMainGame* game_ptr_, EditorModeEnum editor_mode_) override;
	virtual void onUpdateWithOrWithoutProcessing(float deltaTime_, std::vector<SquareBox::GWOM::Layer>& layers_, int active_layer_index_,SquareBox::IMainGame* game_ptr_, EditorModeEnum editor_mode_) override;
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

