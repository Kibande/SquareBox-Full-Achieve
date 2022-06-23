#pragma once
#include <string>
class LayerIndicies {
public:
	/*layer indicies*/

	static const int concerate_layer_index;
	static const std::string concerate_layer_name;

	static const int roads_layer_index;
	static const std::string roads_layer_name;

	static const int grass_layer_index;
	static const std::string grass_layer_name;

	static const int house_tops_layer_index;
	static const std::string house_tops_layer_name;

	static const  int bricks_layer_index;
	static const std::string bricks_layer_name;

	static const int humans_layer_index;
	static const std::string humans_layer_name;
	static const std::string player_layer_name;

	static const int zombies_layer_index;
	static const std::string zombie_layer_name;

	static const int trees_layer_index;
	static const std::string trees_layer_name;

	static int ammunition_layer_index;
	static  std::string ammunition_layer_name;  // these get filled in by the load level file

	/*
		The layer indicies are used to arrange and keep track of the layers during the game play

		while the layer names are used at load time , to be able to idenitiy while layer is currently
		being loaded.

		This is import because a different kind of setup maybe required depending on the layers contents

		The layer names should match the names given to the layers in the Editor while the indcies should match the ones
		given to the layers in the Editor

	*/
};