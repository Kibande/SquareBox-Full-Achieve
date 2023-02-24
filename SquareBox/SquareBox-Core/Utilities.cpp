#include "Utilities.h"
#include <Asset-Manager/Asset-Manager.h>
#include <MathLib/MathLib.h>
namespace SquareBox {

	/*
		the euality checker needs to be better fine tunned


	*/
	bool operator == (SquareBox::GWOM::Joint& lhs, SquareBox::GWOM::Joint& rhs) {
		return (
			lhs.collide_connected_bodies == rhs.collide_connected_bodies
			&&
			lhs.joint_type == rhs.joint_type
			&&
			lhs.body_a_coordinates == rhs.body_a_coordinates
			&&
			lhs.body_b_coordinates == rhs.body_b_coordinates
			&&
			lhs.enable_motor == rhs.enable_motor
			&&
			lhs.enable_limit == rhs.enable_limit
			&&

			SquareBox::MathLib::Float::areFloatsEqual(lhs.reference_angle, rhs.reference_angle)
			&&
			SquareBox::MathLib::Float::areFloatsEqual(lhs.frequency_hz, rhs.frequency_hz)
			&&
			SquareBox::MathLib::Float::areFloatsEqual(lhs.damping_ratio, rhs.damping_ratio)

			&&
			SquareBox::MathLib::Float::areFloatsEqual(lhs.max_length, rhs.max_length)
			&&
			SquareBox::MathLib::Float::areFloatsEqual(lhs.min_length, rhs.min_length)
			&&
			SquareBox::MathLib::Float::areFloatsEqual(lhs.length, rhs.length)

			&&
			SquareBox::MathLib::Float::areFloatsEqual(lhs.length_a, rhs.length_a)
			&&
			SquareBox::MathLib::Float::areFloatsEqual(lhs.length_b, rhs.length_b)
			&&
			SquareBox::MathLib::Float::areFloatsEqual(lhs.lower_angle, rhs.lower_angle)

			&&
			SquareBox::MathLib::Float::areFloatsEqual(lhs.max_motor_torque, rhs.max_motor_torque)
			&&
			SquareBox::MathLib::Float::areFloatsEqual(lhs.max_motor_force, rhs.max_motor_force)

			&&
			SquareBox::MathLib::Float::areFloatsEqual(lhs.motor_speed, rhs.motor_speed)
			&&
			SquareBox::MathLib::Float::areFloatsEqual(lhs.thickness, rhs.thickness)
			&&
			SquareBox::MathLib::Float::areFloatsEqual(lhs.pulley_ratio, rhs.pulley_ratio)

			&&
			SquareBox::MathLib::Float::areFloatsEqual(lhs.stiffness, rhs.stiffness)
			&&
			SquareBox::MathLib::Float::areFloatsEqual(lhs.damping, rhs.damping)
			&&
			SquareBox::MathLib::Float::areFloatsEqual(lhs.local_axis_a.x, rhs.local_axis_a.x)

			&&
			SquareBox::MathLib::Float::areFloatsEqual(lhs.local_anchor_a.x, rhs.local_anchor_a.x)
			&&
			SquareBox::MathLib::Float::areFloatsEqual(lhs.local_anchor_a.y, rhs.local_anchor_a.y)
			&&
			SquareBox::MathLib::Float::areFloatsEqual(lhs.local_anchor_b.x, rhs.local_anchor_b.x)
			&&
			SquareBox::MathLib::Float::areFloatsEqual(lhs.local_anchor_b.y, rhs.local_anchor_b.y)

			&&
			SquareBox::MathLib::Float::areFloatsEqual(lhs.ground_anchor_a.x, rhs.ground_anchor_a.x)
			&&
			SquareBox::MathLib::Float::areFloatsEqual(lhs.ground_anchor_a.y, rhs.ground_anchor_a.y)
			&&
			SquareBox::MathLib::Float::areFloatsEqual(lhs.ground_anchor_b.x, rhs.ground_anchor_b.x)
			&&
			SquareBox::MathLib::Float::areFloatsEqual(lhs.ground_anchor_b.y, rhs.ground_anchor_b.y)
			);
	}

	Utilities::Utilities()
	{
	}

	Utilities::~Utilities()
	{
	}

	void Utilities::init() {
		//populate our shapes class
		m_vec_shapes_pointer.push_back(new SquareBox::Shapes::BoxShape());
		m_vec_shapes_pointer.push_back(new SquareBox::Shapes::CircleShape());
		m_vec_shapes_pointer.push_back(new SquareBox::Shapes::PolygonShape());
		m_vec_shapes_pointer.push_back(new SquareBox::Shapes::EdgeShape());
		m_vec_shapes_pointer.push_back(new SquareBox::Shapes::ChainShape());
		m_vec_shapes_pointer.push_back(new SquareBox::Shapes::CapsuleShape());
		m_vec_shapes_pointer.push_back(new SquareBox::Shapes::TopCapsuleShape());
		m_vec_shapes_pointer.push_back(new SquareBox::Shapes::PointLightShape());
		m_vec_shapes_pointer.push_back(new SquareBox::Shapes::CalculatedLightShape());
	}

	glm::vec2 Utilities::compute2DPolygonCentroid(const std::vector<glm::vec2>&  vertices_)
	{
		//https://stackoverflow.com/questions/2792443/finding-the-centroid-of-a-polygon

		int vertexCount = vertices_.size();
		glm::vec2 centroid = { 0, 0 };
		double signedArea = 0.0;
		double x0 = 0.0; // Current vertex X
		double y0 = 0.0; // Current vertex Y
		double x1 = 0.0; // Next vertex X
		double y1 = 0.0; // Next vertex Y
		double a = 0.0;  // Partial signed area

		// For all vertices except last
		int i = 0;
		for (i = 0; i < vertexCount - 1; ++i)
		{
			x0 = vertices_[i].x;
			y0 = vertices_[i].y;
			x1 = vertices_[i + 1].x;
			y1 = vertices_[i + 1].y;
			a = x0 * y1 - x1 * y0;
			signedArea += a;
			centroid.x += (x0 + x1)*a;
			centroid.y += (y0 + y1)*a;
		}

		// Do last vertex separately to avoid performing an expensive
		// modulus operation in each iteration.
		x0 = vertices_[i].x;
		y0 = vertices_[i].y;
		x1 = vertices_[0].x;
		y1 = vertices_[0].y;
		a = x0 * y1 - x1 * y0;
		signedArea += a;
		centroid.x += (x0 + x1)*a;
		centroid.y += (y0 + y1)*a;

		signedArea *= 0.5;
		centroid.x /= (6.0*signedArea);
		centroid.y /= (6.0*signedArea);

		return centroid;
	}
	void Utilities::rotatePoint(glm::vec2 & position, float angle)
	{
		position.x = position.x * cos(angle) - position.y * sin(angle);
		position.y = position.x * sin(angle) + position.y * cos(angle);
	}
	void Utilities::addIntToVector(std::vector<int> &vector_, int num_, bool allowdups_)
	{
		if (allowdups_) {
			vector_.push_back(num_);
		}
		else if (!isIntVectorMember(vector_, num_)) {
			vector_.push_back(num_);
		}
	}

	void Utilities::removeIntFromVector(std::vector<int> &vector_, int num_, bool removedups_)
	{
		if (isIntVectorMember(vector_, num_)) {
			//get the index then erase
			for (auto it = vector_.begin(); it != vector_.end();)
			{
				bool erased = false;
				if ((*it) == num_) {
					it = vector_.erase(it);
					if (!removedups_) {
						break;
					}
					erased = true;
				}
				if (!erased) {
					++it;
				}
			}
		}
	}

