#include "LevelReaderWriter.h"
#include "DSUGWOM_generated.h"
#include <fstream>

namespace SquareBox {
	//Game World Object Model
	namespace GWOM
	{
		bool LevelReaderWriter::saveWorldAsBinary(const std::string& file_path_, const std::vector<WorldCluster>& vector_of_world_clusters_, const std::vector<SquareBox::GWOM::Layer>& layers_, float active_camera_scale_, const glm::vec2& active_camera_position_, int active_camera_index_)
		{
			flatbuffers::FlatBufferBuilder builder(1024);
			std::vector<flatbuffers::Offset<SquareBox::DSUGWOM::WorldCluster>> vec_of_fb_world_clusters;

			for (unsigned int i = 0; i < vector_of_world_clusters_.size(); i++)
			{
				std::vector<flatbuffers::Offset<SquareBox::DSUGWOM::ClusterObject>> vec_of_fb_cluster_objects;
				const SquareBox::GWOM::WorldCluster & cwcobj = vector_of_world_clusters_[i];

				for (unsigned int j = 0; j < cwcobj.cluster_objects.size(); j++)
				{
					const SquareBox::GWOM::ClusterObject * ccobj = &cwcobj.cluster_objects[j];

					std::vector<flatbuffers::Offset<SquareBox::DSUGWOM::PairOfVec2>> vec_of_edges;
					for (unsigned int k = 0; k < ccobj->vec_of_edges.size(); k++)
					{
						const std::pair<glm::vec2, glm::vec2> & pairOfVec2 = ccobj->vec_of_edges[i];
						auto one = SquareBox::DSUGWOM::Vec2(pairOfVec2.first.x, pairOfVec2.first.y);
						auto two = SquareBox::DSUGWOM::Vec2(pairOfVec2.second.x, pairOfVec2.second.y);
						vec_of_edges.push_back(SquareBox::DSUGWOM::CreatePairOfVec2(
							builder,
							&one,
							&two));
					}

					std::vector<const SquareBox::DSUGWOM::Vec2*> vec_of_vertices;
					for (unsigned int k = 0; k < ccobj->vertices.size(); k++)
					{
						auto current_vertex = SquareBox::DSUGWOM::Vec2(ccobj->vertices[k].x, ccobj->vertices[k].y);
						vec_of_vertices.push_back(&current_vertex);
					}
					std::vector<flatbuffers::Offset<SquareBox::DSUGWOM::Joint>> vec_of_joints;
					for (unsigned int k = 0; k < ccobj->joints.size(); k++)
					{
						const SquareBox::GWOM::Joint * current_joint = ccobj->joints[k];

						std::vector<const SquareBox::DSUGWOM::Vec2*> vec_of_joint_points;
						for (unsigned int l = 0; l < current_joint->vec_of_points.size(); l++)
						{
							auto current_tmp_point = SquareBox::DSUGWOM::Vec2(current_joint->vec_of_points[l].x, current_joint->vec_of_points[l].y);
							vec_of_joint_points.push_back(&current_tmp_point);
						}

						std::pair<int, int> bodyACoords = current_joint->body_a_coordinates;
						if (bodyACoords.first == ccobj->cluster_index &&bodyACoords.second == ccobj->index) {
							//Only Store when this is body A
						
							
							auto tmp_body_A_cords = SquareBox::DSUGWOM::CreatePairOfInt(builder,current_joint->body_a_coordinates.first, current_joint->body_a_coordinates.second);
							auto tmp_body_B_cords = SquareBox::DSUGWOM::CreatePairOfInt(builder, current_joint->body_b_coordinates.first, current_joint->body_b_coordinates.second);
							auto tmp_joint_color = SquareBox::DSUGWOM::Ivec4(current_joint->color.x, current_joint->color.y, current_joint->color.z, current_joint->color.w);
							auto tmp_joint_ground_Anchor_A = SquareBox::DSUGWOM::Vec2(current_joint->ground_anchor_a.x, current_joint->ground_anchor_a.y);
							auto tmp_joint_ground_Anchor_B = SquareBox::DSUGWOM::Vec2(current_joint->ground_anchor_b.x, current_joint->ground_anchor_b.y);
							auto tmp_joint_local_Anchor_A = SquareBox::DSUGWOM::Vec2(current_joint->local_anchor_a.x, current_joint->local_anchor_a.y);
							auto tmp_joint_local_Anchor_B = SquareBox::DSUGWOM::Vec2(current_joint->local_anchor_b.x, current_joint->local_anchor_b.y);
							auto tmp_joint_local_Anchor = SquareBox::DSUGWOM::Vec2(current_joint->local_axis_a.x, current_joint->local_anchor_a.y);

							vec_of_joints.push_back(SquareBox::DSUGWOM::CreateJoint(
								builder,
								tmp_body_A_cords,
							   	tmp_body_B_cords,
								current_joint->collide_connected_bodies,
								current_joint->is_joint_alive,
								current_joint->reference_angle,
								current_joint->frequency_hz,
								current_joint->damping_ratio,
								current_joint->max_length,
								current_joint->min_length,
								current_joint->length,
								current_joint->length_a,
								current_joint->length_b,
								current_joint->lower_angle,
								current_joint->lower_translation,
								current_joint->upper_angle,
								current_joint->upper_translation,
								current_joint->max_motor_torque,
								current_joint->max_motor_force,
								current_joint->motor_speed,
								current_joint->enable_motor,
								current_joint->enable_limit,
								&tmp_joint_color,
								current_joint->thickness,
								current_joint->pulley_ratio,
								current_joint->stiffness,
								current_joint->damping,
								&tmp_joint_ground_Anchor_A,
								&tmp_joint_ground_Anchor_B,
								&tmp_joint_local_Anchor_A,
								&tmp_joint_local_Anchor_B,
								builder.CreateVector(vec_of_joint_points),
								&tmp_joint_local_Anchor,
								static_cast<int>(current_joint->joint_type)
							));
						}
					}

					auto tmp_ccobj_offset = SquareBox::DSUGWOM::Vec2(ccobj->off_set.x, ccobj->off_set.y);
					auto tmp_ccobj_position = SquareBox::DSUGWOM::Vec2(ccobj->position.x, ccobj->position.y);
					auto tmp_ccobj_init_linear_velocity = SquareBox::DSUGWOM::Vec2(ccobj->init_linear_velocity.x, ccobj->init_linear_velocity.y);
					auto tmp_ccobj_direction = SquareBox::DSUGWOM::Vec2(ccobj->direction.x, ccobj->direction.y);
					auto tmp_ccobj_color = SquareBox::DSUGWOM::Ivec4(ccobj->color.x, ccobj->color.y, ccobj->color.z, ccobj->color.w);
					auto tmp_texture_info_uv_rect = SquareBox::DSUGWOM::Vec4(ccobj->texture_info.uv_rect.x, ccobj->texture_info.uv_rect.y, ccobj->texture_info.uv_rect.z, ccobj->texture_info.uv_rect.w);

					//we have to covert each Cluster object to a flatBuffer Cluster Object
					vec_of_fb_cluster_objects.push_back(SquareBox::DSUGWOM::CreateClusterObject(builder,
						ccobj->layer_index,
						ccobj->cluster_index,
						ccobj->index, ccobj->deletion_index,
						ccobj->life_span, 
						ccobj->group_index, static_cast<int>(ccobj->family),
						builder.CreateVector(ccobj->vec_of_mask_bits),
						builder.CreateString(std::string(ccobj->name)),
						ccobj->height,
						ccobj->width,
						ccobj->radius,
						ccobj->density,
						ccobj->friction,
						ccobj->restitution,
						ccobj->angle,
						ccobj->init_angular_velocity,
						ccobj->frequency,
						ccobj->linear_damping,
						ccobj->angular_damping,
						ccobj->gravity_scale,
						ccobj->resistivity,
						ccobj->is_first_hovered,
						ccobj->is_last_hovered,
						ccobj->is_first_selected,
						ccobj->is_last_selected,
						ccobj->is_hidden,
						ccobj->is_fixed_rotation,
						ccobj->is_alive,
						ccobj->allow_sleep,
						ccobj->awake,
						ccobj->is_bullet,
						ccobj->requesting_recreation,
						static_cast<int>(ccobj->controller_type),
						static_cast<int>(ccobj->shape),
						static_cast<int>(ccobj->type),
						builder.CreateVector(vec_of_edges),
						builder.CreateVector(vec_of_vertices),
						builder.CreateVector(vec_of_joints),
						&tmp_ccobj_offset,
						&tmp_ccobj_position,
						&tmp_ccobj_init_linear_velocity,
						&tmp_ccobj_direction,
						ccobj->speed,
						ccobj->physics_properties != nullptr ? 1 : 0,
						&tmp_ccobj_color,
						SquareBox::DSUGWOM::CreateTextureInfo(builder,static_cast<int>(ccobj->texture_info.texture_type),ccobj->texture_info.texture_index,ccobj->texture_info.tile_sheet_index,ccobj->texture_info.texture_id,&tmp_texture_info_uv_rect)
					));
				}
				vec_of_fb_world_clusters.push_back(SquareBox::DSUGWOM::CreateWorldCluster(builder, cwcobj.index, cwcobj.deletionIndex, builder.CreateString(std::string(cwcobj.name)), cwcobj.isSelected, builder.CreateVector(vec_of_fb_cluster_objects)));
			}
			
			//Serializing the layers now
			std::vector<flatbuffers::Offset<SquareBox::DSUGWOM::Layer>> fb_layers;
			for (unsigned int i = 0; i < layers_.size(); i++) {
				const SquareBox::GWOM::Layer& layer = layers_[i];
				auto fb_layer_name = builder.CreateString(layer.name);
				
				//alive_cluster_objects
				std::vector< flatbuffers::Offset<SquareBox::DSUGWOM::PairOfInt>> fb_alive_cluster_objects;
				for (unsigned int k = 0; k < layer.alive_cluster_objects.size(); k++)
				{
					fb_alive_cluster_objects.push_back(SquareBox::DSUGWOM::CreatePairOfInt(builder, layer.alive_cluster_objects[k].first, layer.alive_cluster_objects[k].second));
				}
				
				//tiling system data
				std::vector<flatbuffers::Offset<SquareBox::DSUGWOM::VectorOfInt>> fb_tile_system_data;
				const auto  & tiled_system_data = layer.tile_system.m_tiles;
				for (unsigned int k = 0; k < tiled_system_data.size(); k++)
				{
					std::vector<int> fb_single_tile_system_row;
					for (unsigned int l = 0; l < tiled_system_data[k].size(); l++)
					{
						fb_single_tile_system_row.push_back(tiled_system_data[k][l].key);
					}
					fb_tile_system_data.push_back(SquareBox::DSUGWOM::CreateVectorOfInt(builder, builder.CreateVector(fb_single_tile_system_row)));
				}

				//Textures
				//Singles
				std::vector<flatbuffers::Offset<SquareBox::DSUGWOM::ParentTexture>> fb_single_textures;
				for (unsigned int k = 0; k < layer.single_textures.size(); k++)
				{
					const SquareBox::GWOM::ParentTexture& current_single_parent_texture = layer.single_textures[k];
					auto current_single_texture_tiling_vec2 = SquareBox::DSUGWOM::Ivec2(current_single_parent_texture.texture.tiling.x, current_single_parent_texture.texture.tiling.y);
					fb_single_textures.push_back(SquareBox::DSUGWOM::CreateParentTexture(builder,current_single_parent_texture.texture_index,SquareBox::DSUGWOM::CreateGLTexture(builder,builder.CreateString(current_single_parent_texture.texture.asset_file_path),builder.CreateString(current_single_parent_texture.texture.display_name), current_single_parent_texture.texture.height, current_single_parent_texture.texture.width,&current_single_texture_tiling_vec2)));
				}

				//Tiled 
				std::vector<flatbuffers::Offset<SquareBox::DSUGWOM::ParentTexture>> fb_tiled_textures;
				for (unsigned int k = 0; k < layer.tiled_textures.size(); k++)
				{
					const SquareBox::GWOM::ParentTexture& current_tiled_parent_texture = layer.tiled_textures[k];
					auto current_tiled_texture_tiling_vec2 = SquareBox::DSUGWOM::Ivec2(current_tiled_parent_texture.texture.tiling.x, current_tiled_parent_texture.texture.tiling.y);
					fb_tiled_textures.push_back(SquareBox::DSUGWOM::CreateParentTexture(builder, current_tiled_parent_texture.texture_index, SquareBox::DSUGWOM::CreateGLTexture(builder, builder.CreateString(current_tiled_parent_texture.texture.asset_file_path), builder.CreateString(current_tiled_parent_texture.texture.display_name), current_tiled_parent_texture.texture.height, current_tiled_parent_texture.texture.width,&current_tiled_texture_tiling_vec2)));
				}

				//Sub texture
				std::vector<flatbuffers::Offset<SquareBox::DSUGWOM::SubTexture>> fb_sub_textures;
				//only save the tiling data for a tiled layer, if speed is key
			//	if (layer.tiling != SquareBox::LayerTilingEnum::None) {

				for (auto sub = layer.sub_textures_table.begin(); sub != layer.sub_textures_table.end(); sub++)
				{
					fb_sub_textures.push_back(SquareBox::DSUGWOM::CreateSubTexture(builder, static_cast<int>((*sub).second.parent_type), (*sub).second.sub_texture_key, (*sub).second.parent_texture_index, (*sub).second.tiling_index));
				}
				//}
				
				auto fb_layer = SquareBox::DSUGWOM::CreateLayer(builder,builder.CreateString(layer.name),layer.opacity,layer.is_visible,layer.is_locked,static_cast<int>(layer.tile_system.getTiling()),builder.CreateVector(fb_single_textures),builder.CreateVector(fb_tiled_textures),builder.CreateVector(fb_sub_textures),layer.tile_system.getOriginX(),layer.tile_system.getOriginY(),layer.tile_system.getWidth(),layer.tile_system.getHeight(),layer.tile_system.getTileSize(),builder.CreateVector(fb_tile_system_data),builder.CreateVector(fb_alive_cluster_objects));
			
				fb_layers.push_back(fb_layer);
			}
			auto fb_saved_ready_layers = builder.CreateVector(fb_layers);

			SquareBox::DSUGWOM::Vec2 fb_camera_position(active_camera_position_.x, active_camera_position_.y);
			

			auto complete_level = SquareBox::DSUGWOM::CreateSquareBoxLevel(builder, active_camera_scale_, &fb_camera_position, active_camera_index_, fb_saved_ready_layers, builder.CreateVector(vec_of_fb_world_clusters));
			builder.Finish(complete_level);

			uint8_t* buf = builder.GetCurrentBufferPointer();
			int size = builder.GetSize();

			return SquareBox::AssetManager::IOManager::saveRawDataToFile((const char *)buf, size, file_path_);
		}

