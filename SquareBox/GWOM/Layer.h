#pragma once
#include <glm/glm.hpp>
#include <Render-Engine/ColorRGBA8.h>
#include <Asset-Manager/Asset-Manager.h>
#include <Render-Engine/ParticleEngine.h>
#include <Camera/ParallelCamera.h>
#include "WorldCluster.h"
#include <vector>
#include <string>
#include "TileSystem.h"
#include <algorithm>



namespace SquareBox {
	namespace GWOM {
		/*
		  TABLE OF PARENT TEXTURES (These can be either single or tiled textures)
		  texture_index , texture;

		  TABLE OF SUB TEXTURES 
		  sub_texture_char,parent_texture_index,tiling_index
		*/

		class ParentTexture {
		public:
			int texture_index = 0;//index of its loaction in the associated vector
			AssetManager::GLTexture texture;
		};

		class SubTexture {
		public:
			TextureEnum parent_type = TextureEnum::SINGLE;
			int sub_texture_key = 0; // its identifer, its the same as the key found in the sub_textures_table
			int parent_texture_index = 0;// its index in the category it belongs (either single or tile textures)
			int tiling_index = 0;
			/* we shall use the tile sheet to extract the uv_rect of each tile_index */
		};

		class Layer
		{
		public:
			Layer() {};
			char name[40] = "UnLabelled Layer";
			int index = 0;
			int	 opacity = 100;
			bool is_visible = true;
			bool is_locked = false;
			std::vector<ParentTexture> single_textures;
			std::vector<ParentTexture> tiled_textures;
			std::map<int, SubTexture> sub_textures_table;//to be reconstructed on level loading
			TileSystem tile_system;
			/* if the layer is tiled , the information is going to be kept in the tile_system
			   and if its not tiled, the information is going to be kept in the system of worldClusters
			   and the alive_cluster_objects vector, i think this is mainly applicable to a case where we want to use 
			   box2d physics, where we want free motion of the objects in the editor
			*/
			std::vector<SquareBox::GWOM::WorldCluster> world_clusters;
			std::vector<std::pair<int, int>> alive_cluster_objects;
			std::vector<std::pair<int,int>>active_joints_body_a_objects; 
			/*
			 Joints system is not fully tested, there is need to make a game just about joints in order to test it out
			*/

			/*
			 android ndk wont let me create a vector of particle engine objects, it is only allowing
			 a vector of particle engine pointer


			 when i try creating the object versionot throws this error
			 In function `void std::_Destroy<SquareBox::RenderEngine::ParticleEngine>(SquareBox::RenderEngine::ParticleEngine*)':
			2>C:\\Microsoft\AndroidNDK64\android-ndk-r15c\sources\cxx-stl\gnu-libstdc++\4.9\include\bits/stl_construct.h:93: undefined reference to `SquareBox::RenderEngine::ParticleEngine::~ParticleEngine()'
			2>clang.exe: error: linker command failed with exit code 1 (use -v to see invocation)

			so am going with the pointers because with that approach , i wont even need to  the 
			the header file to the partcle engine but just forward declare the class

			
			*/
			SquareBox::RenderEngine::ParticleEngine particle_engine;
			SquareBox::Camera::ParallelCamera camera;

			//helper functions
			std::pair<int, glm::vec4> getTextureIdAndUvReactFromKey(int key_);
		};
	};
};