	void Utilities::removePairFromVector(std::vector<std::pair<int, int>>& vector_, std::pair<int, int> target_, bool removedups_)
	{
		if (isPairVectorMember(vector_, target_)) {
			for (auto it = vector_.begin(); it != vector_.end();)
			{

				bool erased = false;
				//remove if its the one we are looking for
				if ((*it) == target_) {
					it = vector_.erase(it);
					if (!removedups_) {
						break;
					}
					erased = true;
				}
				if (!erased) {
					++it;
				}
			}
		}
	}

	bool Utilities::isIntVectorMember(std::vector<int> &vector_, int num_)
	{
		for (unsigned int i = 0; i < vector_.size(); i++)
		{
			if (vector_[i] == num_) {
				return true;
			}
		}
		return false;
	}


	void Utilities::addPairToVector(std::vector<std::pair<int, int>>& vector_, std::pair<int, int> pair_, bool allowdups_)
	{
		if (allowdups_) {
			vector_.push_back(pair_);
		}
		else if (!isPairVectorMember(vector_, pair_)) {
			vector_.push_back(pair_);
		}
	}

	void Utilities::movePairInVector(std::vector<std::pair<int, int>>& vector_, std::pair<int, int> pair_, int movementDisplacement)
	{
		for (unsigned int i = 0; i < vector_.size(); i++)
		{
			if (vector_[i] == pair_) {
				//we have got our client pair's index
				unsigned int targetIndex = i + movementDisplacement;

				if (targetIndex < vector_.size()) {
					std::pair<int, int> tempPair_ = std::pair<int, int>(vector_[targetIndex]);
					//the swap
					vector_[targetIndex].first = pair_.first;
					vector_[targetIndex].second = pair_.second;

					vector_[i].first = tempPair_.first;
					vector_[i].second = tempPair_.second;
				}
			}
		}
	}

	bool Utilities::isPairVectorMember(std::vector<std::pair<int, int>>& vector_, std::pair<int, int> pair_)
	{
		for (unsigned int i = 0; i < vector_.size(); i++)
		{
			if (vector_[i] == pair_) {
				return true;
			}
		}
		return false;
	}

	void Utilities::addJointToLayerAliveJointsMap(SquareBox::GWOM::Layer& layer_, SquareBox::GWOM::Joint& joint_)
	{
		const std::map<std::pair<int, int>, std::vector<SquareBox::GWOM::Joint>>::iterator& response = layer_.active_joints_body_a_map.find(joint_.body_a_coordinates);
		if (response == layer_.active_joints_body_a_map.end()) {
			//not found so add it
			layer_.active_joints_body_a_map[joint_.body_a_coordinates] = std::vector<SquareBox::GWOM::Joint>();
		}
		const std::map<std::pair<int, int>, std::vector<SquareBox::GWOM::Joint>>::iterator& cleark = layer_.active_joints_body_a_map.find(joint_.body_a_coordinates);

		// we did not check for duplicates ....

		std::vector<SquareBox::GWOM::Joint> & target_vec = (*cleark).second;
		bool found_exact = false;
		for (std::vector<SquareBox::GWOM::Joint>::iterator it = target_vec.begin(); it != target_vec.end();it++)
		{
			//first equality check
			if ((*it)== joint_) {
				found_exact = true;
			}
		}

		if (found_exact) {
			SBX_ERROR("Tried to record the exact same Joint twice");
		}
		else {
			target_vec.push_back(joint_);
		}
	}

	void Utilities::removeJointFromLayerAliveJointsMap(SquareBox::GWOM::Layer& layer_, SquareBox::GWOM::Joint& joint_)
	{
		const std::map<std::pair<int, int>, std::vector<SquareBox::GWOM::Joint>>::iterator & cleark = layer_.active_joints_body_a_map.find(joint_.body_a_coordinates);
		if (cleark == layer_.active_joints_body_a_map.end()) {
			//not found 
			SBX_CORE_ERROR("Tried to delete a joint that was never rcorded");
		}
		else {
			std::vector<SquareBox::GWOM::Joint>& target_vec = (*cleark).second;
			bool found_exact = false;
			for (std::vector<SquareBox::GWOM::Joint>::iterator  it = target_vec.begin(); it != target_vec.end();it++)
			{
				//first equality check
				if ((*it) == joint_) {
					target_vec.erase(it);
					break;
				}
			}
			
		}
	}

	void Utilities::worldIndiciesCleanUp(std::vector<SquareBox::GWOM::Layer> & layers_)
	{
		for (unsigned int i = 0; i < layers_.size(); i++)
		{
			if (layers_[i].alive_cluster_objects.size() > 0) {
				SBX_CRITICAL("Layer indicies clean up is not yet implemented");
		//	__debugbreak();
			}
		}
	}

	void Utilities::guiLayerCleanUp(SquareBox::GWOM::GuiLayer& gui_layer_)
	{
		//clean up the texures and fonts 
		// since this is not yet implemented we shall only be using textures and fonts that are coming from the
		// Assets folder
		/*
			 implement copying textures and fonts from where every they are to the Assets folder
		*/
	}

	//{
	//	std::vector<std::string> screen_categories;
	//	screen_categories.push_back("Idpi/");	//	120dpi
	//	screen_categories.push_back("mdpi/");	//	160dpi
	//	screen_categories.push_back("hdpi/");	//	240dpi
	//	screen_categories.push_back("xhdpi/");	//	320dpi
	//	screen_categories.push_back("xxhdpi/");	//	48dpi
	//	screen_categories.push_back("xxxhdpi/");	//	640dpi
	//	screen_categories.push_back("nodpi/");	//   

	//	/*  the way this works is by changing the index value kept by the worldCluster or by the clusterObject and not its real physics address 
	//	so this cleanup should only be done before storage because it disyncronises the index with the actual positioning of the clusterObject or worldCluster in out world
	//	deleting the elements is an option if we wanted to make this more reusbale but thats and advancement i will maybe add when i get more time on my hands
	//	*/
	//	std::vector<SquareBox::GWOM::WorldCluster> cleanUpWorldClusters;
	//	int world_cluster_index = 0;
	//	for (unsigned int i = 0; i < world_clusters_.size(); i++)
	//	{
	//		bool changedWorldClusterindex = false;
	//		SquareBox::GWOM::WorldCluster& cwc = world_clusters_[i];
	//		if (cwc.index != world_cluster_index) {
	//			changedWorldClusterindex = true;
	//		}

	//		std::vector<SquareBox::GWOM::ClusterObject> cleanedUpClusterObjects;
	//		int cluster_object_index = 0;
	//		for (unsigned int j = 0; j < cwc.cluster_objects.size(); j++)
	//		{
	//			bool kept = false;//is this cluster object am the alive still 
	//			SquareBox::GWOM::ClusterObject& cobj = cwc.cluster_objects[j];

	//			const int oldWorldClusterIndex = cobj.cluster_index;
	//			const int oldClusterObjectIndex = cobj.index;

	//		

	//			cobj.cluster_index = world_cluster_index;// newWorldClusterIndex being set
	//			if (cobj.is_alive && cobj.life_span >= 1) {
	//				//keep it
	//				kept = true;
	//				//rename clusterObject if necessery
	//				std::string oldName = cobj.name;

	//				nameClusterObjectByIndex(cobj);
	//				std::string expectedName = cobj.name;

	//				//update clusterObject its index
	//				cobj.index = cluster_object_index;// newClusterObjectIndex being set
	//				nameClusterObjectByIndex(cobj);

	//				std::string newName = cobj.name;

	//				if (oldName != expectedName) {// we have a user assigned name so maintain it
	//					for (unsigned int si = 0; si < oldName.size(); si++)
	//					{
	//						cobj.name[si] = oldName[si];
	//					}
	//					cobj.name[oldName.size()] = '\0';
	//				}
	//				else {
	//					//we keep the new name
	//				}

