#include "Sprite.h"

Sprite::Sprite()
{
	_vboID = 0;
}


Sprite::~Sprite()
{
	if (_vboID != 0) {
		glDeleteBuffers(1, &_vboID);
	}
}

void Sprite::init(float x, float y, float width, float height)
{
	_x = x;
	_y = y;
	_width = width;
	_height = height;

	if (_vboID == 0) {
		glGenBuffers(1, &_vboID);
	}

	SquareBox::RenderEngine::Vertex   vertexData[6];
	//First triangle
	vertexData[0].setPosition(x + width, y + height);
	vertexData[0].setUV(1.0f, 1.0f);

	vertexData[1].setPosition(x,y+height);
	vertexData[1].setUV(0.0f, 1.0f);

	vertexData[2].setPosition(x,y);
	vertexData[2].setUV(0.0f, 0.0f);

	//Second triange
	/*vertexData[3].setPosition(x, y);
	vertexData[3].setUV(0.0f, 0.0f);

	vertexData[4].setPosition(x+width, y);
	vertexData[4].setUV(1.0f, 0.0f);

	vertexData[5].setPosition(x+width, y+height);
	vertexData[5].setUV(1.0f, 1.0f);*/

	//Setting color for each

	for (unsigned int i = 0; i < 3; i++)
	{
		vertexData[i].setColor(255, 255, 255, 255);
	}


	//Bind the buffer
	glBindBuffer(GL_ARRAY_BUFFER, _vboID);
	//Upload to the buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

	//Unbind the buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void Sprite::draw()
{
	glBindBuffer(GL_ARRAY_BUFFER, _vboID);
	glEnableVertexAttribArray(0);

	//The position Attribute pointer
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(SquareBox::RenderEngine::Vertex), (void*)offsetof(SquareBox::RenderEngine::Vertex, position));
	//The color Attribute pointer
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(SquareBox::RenderEngine::Vertex), (void*)offsetof(SquareBox::RenderEngine::Vertex, color));
	//This is the UV attribute pointer
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(SquareBox::RenderEngine::Vertex), (void*)offsetof(SquareBox::RenderEngine::Vertex, uv));
	
	//actually drawing the data
	glDrawArrays(GL_TRIANGLES,0,3);

	glDisableVertexAttribArray(0);
	//Unbind the buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
