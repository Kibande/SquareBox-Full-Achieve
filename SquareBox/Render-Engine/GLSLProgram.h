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
		};
	}
}
