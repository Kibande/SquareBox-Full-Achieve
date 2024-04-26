#pragma once
#ifdef SQB_PLATFORM_ANDROID
using namespace std;
//solving for undeclared identifier 'isfinite in b2_math.h, we need a better fix
#endif
#include <glm/glm.hpp>
#include<vector>
#include<algorithm> 
#include<cmath>
#include <Render-Engine/ColorRGBA8.h>
#include <SquareBox-Core/Essentials.h>
#include <Asset-Manager/Asset-Manager.h>
#include "Joint.h"
#include "TextureInfo.h"
//Being specific here helps us to avoid falling prey to circular includes
namespace SquareBox {
	namespace PhysicsCollisionEngine {
		class PhysicsProperties;
	};
};

namespace SquareBox {
	namespace GWOM {
		struct Cell;
	};
};
namespace SquareBox {
	namespace GWOM {



		class ClusterObject {
		public:
			ClusterObject() {};
			~ClusterObject() {};
			/*methods*/

			/*ints*/
			int layer_index = 0;// index of the parent layer, this is not stored in our saved levels
			int cluster_index = 0; //index of the parent cluster
			int index = 0;     // index in the cluster
			int deletion_index = -1; //negative one means that this body has not yet been deleted
			float life_span = 10.0f;

			//box2d overlap
			//objects of the same positive group index collide
			//objects of the same negative group index don't collide
			int group_index = 1;
			SquareBox::FamilyBitsEnum family = SquareBox::FamilyBitsEnum::Family_a;
			std::vector<int> vec_of_mask_bits{ static_cast<int>(SquareBox::MaskBitsEnum::Mask_a) };
			/*Strings*/
			char name[20];
			/*floats*/
			float height = 0.0f;
			float width = 0.0f;
			float radius = 0.0f;
			float density = 1.0f;
			float friction = 0.3f;
			float restitution = 0.5f;
			float angle = 0.0f;
			float init_angular_velocity = 0.0f;
			float frequency = 4.0f;
			//damping Reduces world velocity on bodies
			float linear_damping = 0.0f;
			float angular_damping = 0.01f;
			float gravity_scale = 1.0f;// 0 makes the body float
			float resistivity = 2.0f;

			/*bools*/
			//Hovering Edge and Chains ends
			bool is_first_hovered = false;
			bool is_last_hovered = false;

			//Selecting Edge and Chain Ends
			bool is_first_selected = false;
			bool is_last_selected = false;
			bool is_hidden = false;//controls of the ClusterObject is visible or not
			bool is_fixed_rotation = false;
			bool is_alive = false;// whether the object is just a shell or a real cluster object
			bool allow_sleep = true;//can the body sleep
			bool awake = true;
			bool is_bullet = false;
			bool requesting_recreation = false;
			SquareBox::ControlledTypeEnum controller_type = SquareBox::ControlledTypeEnum::freelance;

			/*Enum*/
			SquareBox::BodyShapeEnum shape = SquareBox::BodyShapeEnum::Box;
			SquareBox::BodyTypeEnum type = SquareBox::BodyTypeEnum::StaticBody;

			/*glm::vecs*/
			std::vector<std::pair<glm::vec2, glm::vec2>> vec_of_edges;
			std::vector<glm::vec2> vertices;
			std::vector<Joint> joints;
			glm::vec2 off_set = glm::vec2(0.0f);
			glm::vec2 position = glm::vec2(0);
			glm::vec2 init_linear_velocity = glm::vec2(0.0f);
			glm::vec2 direction = glm::vec2(0.0f);//These are ment to be used in pure 2D
			float speed = 0.0f;
		

			/*Externals*/
			//PhysicsProperties physics_properties;
			SquareBox::PhysicsCollisionEngine::PhysicsProperties* physics_properties = nullptr;
			TextureInfo texture_info;
			Cell * associated_pointer = nullptr;

			//some POD
			/*
				We have to store these in our levels

			*/
			void* pod_ptr = nullptr;

			float pod_float = 0.0f;

			int pod_int = 0;

			bool pod_bool = false;

			glm::vec2 pod_vec2;
			std::pair<int, int> pod_pair;

			glm::vec4 pod_vec4;

		};
	};
};