		bool LevelReaderWriter::loadWorldAsBinary(const std::string& filePath, std::vector<WorldCluster>& vector_of_world_clusters_, std::vector<SquareBox::GWOM::Layer>& layers_, float & active_camera_scale_, glm::vec2& active_camera_position_, int  & active_camera_index_)
		{
			std::pair<char *, int> fileInfo = SquareBox::AssetManager::IOManager::getRawDataFromFile(filePath);
			//construct a buffer object that knows its size
			char *dataBuffer = new char[fileInfo.second];
			dataBuffer = fileInfo.first;

			auto complete_level = SquareBox::DSUGWOM::GetSquareBoxLevel(dataBuffer);

			//we have to loop through all the worldClusters and populate our end this side
			auto fb_world_clusters = complete_level->world_clusters();
			//resize the vector_of_world_clusters_ so that they are able to accomodate all the cluster Objects
			vector_of_world_clusters_.reserve(fb_world_clusters->size());

			for (unsigned int i = 0; i < fb_world_clusters->size(); i++)
			{
				auto current_fb_world_cluster = fb_world_clusters->Get(i);
				//for each  world cluster
				SquareBox::GWOM::WorldCluster world_cluster;
				world_cluster.index = current_fb_world_cluster->index();
				//world_cluster.cameraIndex = current_fb_world_cluster->camera_index();
				world_cluster.deletionIndex = current_fb_world_cluster->deletion_index();

				//name
				auto fb_name = current_fb_world_cluster->name()->str();
				for (unsigned int j = 0; j < fb_name.length(); j++)
				{
					world_cluster.name[j] = fb_name[j];
				}
				world_cluster.name[fb_name.length()] = '\0';
				world_cluster.isSelected = current_fb_world_cluster->is_selected();

				//now the cluster Objects
				auto current_fb_world_cluster_cluster_objects = current_fb_world_cluster->cluster_objects();

				//reserve enough space to accomodate all the cluster objects
				world_cluster.cluster_objects.reserve(current_fb_world_cluster_cluster_objects->size());

				for (unsigned int j = 0; j < current_fb_world_cluster_cluster_objects->size(); j++)
				{
					auto fb_cluster_object = current_fb_world_cluster_cluster_objects->Get(j);
					SquareBox::GWOM::ClusterObject cluster_object;
					//for each individual world Cluster
					cluster_object.layer_index = fb_cluster_object->layer_index();
					cluster_object.cluster_index = fb_cluster_object->cluster_index();
					cluster_object.index = fb_cluster_object->index();
					cluster_object.deletion_index = fb_cluster_object->deletion_index();
					cluster_object.life_span = fb_cluster_object->life_span();
					cluster_object.group_index = fb_cluster_object->group_index();

					cluster_object.family = static_cast<SquareBox::FamilyBitsEnum>(fb_cluster_object->family());

					auto fb_vec_of_maskBits = fb_cluster_object->vec_of_mask_bits();

					for (unsigned int k = 0; k < fb_vec_of_maskBits->size(); k++)
					{
						cluster_object.vec_of_mask_bits.push_back(fb_vec_of_maskBits->Get(k));
					}

					auto fb_name = fb_cluster_object->name()->str();
					for (unsigned int k = 0; k < fb_name.length(); k++)
					{
						cluster_object.name[k] = fb_name[k];
					}
					cluster_object.name[fb_name.length()] = '\0';

					cluster_object.height = fb_cluster_object->height();
					cluster_object.width = fb_cluster_object->width();
					cluster_object.radius = fb_cluster_object->radius();
					cluster_object.density = fb_cluster_object->density();
					cluster_object.friction = fb_cluster_object->friction();
					cluster_object.restitution = fb_cluster_object->restitution();
					cluster_object.angle = fb_cluster_object->angle();
					cluster_object.init_angular_velocity = fb_cluster_object->init_angular_velocity();
					cluster_object.frequency = fb_cluster_object->frequency();
					//damping Reduces world velocity on bodies
					cluster_object.linear_damping = fb_cluster_object->linear_damping();
					cluster_object.angular_damping = fb_cluster_object->angular_damping();
					cluster_object.gravity_scale = fb_cluster_object->gravity_scale();
					cluster_object.resistivity = fb_cluster_object->resistivity();

					/*bools*/
					//Hovering Edge and Chains ends
					cluster_object.is_first_hovered = fb_cluster_object->is_first_hovered();
					cluster_object.is_last_hovered = fb_cluster_object->is_last_hovered();

					//Selecting Edge and Chain Ends
					cluster_object.is_first_selected = fb_cluster_object->is_first_selected();
					cluster_object.is_last_selected = fb_cluster_object->is_last_selected();
					cluster_object.is_hidden = fb_cluster_object->is_hidden();
					cluster_object.is_fixed_rotation = fb_cluster_object->is_fixed_rotation();
					cluster_object.is_alive = fb_cluster_object->is_alive();
					cluster_object.allow_sleep = fb_cluster_object->allow_sleep();
					cluster_object.awake = fb_cluster_object->awake();
					cluster_object.is_bullet = fb_cluster_object->is_bullet();
					cluster_object.requesting_recreation = fb_cluster_object->requesting_recreation();

					cluster_object.controller_type = static_cast<SquareBox::ControlledTypeEnum>(fb_cluster_object->controller_type());

					/*Enums*/
					cluster_object.shape = static_cast<SquareBox::BodyShapeEnum>(fb_cluster_object->shape());
					cluster_object.type = static_cast<SquareBox::BodyTypeEnum>(fb_cluster_object->type());

					/*glm::vecs*/

					auto fb_vec_of_edges = fb_cluster_object->vec_of_edges();

					for (unsigned int k = 0; k < fb_vec_of_edges->size(); k++)
					{
						auto test = fb_vec_of_edges->Get(k);
						auto first_fb_Vec2 = test->first();
						auto second_fb_Vec2 = test->second();

						glm::vec2 first_ccobj_vec2(first_fb_Vec2->x(), first_fb_Vec2->y());
						glm::vec2 second_ccobj_vec2(second_fb_Vec2->x(), second_fb_Vec2->y());

						std::pair<glm::vec2, glm::vec2> pairOfVec3 = std::pair<glm::vec2, glm::vec2>(first_ccobj_vec2, second_ccobj_vec2);
						cluster_object.vec_of_edges.push_back(pairOfVec3);
					}

					auto fb_vertices = fb_cluster_object->vertices();
					for (unsigned int k = 0; k < fb_vertices->size(); k++)
					{
						auto fb_vec2 = fb_vertices->Get(k);

						cluster_object.vertices.push_back(glm::vec2(fb_vec2->x(), fb_vec2->y()));
					}

					auto fb_joints = fb_cluster_object->joints();
					for (unsigned int k = 0; k < fb_joints->size(); k++)
					{
						auto fb_joint = fb_joints->Get(k);
						//take care of each of the joints attributes

						SquareBox::GWOM::Joint *jointMemory = new SquareBox::GWOM::Joint;

						jointMemory->body_a_coordinates = std::pair<int, int>(fb_joint->body_a_coordinates()->first(), fb_joint->body_a_coordinates()->second());
						jointMemory->body_b_coordinates = std::pair<int, int>(fb_joint->body_b_coordinates()->first(), fb_joint->body_b_coordinates()->second());
						jointMemory->collide_connected_bodies = fb_joint->collide_connected_bodies();
						jointMemory->is_joint_alive = fb_joint->is_joint_alive();
						jointMemory->reference_angle = fb_joint->reference_angle();
						jointMemory->frequency_hz = fb_joint->frequency_hz();
						jointMemory->damping_ratio = fb_joint->damping_ratio();
						jointMemory->max_length = fb_joint->max_length();
						jointMemory->min_length = fb_joint->min_length();
						jointMemory->length = fb_joint->length();
						jointMemory->length_a = fb_joint->length_a();
						jointMemory->length_b = fb_joint->length_b();
						jointMemory->lower_angle = fb_joint->lower_angle();
						jointMemory->lower_translation = fb_joint->lower_translation();
						jointMemory->upper_angle = fb_joint->upper_angle();
						jointMemory->upper_translation = fb_joint->upper_translation();
						jointMemory->max_motor_torque = fb_joint->max_motor_torque();
						jointMemory->max_motor_force = fb_joint->max_motor_force();
						jointMemory->motor_speed = fb_joint->motor_speed();
						jointMemory->enable_motor = fb_joint->enable_motor();
						jointMemory->enable_limit = fb_joint->enable_limit();

						jointMemory->color = glm::vec4(fb_joint->color()->x(), fb_joint->color()->y(), fb_joint->color()->z(), fb_joint->color()->w());
						jointMemory->thickness = fb_joint->thickness();
						jointMemory->pulley_ratio = fb_joint->pulley_ratio();
						jointMemory->stiffness = fb_joint->stiffness();
						jointMemory->damping = fb_joint->damping();
						jointMemory->ground_anchor_a = glm::vec2(fb_joint->ground_anchor_a()->x(), fb_joint->ground_anchor_a()->y());
						jointMemory->ground_anchor_b = glm::vec2(fb_joint->ground_anchor_b()->x(), fb_joint->ground_anchor_b()->y());
						jointMemory->local_anchor_a = glm::vec2(fb_joint->local_anchor_a()->x(), fb_joint->local_anchor_a()->y());
						jointMemory->local_anchor_b = glm::vec2(fb_joint->local_anchor_b()->x(), fb_joint->local_anchor_b()->y());

						auto fb_vec_of_points = fb_joint->vec_of_points();
						for (unsigned int l = 0; l < fb_vec_of_points->size(); l++)
						{
							auto fb_point = fb_vec_of_points->Get(l);
							jointMemory->vec_of_points.push_back(glm::vec2(fb_point->x(), fb_point->y()));
						}

						jointMemory->local_axis_a = glm::vec2(fb_joint->local_axis_a()->x(), fb_joint->local_axis_a()->y());
						jointMemory->joint_type = static_cast<SquareBox::JointTypeEnum>(fb_joint->joint_type());
						jointMemory->is_joint_alive = false;
						cluster_object.joints.push_back(jointMemory);
					}

					cluster_object.off_set = glm::vec2(fb_cluster_object->off_set()->x(), fb_cluster_object->off_set()->y());
					cluster_object.position = glm::vec2(fb_cluster_object->position()->x(), fb_cluster_object->position()->y());
					cluster_object.init_linear_velocity = glm::vec2(fb_cluster_object->init_linear_velocity()->x(), fb_cluster_object->init_linear_velocity()->y());
					cluster_object.direction = glm::vec2(fb_cluster_object->direction()->x(), fb_cluster_object->direction()->y());
					cluster_object.speed = fb_cluster_object->speed();
					//vec4
					/*Externals*/

					cluster_object.physics_properties = fb_cluster_object->physics_properties() == 1 ? new SquareBox::PhysicsCollisionEngine::PhysicsProperties() : nullptr;
					cluster_object.color = glm::vec4(fb_cluster_object->color()->x(), fb_cluster_object->color()->y(), fb_cluster_object->color()->z(), fb_cluster_object->color()->w());;

					auto fb_texture_info = fb_cluster_object->texture_info();
					cluster_object.texture_info.texture_type = static_cast<SquareBox::TextureEnum>(fb_texture_info->texture_type());
					cluster_object.texture_info.texture_index = fb_texture_info->texture_index();
					cluster_object.texture_info.tile_sheet_index = fb_texture_info->tile_sheet_index();
					cluster_object.texture_info.texture_id = fb_texture_info->texture_id();
					cluster_object.texture_info.uv_rect = glm::vec4(fb_texture_info->uv_rect()->x(), fb_texture_info->uv_rect()->y(), fb_texture_info->uv_rect()->x(), fb_texture_info->uv_rect()->w());
					world_cluster.cluster_objects.push_back(cluster_object);
				}
				vector_of_world_clusters_.push_back(world_cluster);
			}
			
			//the layers
			auto fb_layers = complete_level->layers();
			for (unsigned int i = 0; i<fb_layers->size(); i++) {
				auto fb_layer = fb_layers->Get(i);
				SquareBox::GWOM::Layer layer;

			
				//layer name
				auto fb_layer_name = fb_layer->name()->str();
				for (unsigned int j = 0; j < fb_layer_name.length(); j++)
				{
					layer.name[j] = fb_layer_name[j];
				}
				layer.name[fb_layer_name.length()] = '\0';
				layer.opacity = fb_layer->opacity();
				layer.is_visible = fb_layer->is_visible();
				layer.is_locked = fb_layer->is_locked();
				//getting the alive cluster objects back
				//Textures
				//Singles
				auto fb_single_textures = fb_layer->single_textures();

				for (unsigned int j = 0; j < fb_single_textures->size(); j++)
				{
					auto fb_single_texture = fb_single_textures->Get(j);
					SquareBox::GWOM::ParentTexture tmp_parent_texture;
					tmp_parent_texture.texture_index = fb_single_texture->texture_index();
					tmp_parent_texture.texture.asset_file_path = fb_single_texture->texture()->assets_file_path()->str();
					tmp_parent_texture.texture.display_name = fb_single_texture->texture()->display_name()->str();
					tmp_parent_texture.texture.height = fb_single_texture->texture()->heigth();
					tmp_parent_texture.texture.width = fb_single_texture->texture()->width();
					tmp_parent_texture.texture.tiling = glm::vec2(0.0f, 0.0f);
					layer.single_textures.push_back(tmp_parent_texture);
				}

				//Tiled
				auto fb_tiled_textures = fb_layer->tiled_textures();

				for (unsigned int j = 0; j < fb_tiled_textures->size(); j++)
				{
					auto fb_tiled_texture = fb_tiled_textures->Get(j);
					SquareBox::GWOM::ParentTexture tmp_parent_texture;
					tmp_parent_texture.texture_index = fb_tiled_texture->texture_index();
					tmp_parent_texture.texture.asset_file_path = fb_tiled_texture->texture()->assets_file_path()->str();
					tmp_parent_texture.texture.display_name = fb_tiled_texture->texture()->display_name()->str();
					tmp_parent_texture.texture.height = fb_tiled_texture->texture()->heigth();
					tmp_parent_texture.texture.width = fb_tiled_texture->texture()->width();
					tmp_parent_texture.texture.tiling = glm::ivec2(fb_tiled_texture->texture()->tiling()->x(), fb_tiled_texture->texture()->tiling()->y());
					layer.tiled_textures.push_back(tmp_parent_texture);
				}

				//We need to construct the sub_textures manually
				auto fb_sub_textures = fb_layer->sub_textures();
				for (unsigned int j = 0; j < fb_sub_textures->size(); j++)
				{
					auto sub = fb_sub_textures->Get(j);
					SquareBox::GWOM::SubTexture tmp_sub_texture;
					tmp_sub_texture.parent_texture_index = sub->parent_texture_index();
					tmp_sub_texture.parent_type = static_cast<SquareBox::TextureEnum>(sub->parent_type());
					tmp_sub_texture.sub_texture_key = sub->sub_texture_key();
					tmp_sub_texture.tiling_index = sub->tiling_index();
					layer.sub_textures_table[tmp_sub_texture.sub_texture_key] = tmp_sub_texture;
				}

				std::vector<std::vector<int>> tile_system_data_vec;
				auto fb_layer_tile_system_data = fb_layer->tile_system_data();
				for (unsigned j = 0; j < fb_layer_tile_system_data->size(); j++)
				{
					auto fb_layer_tile_row = fb_layer_tile_system_data->Get(j)->data();
					std::vector<int> tile_system_row;
					for (unsigned int k = 0; k < fb_layer_tile_row->size(); k++) {

						tile_system_row.push_back(fb_layer_tile_row->Get(k));
					}
					tile_system_data_vec.push_back(tile_system_row);
				}

				//initiliase the tile system while Adpoting the layer data
				layer.tile_system.init(fb_layer->tile_system_orign_x(), fb_layer->tile_system_orign_y(), fb_layer->tile_system_width(), fb_layer->tile_system_height(), fb_layer->tile_system_tile_size(), static_cast<SquareBox::LayerTilingEnum>(fb_layer->tiling()),  tile_system_data_vec);

				auto fb_layer_alive_cluster_objects = fb_layer->alive_cluster_objects();
				for (unsigned int j = 0; j < fb_layer_alive_cluster_objects->size(); j++)
				{
					auto fb_acobj_coordinate = fb_layer_alive_cluster_objects->Get(j);
					layer.alive_cluster_objects.push_back(std::pair<int, int>(
						fb_acobj_coordinate->first(),
						fb_acobj_coordinate->second()
						));
				}

				layers_.push_back(layer);
			}


			//the rest of the data
			active_camera_index_ = complete_level->active_camera_index();
			active_camera_scale_ = complete_level->camera_scale();
			auto fb_active_camera_position = complete_level->camera_position();
			active_camera_position_.x = fb_active_camera_position->x();
			active_camera_position_.y = fb_active_camera_position->y();

			
			delete[] dataBuffer;
			return true;
		}
	}
}