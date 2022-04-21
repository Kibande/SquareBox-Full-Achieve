#include "IGameScreen.h"

namespace SquareBox {
	void IGameScreen::preUpdateShader(SquareBox::RenderEngine::GLSLProgram * program, std::string textureUnifromName)
	{
		//Upload texture Uniform
		//Tell that we want to use texture unit 0
		SBX_GLCall(glActiveTexture(GL_TEXTURE0));

		//Make sure the shader uses texture 0
		GLint textureUniform = program->getUnifromLocation(textureUnifromName);
		SBX_GLCall(glUniform1i(textureUniform, 0));//the upload
	}

	void IGameScreen::uploadCameraInfo(SquareBox::RenderEngine::GLSLProgram * program, SquareBox::Camera::ParallelCamera * camera, std::string cameraUniformName)
	{
		// The Camera matrix
		glm::mat4 ModalViewProjectionMatrix = camera->getModalViewProjectionMatrix();
		GLint pUniform = program->getUnifromLocation(cameraUniformName);
		SBX_GLCall(glUniformMatrix4fv(pUniform, 1, GL_FALSE, &ModalViewProjectionMatrix[0][0]));//the upload
	}
}