	//				//keep
	//				cleanedUpClusterObjects.push_back(cobj);
	//				cluster_object_index++;//increment for the next
	//			}
	//			else {
	//				//to be skipped, but if it had a physics engine , clean that up
	//				if (cobj.physics_properties != nullptr) {
	//					delete cobj.physics_properties;
	//				}
	//			}

	//			const int newWorldClusterIndex = cobj.cluster_index;
	//			const int newClusterObjectIndex = cobj.index;

	//			if ((changedWorldClusterindex || oldClusterObjectIndex != cobj.index) && kept) {
	//				/*

	//					update the joints references
	//					the joints will have coordinates of (oldWorldClusterIndex,oldClusterObjectIndex),

	//					we should change that to (newWorldClusterIndex,newClusterObjectIndex)

	//					only the local joints actually keep the coordinates

	//				*/
	//				std::pair<int, int> wrongCobjCoordinates = std::pair<int, int>(oldWorldClusterIndex, oldClusterObjectIndex);
	//				std::pair<int, int> correctCobjCoordinates = std::pair<int, int>(newWorldClusterIndex, newClusterObjectIndex);
	//				//go through all my joints
	//				for (unsigned int k = 0; k < cobj.joints.size(); k++)
	//				{
	//					SquareBox::GWOM::Joint *currentJoint = cobj.joints[k];

	//					std::pair<int, int> currentjointBodyACoordinates = currentJoint->body_a_coordinates;
	//					std::pair<int, int> currentjointBodyBCoordinates = currentJoint->body_b_coordinates;

	//					bool i_am_bodyA = currentjointBodyACoordinates == wrongCobjCoordinates;

	//					SquareBox::GWOM::ClusterObject & currentJointBodyA = world_clusters_[currentjointBodyACoordinates.first].cluster_objects[currentjointBodyACoordinates.second];
	//					SquareBox::GWOM::ClusterObject & currentJointBodyB = world_clusters_[currentjointBodyBCoordinates.first].cluster_objects[currentjointBodyBCoordinates.second];

	//					if (i_am_bodyA) {
	//						//find joint in body B and edit its Coordinates
	//						for (unsigned int l = 0; l < currentJointBodyB.joints.size(); l++)
	//						{
	//							SquareBox::GWOM::Joint *bodyBCurrentJoint = currentJointBodyB.joints[l];
	//							if (bodyBCurrentJoint->body_a_coordinates == wrongCobjCoordinates && bodyBCurrentJoint->body_b_coordinates == currentjointBodyBCoordinates) {
	//								//this is our joint
	//								//make the correction
	//								bodyBCurrentJoint->body_a_coordinates = correctCobjCoordinates;
	//								break;
	//							}
	//						}
	//						//correct cobjs joint coordinates as well
	//						currentJoint->body_a_coordinates = correctCobjCoordinates;
	//					}
	//					else {
	//						//we are body B
	//						//find joint in body A and edit its Coordinates
	//						for (unsigned int l = 0; l < currentJointBodyA.joints.size(); l++)
	//						{
	//							SquareBox::GWOM::Joint *bodyACurrentJoint = currentJointBodyA.joints[l];
	//							if (bodyACurrentJoint->body_b_coordinates == wrongCobjCoordinates && bodyACurrentJoint->body_a_coordinates == currentjointBodyACoordinates) {
	//								//this is our joint
	//								//make the correction
	//								bodyACurrentJoint->body_b_coordinates = correctCobjCoordinates;
	//								break;
	//							}
	//						}
	//						//correct cobjs joint coordinates as well
	//						currentJoint->body_b_coordinates = correctCobjCoordinates;
	//					}
	//				}
	//			}


	//			
	//			//maintainig our layer system integrity
	//			if (kept) {
	//				//check the layer that has the old  coordinates and update them to these new co-ordinates

	//				std::pair<int, int> old_coordinates(oldWorldClusterIndex, oldClusterObjectIndex);
	//				std::pair<int, int> new_coordinates(newWorldClusterIndex, newClusterObjectIndex);
	//				bool found_match = false;
	//				for (unsigned k = 0; k < layers_.size(); k++)
	//				{
	//					SquareBox::GWOM::Layer& layer = layers_[k];

	//					for (unsigned l = 0; l < layer.alive_cluster_objects.size(); l++)
	//					{
	//						std::pair<int, int>& current_coordinates = layer.alive_cluster_objects[l];
	//						if (current_coordinates == old_coordinates) {
	//							//chanhe them to the new coordinates and then break out of this loop
	//							current_coordinates = new_coordinates;
	//							found_match = true;
	//							break;
	//						}
	//					}
	//				}

	//				if (!found_match)
	//				{
	//					SBX_CORE_ERROR("Failed to find a math to the old layer coordinates during clen up");
	//				}
	//			}
	//		}

	//		if (cleanedUpClusterObjects.size() == 0) {
	//			//skip it
	//		}
	//		else {
	//			//rename if necessary
	//			std::string oldName = cwc.name;

	//			nameWorldClusterByIndex(cwc);
	//			std::string expectedName = cwc.name;

	//			//update worldCluster its index
	//			cwc.index = world_cluster_index;
	//			nameWorldClusterByIndex(cwc);
	//			std::string newName = cwc.name;
	//			if (oldName != expectedName) {
	//				for (unsigned int si = 0; si < oldName.size(); si++)
	//				{
	//					cwc.name[si] = oldName[si];
	//				}
	//				cwc.name[oldName.size()] = '\0';
	//			}
	//			else {
	//				//we keep the new name
	//			}

	//			//update the cluster objects list
	//			cwc.cluster_objects = cleanedUpClusterObjects;

	//			//add this to the cleanUpWorldClusters
	//			cleanUpWorldClusters.push_back(cwc);
	//			world_cluster_index++;
	//		}
	//	}
	//	world_clusters_ = cleanUpWorldClusters;

	//	//loop through all layers and enforce proper tetxure file placement
	//	for (unsigned int i = 0; i < layers_.size(); i++)
	//	{
	//		// singles

	//		for (unsigned int j = 0; j < layers_[i].single_textures.size(); j++)
	//		{
	//			auto& current_single_parent_texture = layers_[i].single_textures[j];
	//			/*
	//				transfer the texture file to our Assets Folder
	//				if its' file path is not relative to our Assets Folder.
	//				This will give us a better collected level that is easy to share

	//			*/
	//							const std::string rootAssetsFolderName = "Assets/Textures/";
	//							const unsigned int rootAssetsFolderNameLength = rootAssetsFolderName.size();
	//							const std::string targetTextureFilepath = current_single_parent_texture.texture.asset_file_path;
	//			
	//							bool isInAssetsFolder = true;//give all files the benfit of dout
	//			
	//							/*Check if this file is really in the assets folder*/
	//							if (targetTextureFilepath.size() > rootAssetsFolderNameLength) {
	//								for (int k = 0; k < rootAssetsFolderNameLength; k++)
	//								{
	//									if (targetTextureFilepath[k] != rootAssetsFolderName[k]) {
	//										isInAssetsFolder = false;
	//										break;
	//									}
	//								}
	//							}
	//							else {
	//								//clearly not inside assets folder
	//								isInAssetsFolder = false;
	//							}
	//						
	//			
	//							if (!isInAssetsFolder && current_single_parent_texture.texture.id !=0) {
	//								int screen_category_index = 0;
	//								bool found_screen_category = false;
	//								//if the old file path contained any of these we shall copy it to the respective screen category
	//								for (unsigned k = 0; k < screen_categories.size(); k++)
	//								{
	//									if (targetTextureFilepath.find(screen_categories[i])!=std::string::npos) {
	//										//found image category
	//										found_screen_category = true;
	//										screen_category_index = k;
	//										break;
	//			}
	//								}
	//			
	//								if (found_screen_category) {
	//									const std::string newFileLocation = rootAssetsFolderName+ screen_categories[screen_category_index] + current_single_parent_texture.texture.display_name;
	//									//if the image is already there , this doesn't over wrie it so we are safe like that
	//									if (SquareBox::AssetManager::IOManager::copyFileFromLocationAtoLocationB(targetTextureFilepath, newFileLocation)) {
	//										current_single_parent_texture.texture.asset_file_path = newFileLocation;
	//									}
	//									else {
	//										SBX_CORE_ERROR("File copy operation failed");
	//									}
	//								}
	//								else {
	//									//just copy into dead space
	//									const std::string newFileLocation = rootAssetsFolderName + current_single_parent_texture.texture.display_name;
	//									//if the image is already there , this doesn't over wrie it so we are safe like that
	//									if (SquareBox::AssetManager::IOManager::copyFileFromLocationAtoLocationB(targetTextureFilepath, newFileLocation)) {
	//										current_single_parent_texture.texture.asset_file_path = newFileLocation;
	//									}
	//									else {
	//										SBX_CORE_ERROR("File copy operation failed");
	//									}
	//								}
	//								
	//							}
	//		}

