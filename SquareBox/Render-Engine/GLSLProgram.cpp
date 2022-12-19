#include "GLSLProgram.h"
#include <Asset-Manager/Asset-Manager.h>
namespace SquareBox {
	namespace RenderEngine {
		GLSLProgram::GLSLProgram() : _numAttributes(0), _programID(0), _vertexShaderID(0), _fragmentShaderID(0)
		{
		}

		GLSLProgram::~GLSLProgram()
		{
			dispose();
		}

		void GLSLProgram::compileShaders(const std::string & vertexShaderFilePath, const std::string & fragmentShaderFilePath)
		{
			std::string vertex_shader_string = "";
			std::string fragment_shader_string = "";

			std::pair<char *, int> vertextShaderfileInfo = SquareBox::AssetManager::IOManager::getRawDataFromFile(vertexShaderFilePath);
			char * vertex_shader_buffer = vertextShaderfileInfo.first;
			int total_vertex_shader_buffer_size = vertextShaderfileInfo.second;
			if (vertex_shader_buffer != nullptr) {
				std::string vex((const char*)vertex_shader_buffer, total_vertex_shader_buffer_size);
				delete[] vertex_shader_buffer;
				vertex_shader_string = vex;
			}

			std::pair<char *, int> fragmentShaderfileInfo = SquareBox::AssetManager::IOManager::getRawDataFromFile(fragmentShaderFilePath);
			char * fragment_shader_buffer = fragmentShaderfileInfo.first;
			int total_fragment_shader_buffer_size = fragmentShaderfileInfo.second;
			if (fragment_shader_buffer != nullptr) {
				std::string frag((const char*)fragment_shader_buffer, total_fragment_shader_buffer_size);
				delete[] fragment_shader_buffer;
				fragment_shader_string = frag;
			}

			SBX_GLCall(_programID = glCreateProgram());//create shader program
			SBX_GLCall(_vertexShaderID = glCreateShader(GL_VERTEX_SHADER));

			if (_vertexShaderID == 0) {
				SBX_CORE_CRITICAL("Vertex Failed to be created");
			}

			SBX_GLCall(_fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER));

			if (_fragmentShaderID == 0) {
				SBX_CORE_CRITICAL("Fragment Failed to be created");
			}

			//load our shaders
			compileShader(vertex_shader_string.c_str(), vertexShaderFilePath, _vertexShaderID);
			compileShader(fragment_shader_string.c_str(), fragmentShaderFilePath, _fragmentShaderID);
		}

		void GLSLProgram::compileShadersFromSource(const char *vertexSource, const char * fragmentSource)
		{
			SBX_GLCall(_programID = glCreateProgram());
			SBX_GLCall(_vertexShaderID = glCreateShader(GL_VERTEX_SHADER));

			if (_vertexShaderID == 0) {
				SBX_CORE_CRITICAL("Vertex Failed to be created");
			}

			SBX_GLCall(_fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER));

			if (_fragmentShaderID == 0) {
				SBX_CORE_CRITICAL("Fragment Failed to be created");
			}

