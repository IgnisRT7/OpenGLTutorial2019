/**
*	@file Shader.h
*/
#pragma once

#include <GL/glew.h>
#include <string>
#include <memory>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


namespace Shader {

	class Program;
	using ProgramPtr = std::shared_ptr<Program>;	///< �v���O�����I�u�W�F�N�g�̃|�C���^

	GLuint CreateProgramFromFile(const char* vsFilename, const char* fsFilename);

	/**
	*	�V�F�[�_�v���O�����N���X
	*/
	class Program {
	public:

		static ProgramPtr Create(const char* vsFilename, const char* fsFilename);

		bool UniformBlockBinding(const char* blocblockName, GLuint bindingPoint);
		void UseProgram();
		void BindTexture(GLenum unit, GLenum texture, GLuint type = GL_TEXTURE_2D);


		void BindShadowTexture(GLenum type, GLuint texture);
		void SetViewIndex(int index);

		void SetViewProjectionMatrix(const glm::mat4& matVP);
		void SetVectorParameter(glm::vec3 p, std::string name);
		void SetBoolParameter(bool b, std::string name);
		void SetFloatParameter(float f, std::string name);
		bool IsNull() { return program == 0; }

	private:
		Program() = default;
		~Program();
		Program(const Program&) = delete;
		Program& operator=(const Program&) = delete;

	private:

		GLuint program = 0;			///< �v���O�����I�u�W�F�N�g
		GLint samplerLocation = -1;	///< �T���v���[�̈ʒu
		int samperCount = 0;		///< �T���v���[�̐�
		GLint viewIndexLocation = 1;///< �n�_�C���f�b�N�X�̈ʒu
		GLuint depthSamplerLocation = -1;
		GLuint matVPLocation = -1;	///< �r���[�ˉe�ϊ��p�s��̈ʒu
		std::string name;			///< �v���O������

		glm::mat4 matVP;
	};

}