	//		//	tiled
	//		for (unsigned int j = 0; j < layers_[i].tiled_textures.size(); j++)
	//		{
	//			auto& current_tiled_parent_texture = layers_[i].tiled_textures[j];
	//			/*
	//				transfer the texture file to our Assets Folder
	//				if its' file path is not relative to our Assets Folder.
	//				This will give us a better collected level that is easy to share

	//			*/
	//			const std::string rootAssetsFolderName = "Assets/Textures/";
	//			const unsigned int rootAssetsFolderNameLength = rootAssetsFolderName.size();
	//			const std::string targetTextureFilepath = current_tiled_parent_texture.texture.asset_file_path;

	//			bool isInAssetsFolder = true;//give all files the benfit of dout

	//			/*Check if this file is really in the assets folder*/
	//			if (targetTextureFilepath.size() > rootAssetsFolderNameLength) {
	//				for (int k = 0; k < rootAssetsFolderNameLength; k++)
	//				{
	//					if (targetTextureFilepath[k] != rootAssetsFolderName[k]) {
	//						isInAssetsFolder = false;
	//						break;
	//					}
	//				}
	//			}
	//			else {
	//				//clearly not inside assets folder
	//				isInAssetsFolder = false;
	//			}


	//			if (!isInAssetsFolder && current_tiled_parent_texture.texture.id != 0) {
	//				int screen_category_index = 0;
	//				bool found_screen_category = false;
	//				//if the old file path contained any of these we shall copy it to the respective screen category
	//				for (unsigned k = 0; k < screen_categories.size(); k++)
	//				{
	//					if (targetTextureFilepath.find(screen_categories[i]) != std::string::npos) {
	//						//found image category
	//						found_screen_category = true;
	//						screen_category_index = k;
	//						break;
	//					}
	//				}

	//				if (found_screen_category) {
	//					const std::string newFileLocation = rootAssetsFolderName + screen_categories[screen_category_index] + current_tiled_parent_texture.texture.display_name;
	//					//if the image is already there , this doesn't over wrie it so we are safe like that
	//					if (SquareBox::AssetManager::IOManager::copyFileFromLocationAtoLocationB(targetTextureFilepath, newFileLocation)) {
	//						current_tiled_parent_texture.texture.asset_file_path = newFileLocation;
	//					}
	//					else {
	//						SBX_CORE_ERROR("File copy operation failed");
	//					}
	//				}
	//				else {
	//					//just copy into dead space
	//					const std::string newFileLocation = rootAssetsFolderName + current_tiled_parent_texture.texture.display_name;
	//					//if the image is already there , this doesn't over wrie it so we are safe like that
	//					if (SquareBox::AssetManager::IOManager::copyFileFromLocationAtoLocationB(targetTextureFilepath, newFileLocation)) {
	//						current_tiled_parent_texture.texture.asset_file_path = newFileLocation;
	//					}
	//					else {
	//						SBX_CORE_ERROR("File copy operation failed");
	//					}
	//				}

	//			}
	//		}
	//	}
	//}

	void Utilities::showPlottedVertices(bool onlyshowStartAndEnd_, SquareBox::RenderEngine::DebugRenderer & debugRenderer_, const SquareBox::GWOM::ClusterObject & clusterObject_, float pointRadius_, SquareBox::RenderEngine::ColorRGBA8 color_)
	{
		if (clusterObject_.shape == SquareBox::BodyShapeEnum::Edge || clusterObject_.shape == SquareBox::BodyShapeEnum::Chain || clusterObject_.shape == SquareBox::BodyShapeEnum::Polygon)
		{
			//we are going to display the plotted coordinates

			if (clusterObject_.physics_properties != nullptr) {
				std::vector<glm::vec2> verts;
				if (clusterObject_.shape == SquareBox::BodyShapeEnum::Edge) {
					verts = clusterObject_.physics_properties->getEdgeVerticesCoordinates();
				}
				else if (clusterObject_.shape == SquareBox::BodyShapeEnum::Chain) {
					verts = clusterObject_.physics_properties->getChainVerticesCoordinates();
				}
				else if (clusterObject_.shape == SquareBox::BodyShapeEnum::Polygon) {
					verts = clusterObject_.physics_properties->getPolygonVerticesCoordinates();
				}
				if (onlyshowStartAndEnd_) {
					int numVerts = verts.size();
					if (numVerts >= 0) {
						debugRenderer_.drawCircle(verts[0], color_, pointRadius_);
					}
					if (numVerts > 0) {
						debugRenderer_.drawCircle(verts[numVerts - 1], color_, pointRadius_);
					}
				}
				else {
					for (size_t j = 0; j < verts.size(); j++)
					{
						debugRenderer_.drawCircle(verts[j], color_, pointRadius_);
					}
				}
			}
			else {
				const std::vector<glm::vec2> &verts = clusterObject_.vertices;
				auto numVerts = verts.size();
				if (onlyshowStartAndEnd_) {
					if (numVerts >= 0) {
						debugRenderer_.drawCircle(verts[0], color_, pointRadius_);
					}
					if (numVerts > 0) {
						debugRenderer_.drawCircle(verts[numVerts - 1], color_, pointRadius_);
					}
				}
				else {
					for (size_t j = 0; j < numVerts; j++)
					{
						debugRenderer_.drawCircle(verts[j], color_, pointRadius_);
					}
				}
			}
		}
	}

	void Utilities::deactivateClusterObjectJoints(std::vector<SquareBox::GWOM::Layer>& layers_, SquareBox::GWOM::ClusterObject & clusterObject_,SquareBox::PhysicsCollisionEngine::PhysicsWorld* physics_world_ptr_)
	{
		//store the joints info somewhere else
		std::vector<SquareBox::GWOM::Joint> copied_joint_vec = clusterObject_.joints;

		//loop through all its joints destorying one by one

		for (unsigned int i = 0; i < copied_joint_vec.size(); i++)
		{
			auto& focus_joint = copied_joint_vec[i];
			destroyClusterObjectJoint(layers_[clusterObject_.layer_index],focus_joint,physics_world_ptr_);
		}

		//restore back these joints data about the joints os that it is not lost
		clusterObject_.joints = copied_joint_vec;
	}

