#pragma once
#include<string>
#ifdef SQB_PLATFORM_ANDROID
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#else
#include<GL/glew.h>
#endif
#include <SquareBox-Logger/Logger.h>
#include<fstream>
#include<vector>
#include <SquareBox-Logger/Logger.h>
namespace SquareBox {
	namespace RenderEngine {
		//this class handles the compilation,linking and usage of GLSL shader program.
		//Reference: http:://www.opengl.org/wiki/Shader_Compilation
		class GLSLProgram
		{
		public:
			GLSLProgram();
			~GLSLProgram();
			void init() {};
			void compileShaders(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath);

			void compileShadersFromSource(const char* vertexSource, const char* fragmentSource);
			void addDefaultTextureAttributes();
			void addDefaultDebugAttributes();
			void compileDefaultTextureShaders();
			void compileDefaultDebugShaders();
			void linkShaders();
			void addAttribute(const std::string& attributeName);
			GLint getUnifromLocation(const std::string& uniformName);
			void use();
			void unuse();

			void dispose();
		private:
			//caching for uniforms
			std::unordered_map<std::string, int> m_UniformLocationCache;
			int _numAttributes;
			GLuint _programID;
			GLuint _vertexShaderID;
			GLuint _fragmentShaderID;
			void compileShader(const char* source, const std::string& identifier, GLuint id);


			const char* m_default_texture_vertex_shader = R"(#version 300 es

					//The Vertex shader operates on each vertex

					//input data from the VBO .Each vertex is 2 vertex
					in vec2 vertexPosition;
					in vec4 vertexColor;
					in vec2 vertexUV;

					out vec2 fragmentPosition;
					out vec4 fragmentColor;
					out vec2 fragmentUV;

					uniform mat4 P;

					void main() {
						//Set the x,y position on the screen
						gl_Position.xy = (P*vec4(vertexPosition, 0.0, 1.0)).xy;
						//the z position is zero since we are in 2D
						gl_Position.z = 0.0;

						// Indicate that the coordinates are normalized
						gl_Position.w = 1.0;

					   fragmentPosition=vertexPosition;
					   fragmentColor=vertexColor;
					   fragmentUV= vec2(vertexUV.x, 1.0- vertexUV.y);
					}


				)";


			const char* m_default_texture_fragment_shader = R"(#version 300 es
						precision mediump float;
						//The fragment shader operates on each pixel in a given polygon

						in vec2 fragmentPosition;
						in vec4 fragmentColor;
						in vec2 fragmentUV;

						//This is the 3 component float vector that gets outputted to the screen
						//for each pixel
						out vec4 color;

						//out vec2 vertexPosition;

						//uniform float time;
						uniform sampler2D mySampler;

						void main() {

  							//get the color from the texture
							vec4 textureColor = texture(mySampler,fragmentUV);
							//use the color that was passed in to the fragment shader
							color = textureColor *fragmentColor;
						}
					)";

			const char* m_default_debug_vertex_shader = R"(#version 300 es
				//the vertex shader operates on each vertex

				//input data from the VBO. Each vertex is 2 floats
				in vec2 vertexPosition;
				in vec4 vertexColor;

				out vec2 fragmentPosition;
				out vec4 fragmentColor;

				uniform mat4 P;

				void main(){
				//set the x,y position on the screen
				gl_Position.xy=(P*vec4(vertexPosition,0.0,1.0)).xy;
				//the z position is zero since we are in 2D

				gl_Position.z=0.0;

				//Indicate that the coordinates are normalized
				gl_Position.w=1.0;

				fragmentPosition=vertexPosition;

				fragmentColor=vertexColor;
}
		)";

			const char* m_default_debug_fragment_shader = R"(#version 300 es
				precision mediump float;
				//The fragment shader operates on each pixel in a given polygon

				in vec2 fragmentPosition;
				in vec4 fragmentColor;

				//This is the 3 componet float vector that gets outputted to the screen
				//for each pixel.
				out vec4 color;
				void main(){
				color=fragmentColor;
				}
		)";
		};
	}
}
