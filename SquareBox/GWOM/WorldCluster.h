#pragma once
#include "ClusterObject.h"
#include<vector>
#include<string>


namespace SquareBox {
	namespace GWOM {
		class WorldCluster {
		public:
			WorldCluster() {};
			~WorldCluster() {};
			/*ints*/
			int index = 0;  //index of this cluster in our workspace
			int deletionIndex = -1; //negative one means that this cluster has not yet been deleted

			/*string*/
			char name[20];

			/*floats*/

			/*bool*/
			bool isSelected = false;
			/*glm::vecs*/
			/*Enum*/

			/*External*/
			std::vector<ClusterObject> cluster_objects;

			/*method*/
		};
	};

};