	void Utilities::destroryClusterObjectJoints(std::vector<SquareBox::GWOM::Layer>& layers_, SquareBox::GWOM::ClusterObject& clusterObject_, SquareBox::PhysicsCollisionEngine::PhysicsWorld* physics_world_ptr_)
	{
		for (unsigned int i = 0; i < clusterObject_.joints.size(); i++)
		{
			auto& focus_joint = clusterObject_.joints[i];
			
			destroyClusterObjectJoint(layers_[clusterObject_.layer_index], focus_joint, physics_world_ptr_);
		}
	}
	//we only need one joint member to reactivate a joint
	void Utilities::reactivateClusterObjectJoints(std::vector<SquareBox::GWOM::Layer>& layers_, SquareBox::GWOM::ClusterObject& clusterObject_, SquareBox::PhysicsCollisionEngine::PhysicsWorld* physics_world_ptr_)
	{
		//store the joints info somewhere else
		std::vector<SquareBox::GWOM::Joint> copied_joint_vec= clusterObject_.joints;
	
		//clear this cluster objects joints info
		clusterObject_.joints.clear();

		for (unsigned int i = 0; i < copied_joint_vec.size(); i++)
		{
			SquareBox::GWOM::Joint& focus_joint = copied_joint_vec[i];
				SquareBox::GWOM::ClusterObject & bodyA = layers_[clusterObject_.layer_index].world_clusters[focus_joint.body_a_coordinates.first].cluster_objects[focus_joint.body_a_coordinates.second];
				SquareBox::GWOM::ClusterObject & bodyB = layers_[clusterObject_.layer_index].world_clusters[focus_joint.body_b_coordinates.first].cluster_objects[focus_joint.body_b_coordinates.second];
				physics_world_ptr_->createJoint(bodyA, bodyB, focus_joint);
				addJointToLayerAliveJointsMap(layers_[clusterObject_.layer_index], copied_joint_vec[i]);
			
		}
	
	}

	void Utilities::destroyClusterObjectJoint(SquareBox::GWOM::Layer& layer_, SquareBox::GWOM::Joint& joint_, SquareBox::PhysicsCollisionEngine::PhysicsWorld* physics_world_ptr_)
	{
		SquareBox::GWOM::ClusterObject& clusterObjectA_ = layer_.world_clusters[joint_.body_a_coordinates.first].cluster_objects[joint_.body_a_coordinates.second];
		SquareBox::GWOM::ClusterObject& clusterObjectB_ = layer_.world_clusters[joint_.body_b_coordinates.first].cluster_objects[joint_.body_b_coordinates.second];

		//deletefrom the physics world
		physics_world_ptr_->destoryJoint(clusterObjectA_, clusterObjectB_,joint_);

		//remove from the layers vector
		removeJointFromLayerAliveJointsMap(layer_, joint_);
	}

	void Utilities::createClusterObjectJoint(SquareBox::GWOM::Layer& layer_, SquareBox::GWOM::Joint & joint_, SquareBox::PhysicsCollisionEngine::PhysicsWorld* physics_world_ptr_)
	{
		SquareBox::GWOM::ClusterObject & clusterObjectA_ = layer_.world_clusters[joint_.body_a_coordinates.first].cluster_objects[joint_.body_a_coordinates.second];
		SquareBox::GWOM::ClusterObject& clusterObjectB_ = layer_.world_clusters[joint_.body_b_coordinates.first].cluster_objects[joint_.body_b_coordinates.second];
		//add to the physics world 
		physics_world_ptr_->createJoint(clusterObjectA_, clusterObjectB_, joint_);
		// add to the layers vector
		addJointToLayerAliveJointsMap(layer_, joint_);
	}

	void Utilities::createLayerJointsOnLoadFromStorage(SquareBox::GWOM::Layer& layer_, SquareBox::PhysicsCollisionEngine::PhysicsWorld* physics_world_ptr_)
	{
			for (auto  it = layer_.temp_joint_definitions.begin(); it != layer_.temp_joint_definitions.end(); it++)
			{
				createClusterObjectJoint(layer_, (*it), physics_world_ptr_);
			}
			layer_.temp_joint_definitions.clear();
	}

	void Utilities::dispose()
	{
	}

	void Utilities::nameWorldClusterByIndex(SquareBox::GWOM::WorldCluster & world_cluster)
	{
		int index = world_cluster.index;
		std::stringstream ss;
		if (index < 10) {
			ss << "World Cluster 00";
			ss << index;
		}
		else if (index < 100) {
			ss << "World Cluster 0";
			ss << index;
		}
		else {
			ss << "World Cluster ";
			ss << index;
		}
		std::string nameString = ss.str();

		for (unsigned int si = 0; si < nameString.size(); si++)
		{
			world_cluster.name[si] = nameString[si];
		}
		world_cluster.name[nameString.size()] = '\0';
	}

	void Utilities::nameWorldClusterByGivenName(SquareBox::GWOM::WorldCluster & world_cluster_, const std::string new_name_)
	{
		for (unsigned int si = 0; si < new_name_.size(); si++)
		{
			world_cluster_.name[si] = new_name_[si];
		}
		world_cluster_.name[new_name_.size()] = '\0';
	}

	void Utilities::nameClusterObjectByIndex(SquareBox::GWOM::ClusterObject & cluster_object)
	{
		int index = cluster_object.index;
		std::stringstream ss;
		if (index < 10) {
			ss << "Cluster Object 00";
			ss << index;
		}
		else if (index < 100) {
			ss << "Cluster Object 0";
			ss << index;
		}
		else {
			ss << "Cluster Object ";
			ss << index;
		}
		std::string nameString = ss.str();

		for (unsigned int si = 0; si < nameString.size(); si++)
		{
			cluster_object.name[si] = nameString[si];
		}
		ss.clear();
		cluster_object.name[nameString.size()] = '\0';
	}

	void Utilities::nameClusterObjectByGivenName(SquareBox::GWOM::ClusterObject & cluster_object_, const std::string new_name_)
	{
		for (unsigned int si = 0; si < new_name_.size(); si++)
		{
			cluster_object_.name[si] = new_name_[si];
		}
		cluster_object_.name[new_name_.size()] = '\0';
	}

	void Utilities::nameLayerByGivenName(SquareBox::GWOM::Layer& layer_, const std::string new_name_)
	{
		for (unsigned int si = 0; si < new_name_.size(); si++)
		{
			layer_.name[si] = new_name_[si];
		}
		layer_.name[new_name_.size()] = '\0';
	}

	void Utilities::nameLayerByIndex(SquareBox::GWOM::Layer & layer_)
	{
		int index = layer_.index;
		std::stringstream ss;
		if (index < 10) {
			ss << "Layer 00";
			ss << index;
		}
		else if (index < 100) {
			ss << "Layer 0";
			ss << index;
		}
		else {
			ss << "Layer ";
			ss << index;
		}
		std::string nameString = ss.str();

		for (unsigned int si = 0; si < nameString.size(); si++)
		{
			layer_.name[si] = nameString[si];
		}
		ss.clear();
		layer_.name[nameString.size()] = '\0';
	}

	void Utilities::updateClusterObjectLayerIndex(SquareBox::GWOM::ClusterObject& cluster_object_, int new_layer_index_)
	{
		cluster_object_.layer_index = new_layer_index_;
		/*
		  joints do not reference the layers index, so a change in layer index does not affect them
		  if i find other parts that refrenece tha layer inside of a cluster objects that they need to be changed.
		  it will be done from here.
		*/
	}

