#pragma once
#include <SquareBox-Core/SquareBoxGameEngine.h>
#include "EditorEnums.h"
/*
	layers can either be 
	flat  -  no grid
	tiled - 2D grid system
	isometric - isometric grid system
*/

/* this interface class handles the passed in layer as if it was the last layer to call onFocus , 
	so before a layer accesses any of its function it should have been the last layer to call onFocus
	as that prepares the shells variables
*/
class LayerTypeInterface {
	public:
		//sets up the interfaces parameters
		virtual void onInit() = 0;
		// initialses a layer to be to a specfic layer type
		virtual void onLayerInit(SquareBox::GWOM::Layer& layer_) = 0;
		// 0 - user cancelled.  1 - user accepted.  <0 user did not make selection yet
		virtual int onLayerInitGui(SquareBox::GWOM::Layer& layer_) = 0;
		virtual void onPrepareForStorage(std::vector<SquareBox::GWOM::Layer> & layers_, int active_layer_index_) = 0;
		virtual void onLoadFromStorage(std::vector<SquareBox::GWOM::Layer>& layers_, int active_layer_index_) = 0;
		/*
			called when we have just switcheed to this layer type
			the parameters setup here help with the behind the scenes leg work of the engine and imgui 
		*/
		virtual void onLayerIndexChange(SquareBox::GWOM::Layer& active_layer_, int new_layer_index_) =0 ;
		virtual void onFocus(SquareBox::GWOM::Layer& layer_, EditorModeEnum editor_mode_) = 0;
		virtual void onOutOfFocus(SquareBox::GWOM::Layer& layer_) = 0;
		virtual void onUpdateProcessingInput(float deltaTime_,std::vector<SquareBox::GWOM::Layer> & layers_, int active_layer_index_,SquareBox::IMainGame* game_ptr_, EditorModeEnum editor_mode_) = 0;
		
		// done even when the layer is not the one receiving input
		virtual void onUpdateWithOrWithoutProcessing(float deltaTime_,std::vector<SquareBox::GWOM::Layer> & layers_, int active_layer_index_,SquareBox::IMainGame* game_ptr_, EditorModeEnum editor_mode_) = 0;
		virtual void onContentDraw(SquareBox::GWOM::Layer& layer_,SquareBox::RenderEngine::SpriteBatch & sprite_batch_) = 0;
		virtual void onDebugDraw(SquareBox::GWOM::Layer& layer_,SquareBox::RenderEngine::DebugRenderer & debug_renderer_) = 0;
		//this is mostly for the placement object skeleton  and drawing selected objects
		virtual void onOutsideDebugModeDebugDraw(SquareBox::GWOM::Layer& layer_, SquareBox::IMainGame* game_ptr_, SquareBox::RenderEngine::DebugRenderer & debug_renderer_, EditorModeEnum editor_mode_) = 0;
		virtual void onLightDraw(SquareBox::GWOM::Layer& layer_, SquareBox::RenderEngine::SpriteBatch& sprite_batch_) = 0;

		virtual void onGUILeftPanelDraw(std::vector<SquareBox::GWOM::Layer>& layers_, int active_layer_index_, EditorModeEnum editor_mode_) = 0; // draws the IMGUI layer interface
		virtual void onLayerPropertiesGUIDraw(std::vector<SquareBox::GWOM::Layer>& layers_, int active_layer_index_, EditorModeEnum editor_mode_) = 0; 
		virtual void onObjectPropertiesGUIDraw(std::vector<SquareBox::GWOM::Layer>& layers_, int active_layer_index_, EditorModeEnum editor_mode_) = 0;
		//dispose layer parameters
		virtual void onDispose() = 0;
		// disposes layer from being a particular type
		virtual void onLayerDispose(SquareBox::GWOM::Layer& layer_) = 0;

		SquareBox::LayerTypeEnum layer_type;

		SquareBox::Utilities m_utilities;
		bool m_is_all_work_saved = true;
};