#version 300 es
precision mediump float;
//The fragment shader operates on each pixel in a given polygon

in vec2 fragmentPosition;
in vec4 fragmentColor;
in vec2 fragmentUV;


//This is the 3 component float vector that gets outputted to the screen
//for each pixel
out vec4 color;

out vec2 vertexPosition;

void main(){

        float distance=length(fragmentUV);

        //the further away the light particle the more tranparent it is
	//color = vec4(fragmentColor.rgb, fragmentColor.a * (pow(0.01,distance)-0.01));
	color=fragmentColor;

	
}