	void Utilities::updateClusterObjectIndex(SquareBox::GWOM::ClusterObject& cluster_object_, int new_cluster_object_index, std::vector<SquareBox::GWOM::Layer>& layers_, SquareBox::PhysicsCollisionEngine::PhysicsWorld* targetPhysicsWorld_)
	{
		const int old_cluster_object_index = cluster_object_.index;
		std::string old_cluster_object_name = cluster_object_.name;
		nameClusterObjectByIndex(cluster_object_);
		std::string expected_cluster_object_name = cluster_object_.name;
		cluster_object_.index = new_cluster_object_index;
		const int focus_layer_index = cluster_object_.layer_index; //since joints are only between same layer objects
		//handle the joints

		//store the joints info somewhere else since destorying the joint will erase it
		std::vector<SquareBox::GWOM::Joint> copied_joint_vec = cluster_object_.joints;
		
		for (unsigned int i = 0; i < copied_joint_vec.size(); i++)
		{
			auto& focus_joint = copied_joint_vec[i];
			if (focus_joint.is_joint_alive) {

				destroyClusterObjectJoint(layers_[focus_layer_index],focus_joint,targetPhysicsWorld_);
				//update my coordinates in the joint
				focus_joint.body_b_coordinates = std::pair<int, int>(cluster_object_.cluster_index, cluster_object_.index);
				createClusterObjectJoint(layers_[focus_layer_index], focus_joint, targetPhysicsWorld_);
			}
		}
		
		//changing the name if there is need
		nameClusterObjectByIndex(cluster_object_);
		std::string new_cluster_object_name = cluster_object_.name;

		if (old_cluster_object_name == expected_cluster_object_name) {
			nameClusterObjectByGivenName(cluster_object_, new_cluster_object_name);
		}
		else {
			nameClusterObjectByGivenName(cluster_object_, old_cluster_object_name);
		}
	}

	void Utilities::deleteClusterObjectFromWorld(std::vector<SquareBox::GWOM::Layer>& layers_, SquareBox::GWOM::ClusterObject & cluster_object_, SquareBox::PhysicsCollisionEngine::PhysicsWorld* physics_world_ptr_)
	{
		if (cluster_object_.is_alive) {
			//delete all joints associated to this object
			destroryClusterObjectJoints(layers_, cluster_object_, physics_world_ptr_);
			if (cluster_object_.physics_properties != nullptr) {
				cluster_object_.physics_properties->dispose();
				cluster_object_.physics_properties = nullptr;
			}
			cluster_object_.is_alive = false;
			removePairFromVector(layers_[cluster_object_.layer_index].alive_cluster_objects, std::pair<int, int>(cluster_object_.cluster_index, cluster_object_.index));

		}
		else {
			SBX_CORE_ERROR("Tried to delete a  dead cluster object");
		}
	
	}

	void Utilities::setCurrentShapePointer(SquareBox::BodyShapeEnum enum_, SquareBox::IShape** currentShape_)
	{
		bool found = false;
		for (unsigned i = 0; i < m_vec_shapes_pointer.size(); i++)
		{
			if (m_vec_shapes_pointer[i]->body_shape == enum_) {
				*currentShape_ = m_vec_shapes_pointer[i];
				found = true;
				break;
			}
		}

		if (!found) {
			SBX_CORE_CRITICAL("Invalid Shape , Shape not part of known shapes");
		}
	}

	float Utilities::scaleFloatAccordingToScreen(float original, glm::ivec2 orignalMaxWindow_, glm::ivec2 currentMaxWindow_)
	{
		int orignal_total = std::max(orignalMaxWindow_.x , orignalMaxWindow_.y);
		int current_total = std::max(currentMaxWindow_.x , currentMaxWindow_.y);

		return original*(current_total/orignal_total);
	}

	void Utilities::loadLayerTextures(SquareBox::GWOM::Layer& layer_,float dpi_)
	{
		//load all the layer_s textures
		/* We need to load the textures in an order that respects the their index in their respective category*/
		//singles
		//copy the orignal list
		auto original_single_textures = layer_.single_textures;
		layer_.single_textures.clear();
		for (unsigned int j = 0; j < original_single_textures.size(); j++)
		{
			auto& current_single_parent_texture = original_single_textures[j];
			SquareBox::GWOM::ParentTexture tmp_single_parent_texture;
			tmp_single_parent_texture.texture_index = j;//if this causes errors then we have a bug somewhere
			tmp_single_parent_texture.texture = SquareBox::AssetManager::TextureManager::getTextureByName(current_single_parent_texture.texture.display_name, dpi_);
			tmp_single_parent_texture.texture.display_name = current_single_parent_texture.texture.display_name;
			//set the textures display name , in the asset manager for future use for future use
			SquareBox::AssetManager::TextureManager::setTextureDisplayNameById(tmp_single_parent_texture.texture.id, current_single_parent_texture.texture.display_name);
			layer_.single_textures.push_back(tmp_single_parent_texture);
		}

		//tiled textures
		auto original_tiled_textures = layer_.tiled_textures;
		layer_.tiled_textures.clear();
		for (unsigned int j = 0; j < original_tiled_textures.size(); j++)
		{
			auto& current_tiled_parent_texture = original_tiled_textures[j];
			SquareBox::GWOM::ParentTexture tmp_tiled_parent_texture;
			tmp_tiled_parent_texture.texture_index = j;//if this causes errors then we have a bug somewhere
			tmp_tiled_parent_texture.texture = SquareBox::AssetManager::TextureManager::getTextureByName(current_tiled_parent_texture.texture.display_name, dpi_);
			tmp_tiled_parent_texture.texture.asset_file_path = current_tiled_parent_texture.texture.asset_file_path;
			tmp_tiled_parent_texture.texture.display_name = current_tiled_parent_texture.texture.display_name;
			tmp_tiled_parent_texture.texture.tiling = current_tiled_parent_texture.texture.tiling;
			//set the textures display name , in the asset manager for future use for future use
			SquareBox::AssetManager::TextureManager::setTextureDisplayNameById(tmp_tiled_parent_texture.texture.id, current_tiled_parent_texture.texture.display_name);
			SquareBox::AssetManager::TextureManager::setTextureTilingById(tmp_tiled_parent_texture.texture.id, tmp_tiled_parent_texture.texture.tiling);
			layer_.tiled_textures.push_back(tmp_tiled_parent_texture);
		}
	}

	void Utilities::loadGuiLayerTexturesAndFonts(SquareBox::GWOM::GuiLayer& gui_layer_, float dpi_)
	{
		//loading the textures
		// singles
		//copy the orignal list
		auto original_single_textures = gui_layer_.single_textures;
		gui_layer_.single_textures.clear();
		for (unsigned int i = 0; i < original_single_textures.size(); i++)
		{
			auto& current_single_texture = original_single_textures[i];
			SquareBox::AssetManager::GLTexture tmp_single_texture = SquareBox::AssetManager::TextureManager::getTextureByName(current_single_texture.display_name, dpi_);
			SquareBox::AssetManager::TextureManager::setTextureTilingById(tmp_single_texture.id, current_single_texture.tiling);
			gui_layer_.single_textures.push_back(tmp_single_texture);
		}

		//copy the orignal list
		//tiled
		auto original_tiled_textures = gui_layer_.tiled_textures;
		gui_layer_.tiled_textures.clear();
		for (unsigned int i = 0; i < original_tiled_textures.size(); i++)
		{
			auto& current_tiled_texture = original_tiled_textures[i];
			SquareBox::AssetManager::GLTexture tmp_tiled_texture = SquareBox::AssetManager::TextureManager::getTextureByName(current_tiled_texture.display_name, dpi_);
			SquareBox::AssetManager::TextureManager::setTextureTilingById(tmp_tiled_texture.id, current_tiled_texture.tiling);
			tmp_tiled_texture.tiling = current_tiled_texture.tiling;
			gui_layer_.tiled_textures.push_back(tmp_tiled_texture);
		}


		//the sprite fonts
		auto original_sprite_fonts = gui_layer_.sprite_fonts;
		gui_layer_.sprite_fonts.clear();
		for (unsigned int i = 0; i < original_sprite_fonts.size(); i++)
		{
			SquareBox::RenderEngine::SpriteFont tmp_sprite_font;
			//this has to be edited so that sprite fonts are also loaded from 
			//respective folders like textures
			tmp_sprite_font.initWithName(original_sprite_fonts[i].getDisplayName(), original_sprite_fonts[i].getFontSize());
			tmp_sprite_font.setDisplayName(original_sprite_fonts[i].getDisplayName());
			gui_layer_.sprite_fonts.push_back(tmp_sprite_font);
		}


		//upating the texture infos texture_id
		for (unsigned int i = 0; i < gui_layer_.gui_elements.size(); i++)
		{
			for (unsigned int j = 0; j < gui_layer_.gui_elements[i].textures.size(); j++) {
				auto& focus_texture_info = gui_layer_.gui_elements[i].textures[j];
				if (focus_texture_info.texture_type == SquareBox::TextureEnum::SINGLE) {
					if (gui_layer_.single_textures.size() > focus_texture_info.texture_index) {
						focus_texture_info.texture_id = gui_layer_.single_textures[focus_texture_info.texture_index].id;
					}
				}
				else {
					//tile sheet
					if (gui_layer_.tiled_textures.size() > focus_texture_info.texture_index) {
						focus_texture_info.texture_id = gui_layer_.tiled_textures[focus_texture_info.texture_index].id;
					}
				}
			}
		}
	}

