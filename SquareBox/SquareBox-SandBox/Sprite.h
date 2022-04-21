#pragma once

#ifdef SQB_PLATFORM_ANDROID
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#else
#include<GL/glew.h>
#endif

#include <iostream>
#include <string>
#include <SDL\include\SDL.h>
#include <Render-Engine/RenderingEngine.h>

class Sprite
{
public:
	Sprite();
	~Sprite();

	void init(float x, float y, float width, float height);


	void draw();

private:
	float _x;
	float _y;
	float _width;
	float _height;

	GLuint _vboID;

};

