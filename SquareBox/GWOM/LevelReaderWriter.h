#include <string>
#include <vector>
/*
we are these includes are a must here , we have a circular include issue

*/
#include <GWOM/Entities.h>
#include <Physics-Collision-Engine/PhysicsProperties.h>
namespace SquareBox {
	//Game World Object Model
	namespace GWOM
	{
		class LevelReaderWriter
		{
		public:
			bool saveWorldAsBinary(const std::string& file_path_, const std::vector<WorldCluster>& vector_of_world_clusters_,const std::vector<SquareBox::GWOM::Layer> & layers_,float active_camera_scale_, const glm::vec2 & active_camera_position_,int active_camera_index_);
			bool loadWorldAsBinary(const std::string& file_path_, std::vector<WorldCluster>& vector_of_world_clusters_, std::vector<SquareBox::GWOM::Layer>& layers_, float & active_camera_scale_, glm::vec2& active_camera_position_,int & active_camera_index_);

		private:
		};
	};
}
