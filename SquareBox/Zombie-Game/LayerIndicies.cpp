#include "LayerIndicies.h"
// these should be set to represent the way the world layers
// in the editor are stacked up
int const LayerIndicies::bricks_layer_index = 0;
int const LayerIndicies::house_tops_layer_index = 1;
int const  LayerIndicies::humans_layer_index = 2;
int const LayerIndicies::zombies_layer_index = 3;
// index 4 is where the humans were originally kept , so after loading , we just leave that layer
// alone and do not access it again , because doing this will disorganize our layer arrangements
int const LayerIndicies::grass_layer_index = 5; 
int LayerIndicies::ammunition_layer_index; // the layer that ammunition is stored s
