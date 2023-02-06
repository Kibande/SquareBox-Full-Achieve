#version 130
//The fragment shader operates on each pixel in a given polygon

in vec2 fragmentPosition;
in vec4 fragmentColor;
in vec2 fragmentUV;


//This is the 3 component float vector that gets outputted to the screen
//for each pixel
out vec4 color;

out vec2 vertexPosition;


//uniform float time;
uniform sampler2D mySampler;


void main(){

  	//get the color from the texture
	vec4 textureColor = texture(mySampler,fragmentUV);


	//use the color that was passed in to the fragment shader
	color = textureColor *fragmentColor;


	color = fragmentColor * textureColor ;

	
}