	Utilities::creationDetails  Utilities::createClusterObjectIntoWorld(bool drawingPlotedCords, bool rebirth, bool editPosition, glm::vec2 newPosition_, SquareBox::GWOM::ClusterObject * ClusterObject_, std::vector<SquareBox::GWOM::Layer>& layers_, SquareBox::PhysicsCollisionEngine::PhysicsWorld* targetPhysicsWorld_, bool autoNewClusterObjectCreation, bool respectAncestor_)
	{
		creationDetails detials;
		std::pair<int, int> settlement = std::pair<int, int>(-1, -1);
		/* 
		  this function takes in a clusterObject

		  this clusterObject , at  the time it is handed over to this function is already a member in the 
		  worldCluster vector.

		  so meaning that this function does not have a power to fix a cluster object into a specific worldCluster
		  system.

          if the world cluster of an object is desired to be changed , then the correct approach would be to 
		  push the world cluster in the appropriate world cluster and update its details

		  This functions only roles are to enable smooth
		  adding physics objects to the world.
		  object rebirths with editied attributes like position
		  sending back coordinates of what should be our next current cluster object in the world
		  set is_alive to true
		  add the cluster_objects coordinates to alive_cluster_object
		 */
		std::pair<int, int> shell = std::pair<int, int>(-1, -1);
		int layer_index = ClusterObject_->layer_index;
		int world_cluster_index = ClusterObject_->cluster_index;
		SquareBox::GWOM::ClusterObject ancestor;
		if (ClusterObject_ != nullptr) {
		/* determining the current shape */
			SquareBox::IShape* currentShape_ptr = nullptr;

			setCurrentShapePointer(ClusterObject_->shape, &currentShape_ptr);

			if (!rebirth) {//we should not copy visitors attributes
				if (currentShape_ptr->is_plotted) {
					if (drawingPlotedCords) {//only copy if we are plotting
						ancestor = *ClusterObject_;
					}
				}
				else {
					//copy properties
					ancestor = *ClusterObject_;
				}
			}
			if (rebirth) {
				if (ClusterObject_->physics_properties != nullptr) {
					deactivateClusterObjectJoints(layers_, *ClusterObject_,targetPhysicsWorld_);
					ClusterObject_->physics_properties->dispose();
					/*
						we should not set ClusterObject_->physics_properties = nullptr, this will prevent the object from being created in our
						physics world, because it will not have a physics Engine !!!!
					*/
				}

				//displacement for the plotedShapes
				if (editPosition) {
					if (currentShape_ptr->is_plotted) {
						float xDisplacement;
						float yDisplacement;
						auto numVertices = ClusterObject_->vertices.size();
						glm::vec2 currentVertex;

						if (currentShape_ptr->displacement_based_on_centroid) {
							//displacement based on centroid
							currentVertex = compute2DPolygonCentroid(ClusterObject_->vertices);
						}
						else {
							//displacement based on what was clicked
						//we are not considering the centroid for the polygon cos we can create a polygon in every place
						//so its nyc to be able to steadly move it
							if (ClusterObject_->is_last_selected) {
								currentVertex = ClusterObject_->vertices[numVertices - 1];
							}
							else {
								//displacement based on first  , aploygon always defaults to this cos its first and end points are in the same place
								currentVertex = ClusterObject_->vertices[0];
							}
						}

						xDisplacement = newPosition_.x - currentVertex.x;
						yDisplacement = newPosition_.y - currentVertex.y;
						for (size_t i = 0; i < numVertices; i++)
						{
							ClusterObject_->vertices[i] = glm::vec2(ClusterObject_->vertices[i].x + xDisplacement, ClusterObject_->vertices[i].y + yDisplacement);
						}
					}
				}
			}

			//the actual placement of ClusterObjects into the world

			if (drawingPlotedCords) {
				if (currentShape_ptr->is_calculated_light) {
					if (ClusterObject_->vec_of_edges.size() >= 4) {
						if (!rebirth) {
							ClusterObject_->is_alive = true;
							settlement.first = ClusterObject_->cluster_index;
							settlement.second = ClusterObject_->index;
							if (autoNewClusterObjectCreation) {
								//Create next
								int new_cluster_object_index = 0;
								layers_[layer_index].world_clusters[world_cluster_index].cluster_objects.emplace_back();
								new_cluster_object_index = layers_[layer_index].world_clusters[world_cluster_index].cluster_objects.size() - 1;
								// can not be zero because we just pushed something behind here
								auto & new_cluster_object = layers_[layer_index].world_clusters[world_cluster_index].cluster_objects[new_cluster_object_index];
								new_cluster_object.cluster_index = layers_[layer_index].world_clusters[world_cluster_index].index;
								new_cluster_object.index = new_cluster_object_index;
								new_cluster_object.layer_index = layer_index;
								nameClusterObjectByIndex(new_cluster_object);
								shell.first = new_cluster_object.cluster_index;
								shell.second = new_cluster_object.index;
							}
						}
					}
					else {
						SBX_WARN("Failed to plot object .{} points are Insufficient points", ClusterObject_->vertices.size());
					}
				}
				else if (currentShape_ptr->is_plotted) {
					//a ploygon needs a minium of 3 points
					if (ClusterObject_->vertices.size() >= currentShape_ptr->min_plotting_vertices) {
						//Setting the polygons position to the mouse coords messes up the physics
						//So will just leave it to the default which is glm::vec2(0);

						//TODO; Calculate the max width and height that the ploygon is occupying and use those for its desRect.z ans destRect.w
						if (ClusterObject_->physics_properties != nullptr) {
							if (targetPhysicsWorld_ != nullptr&& targetPhysicsWorld_->isWorldinitalised()) {
								targetPhysicsWorld_->createObject(*ClusterObject_);
							}
							else {
								SBX_CORE_ERROR("Physics World is not Intialised");
							}
						}
						if (!rebirth) {
							ClusterObject_->is_alive = true;
							settlement.first = ClusterObject_->cluster_index;
							settlement.second = ClusterObject_->index;
							if (autoNewClusterObjectCreation) {
								//Create next
								int new_cluster_object_index = 0;

								layers_[layer_index].world_clusters[world_cluster_index].cluster_objects.emplace_back();
								new_cluster_object_index = layers_[layer_index].world_clusters[world_cluster_index].cluster_objects.size() - 1;
								// can not be zero because we just pushed something behind here
								auto & new_cluster_object = layers_[layer_index].world_clusters[world_cluster_index].cluster_objects[new_cluster_object_index];
								new_cluster_object.cluster_index = layers_[layer_index].world_clusters[world_cluster_index].index;
								new_cluster_object.index = new_cluster_object_index;
								new_cluster_object.layer_index = layer_index;
								nameClusterObjectByIndex(new_cluster_object);
								shell.first = new_cluster_object.cluster_index;
								shell.second = new_cluster_object.index;
							}
						}
					}
					else
					{
						SBX_WARN("Failed to plot object .{} points are Insufficient points", ClusterObject_->vertices.size());
					}
				}
			}

			else if (currentShape_ptr->needs_radius) {
				//radius should be greater than 0
				if (ClusterObject_->radius > 0) {
					if (editPosition) {
						ClusterObject_->position = glm::vec2(newPosition_.x, newPosition_.y);
					}
					if (ClusterObject_->physics_properties != nullptr) {
						if (targetPhysicsWorld_ != nullptr&& targetPhysicsWorld_->isWorldinitalised()) {
							targetPhysicsWorld_->createObject(*ClusterObject_);
						}
						else {
							SBX_CORE_ERROR("Physics World is not Intialised");
						}
					}
					if (!rebirth) {
						ClusterObject_->is_alive = true;
						settlement.first = ClusterObject_->cluster_index;
						settlement.second = ClusterObject_->index;
						if (autoNewClusterObjectCreation) {
							//Create next
							int new_cluster_object_index = 0;

							layers_[layer_index].world_clusters[world_cluster_index].cluster_objects.emplace_back();
							new_cluster_object_index = layers_[layer_index].world_clusters[world_cluster_index].cluster_objects.size() - 1;
							// can not be zero because we just pushed something behind here
							auto & new_cluster_object = layers_[layer_index].world_clusters[world_cluster_index].cluster_objects[new_cluster_object_index];
							new_cluster_object.cluster_index = layers_[layer_index].world_clusters[world_cluster_index].index;
							new_cluster_object.index = new_cluster_object_index;
							new_cluster_object.layer_index = layer_index;
							nameClusterObjectByIndex(new_cluster_object);
							shell.first = new_cluster_object.cluster_index;
							shell.second = new_cluster_object.index;
						}
					}
				}
				else {
					SBX_WARN("Failed to Create Circle,invalid {} meters radius  ", ClusterObject_->radius);
				}
			}
			else if (currentShape_ptr->needs_height_width) {
				if (ClusterObject_->width > 0 && ClusterObject_->height > 0)
				{
					if (editPosition) {
						ClusterObject_->position = glm::vec2(newPosition_.x, newPosition_.y);
					}
					if (ClusterObject_->physics_properties != nullptr) {
						if (targetPhysicsWorld_ != nullptr&& targetPhysicsWorld_->isWorldinitalised()) {
							targetPhysicsWorld_->createObject(*ClusterObject_);
						}
						else {
							SBX_CORE_ERROR("Physics World is not Intialised");
						}
					}
					if (!rebirth) {
						ClusterObject_->is_alive = true;
						settlement.first = ClusterObject_->cluster_index;
						settlement.second = ClusterObject_->index;
						if (autoNewClusterObjectCreation) {
							//Create next
							int new_cluster_object_index = 0;

							layers_[layer_index].world_clusters[world_cluster_index].cluster_objects.emplace_back();
							new_cluster_object_index = layers_[layer_index].world_clusters[world_cluster_index].cluster_objects.size() - 1;
							// can not be zero because we just pushed something behind here
							auto & new_cluster_object = layers_[layer_index].world_clusters[world_cluster_index].cluster_objects[new_cluster_object_index];
							new_cluster_object.cluster_index = layers_[layer_index].world_clusters[world_cluster_index].index;
							new_cluster_object.index = new_cluster_object_index;
							new_cluster_object.layer_index = layer_index;
							nameClusterObjectByIndex(new_cluster_object);
							shell.first = new_cluster_object.cluster_index;
							shell.second = new_cluster_object.index;
						}
					}
				}
				else {
					SBX_WARN("Failed to create Box . {} width and {} height are invalid dimensions", ClusterObject_->width, ClusterObject_->height);
				}
			}
			else {
				//this is for the plotted shapes
				ClusterObject_->vertices.push_back(newPosition_);
			}

			//at the end
			if (rebirth && ClusterObject_->physics_properties != nullptr&&ClusterObject_->physics_properties->isIntialised()) {
				if (targetPhysicsWorld_ != nullptr&& targetPhysicsWorld_->isWorldinitalised()) {
					reactivateClusterObjectJoints(layers_, *ClusterObject_,targetPhysicsWorld_);
				}
				else {
					SBX_CORE_ERROR("Physics World is not Intialised");
				}
			}

			bool prepopulateClusterObject = false;

			if (!rebirth && autoNewClusterObjectCreation && shell.first!=-1 && shell.second!=-1) {
				prepopulateClusterObject = true;
			}

			if (respectAncestor_ && prepopulateClusterObject) {
				auto & new_cluster_object = layers_[layer_index].world_clusters[shell.first].cluster_objects[shell.second];
				new_cluster_object.layer_index = shell.first;
				new_cluster_object.index = shell.second;
				new_cluster_object.layer_index = ancestor.layer_index;
				new_cluster_object.texture_info = ancestor.texture_info;
				new_cluster_object.group_index = ancestor.group_index;
				new_cluster_object.family = ancestor.family;
				new_cluster_object.vec_of_mask_bits = ancestor.vec_of_mask_bits;
				new_cluster_object.height = ancestor.height;
				new_cluster_object.width = ancestor.width;
				new_cluster_object.radius = ancestor.radius;
				new_cluster_object.friction = ancestor.friction;
				new_cluster_object.restitution = ancestor.restitution;
				new_cluster_object.angle = ancestor.angle;
				new_cluster_object.init_angular_velocity = ancestor.init_angular_velocity;
				new_cluster_object.frequency = ancestor.frequency;
				new_cluster_object.linear_damping = ancestor.linear_damping;
				new_cluster_object.angular_damping = ancestor.angular_damping;
				new_cluster_object.gravity_scale = ancestor.gravity_scale;
				new_cluster_object.resistivity = ancestor.resistivity;

				new_cluster_object.is_fixed_rotation = ancestor.is_fixed_rotation;
				new_cluster_object.allow_sleep = ancestor.allow_sleep;
				new_cluster_object.awake = ancestor.awake;
				new_cluster_object.is_bullet = ancestor.is_bullet;
				new_cluster_object.controller_type = ancestor.controller_type;
				new_cluster_object.shape = ancestor.shape;
				new_cluster_object.type = ancestor.type;

				new_cluster_object.vertices = ancestor.vertices;
				new_cluster_object.off_set = ancestor.off_set;
				new_cluster_object.position = ancestor.position;
				new_cluster_object.init_linear_velocity = ancestor.init_linear_velocity;

				new_cluster_object.texture_info.color = ancestor.texture_info.color;
			}
		}
		else {
			SBX_CORE_ERROR("Null ClusterObject during createClusterObject");
		}
		detials.settlementCoordinates = settlement;
		detials.shellCoordinates = shell;

		if (settlement.first > -1 && settlement.second > -1 && !rebirth) {
			/*
				when doing multiple cluster objects insertion into the world i.e
				when loading a level, this addPairToVector becomes more time consumo=ing with time because of 
				the increasing number of checks that must be done for every new member that it recieves
			*/
			layers_[layer_index].world_clusters[settlement.first].cluster_objects[settlement.second].is_alive = true;
			addPairToVector(layers_[layer_index].alive_cluster_objects,settlement);
		}
		return detials;
	}
	bool Utilities::isInBox(const  glm::vec2& testCoordinates_, const glm::vec4& testBoxDestRect_) {
		const float x_position = testBoxDestRect_.x;
		const float y_position = testBoxDestRect_.y;
		const float width = testBoxDestRect_.z;
		const float height = testBoxDestRect_.w;

		const float x_coordinate = testCoordinates_.x;
		const float y_coordinate = testCoordinates_.y;

		const float  x1 = x_position;
		const float  y1 = y_position;
		const float  x2 = x1 + width;
		const float  y2 = y1 + +height;

		return (x_coordinate > x1 && x_coordinate<x2&& y_coordinate>y1 && y_coordinate < y2);
	}
}