			//load our shaders
			const std::string& vertexShader = "VertexShader";
			const std::string &fragmentShader = "FragmentShader";
			compileShader(vertexSource, vertexShader, _vertexShaderID);
			compileShader(fragmentSource, fragmentShader, _fragmentShaderID);
		}

		void GLSLProgram::addDefaultTextureAttributes()
		{
			addAttribute("vertexPosition");
			addAttribute("vertexColor");
			addAttribute("vertexUV");
		}

		void GLSLProgram::addDefaultDebugAttributes()
		{
			addAttribute("vertexPosition");
			addAttribute("vertexColor");
		}

		void GLSLProgram::compileDefaultTextureShaders()
		{
			compileShadersFromSource(m_default_texture_vertex_shader, m_default_texture_fragment_shader);
		}


		void GLSLProgram::compileDefaultDebugShaders()
		{
			compileShadersFromSource(m_default_debug_vertex_shader, m_default_debug_fragment_shader);
		}
		void GLSLProgram::linkShaders()
		{
			if (_programID == 0 || _vertexShaderID == 0 || _fragmentShaderID == 0) {
				SBX_CORE_CRITICAL("A programme should first be created,\n and the Shaders have to be complied before linking");
			}
			//Attach our shaders to our program
			SBX_GLCall(glAttachShader(_programID, _vertexShaderID));
			SBX_GLCall(glAttachShader(_programID, _fragmentShaderID));

			//Link our Program
			SBX_GLCall(glLinkProgram(_programID));
			SBX_GLCall(glValidateProgram(_programID));

			//Not the different functions here:glGetProgram* instead of glGetShader*
			GLint isLinked = 0;
			SBX_GLCall(glGetProgramiv(_programID, GL_LINK_STATUS, (int *)&isLinked));

			if (isLinked == GL_FALSE) {
				GLint length = 0;
				SBX_GLCall(glGetProgramiv(_programID, GL_INFO_LOG_LENGTH, &length));
				//construct our error message m_buffer since we now even know its' length
				char * message = (char*)alloca(length * sizeof(char));
				//populating our m_buffer
				SBX_GLCall(glGetProgramInfoLog(_programID, length, &length, message));

				//We don't need the program anymore.
				SBX_GLCall(glDeleteProgram(_programID));

				//Don't leak shaders either;
				SBX_GLCall(glDeleteShader(_vertexShaderID));
				SBX_GLCall(glDeleteShader(_fragmentShaderID));

				SBX_CORE_CRITICAL("Shaders  failed to Link! error {} ", message);
			}
			else {
				//Always detach then delete the shaders after a successfully linking.
				SBX_GLCall(glDeleteShader(_vertexShaderID));
				SBX_GLCall(glDeleteShader(_fragmentShaderID));
			}
		}

		void GLSLProgram::addAttribute(const std::string & attributeName)
		{
			SBX_GLCall(glBindAttribLocation(_programID, _numAttributes++, attributeName.c_str()));
		}

		GLint GLSLProgram::getUnifromLocation(const std::string & uniformName)
		{
			if (m_UniformLocationCache.find(uniformName) != m_UniformLocationCache.end())
			{
				return m_UniformLocationCache[uniformName];
			}
			else {
				SBX_GLCall(int location = glGetUniformLocation(_programID, uniformName.c_str()));

				if (location == -1 || location == GL_INVALID_INDEX) {
					SBX_CORE_CRITICAL("Uniform {}  Not Found in shader!", uniformName);
				}
				m_UniformLocationCache[uniformName] = location;
				return location;
			}
		}

		void GLSLProgram::use()
		{
			SBX_GLCall(glUseProgram(_programID));
			//enable all set attributes
			for (int i = 0; i < _numAttributes; i++)
			{
				SBX_GLCall(glEnableVertexAttribArray(i));
			}
		}

		void GLSLProgram::unuse()
		{
			SBX_GLCall(glUseProgram(0));
			//disable all set attributes
			for (int i = 0; i < _numAttributes; i++)
			{
				SBX_GLCall(glDisableVertexAttribArray(i));
			}
		}

		void GLSLProgram::compileShader(const char* source, const std::string& identifier, GLuint id)
		{
			SBX_GLCall(glShaderSource(id, 1, &source, nullptr));
			//actual shader compilation
			SBX_GLCall(glCompileShader(id));
			//get the compilation status , Error Checking
			GLint success = 0;
			SBX_GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &success));

			if (success == GL_FALSE) {
				GLint length = 0;
				SBX_GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
				//construct our error message m_buffer since we now even know its' length
				char * message = (char*)alloca(length * sizeof(char));
				//populating our m_buffer
				SBX_GLCall(glGetShaderInfoLog(id, length, &length, message));

				SBX_GLCall(glDeleteShader(id));
				SBX_CORE_CRITICAL("{} Shader failed to compile error {}", identifier, message);
				return;
			}
		}
		void GLSLProgram::dispose()
		{
			if (_programID) {
				SBX_GLCall(glDeleteProgram(_programID));
				_programID = 0;
			}
		}
	}
}