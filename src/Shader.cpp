/**
*	@file Shader.cpp
*/
#include "Shader.h"
#include <vector>
#include <iostream>
#include <cstdint>
#include <stdio.h>
#include <sys/stat.h>

namespace Shader {

	/**
	*	�V�F�[�_�v���O�������쐬����
	*
	*	@param vsCode;
	*	@param fsCode;
	*
	*	@return �쐬�����v���O�����I�u�W�F�N�g
	*/
	ProgramPtr Program::Create(const char* vsFilename, const char* fsFilename) {

		struct Impl : Program { Impl() {}~Impl() {} };
		ProgramPtr p = std::make_shared<Impl>();
		if (!p) {
			std::cerr << "ERROR: �v���O����'" << fsFilename << "'�̍쐬�Ɏ��s" << std::endl;
			return {};
		}

		std::cout << "Compiling " << vsFilename << " and " << fsFilename << std::endl;

		p->program = CreateProgramFromFile(vsFilename, fsFilename);
		if (!p->program) {
			return {};
		}

		//�T���v���[�̐��ƈʒu���擾����
		GLint activeUniforms;
		glGetProgramiv(p->program, GL_ACTIVE_UNIFORMS, &activeUniforms);
		for (int i = 0; i < activeUniforms; ++i) {
			GLint size;
			GLenum type;
			GLchar name[128];
			glGetActiveUniform(p->program, i, sizeof(name), nullptr, &size, &type, name);
			if (type == GL_SAMPLER_2D) {
				p->samperCount = size;
				p->samplerLocation = glGetUniformLocation(p->program, name);
				if (p->samplerLocation < 0) {
					std::cerr << "ERROR: �v���O����'" << vsFilename << "'�̍쐬�Ɏ��s���܂���" << std::endl;
					return {};
				}
				break;
			}
		}

		p->viewIndexLocation = glGetUniformLocation(p->program, "viewIndex");
		p->depthSamplerLocation = glGetUniformLocation(p->program, "depthSampler");
		p->matVPLocation = glGetUniformLocation(p->program, "matVP");

		auto result = glGetError();


		//���_�V�F�[�_�t�@�C�����̖�������".vert"����菜�������̂��v���O�������Ƃ���
		p->name = vsFilename;
		p->name.resize(p->name.size() - 4);

		return p;
	}

	/**
	*	�f�X�g���N�^
	*/
	Program::~Program() {
		if (program) {
			glDeleteProgram(program);
		}
	}

	/**
	*	Uniform �u���b�N���o�C���f�B���O�E�|�C���g�Ɋ��蓖�Ă�
	*
	*	@param blockName	���蓖�Ă�Uniform�u���b�N�̖��O
	*	@param bindingPoint	������̃o�C���f�B���O�E�|�C���g
	*
	*	@retval	true	���蓖�Đ���
	*	@retval false	���蓖�Ď��s
	*/
	bool Program::UniformBlockBinding(const char* blockName, GLuint bindingPoint) {

		const GLuint blockIndex = glGetUniformBlockIndex(program, blockName);
		if (blockIndex == GL_INVALID_INDEX) {
			std::cerr << "ERROR(" << name << "): Uniform�u���b�N'" << blockName << "'��������܂���" << std::endl;
			return false;
		}

		glUniformBlockBinding(program, blockIndex, bindingPoint);
		const GLenum result = glGetError();
		if (result != GL_NO_ERROR) {
			std::cerr << "ERROR(" << name << "): Uniform�u���b�N'" << blockName << "'�̃o�C���h�Ɏ��s" << std::endl;
			return false;
		}

		//std::cout << "Program::UniformBlockBinding()" << "Successed" << " blockName:" << blockName << " bindingPoint:"<<bindingPoint << std::endl;
		return true;
	}

	/**
	*	�`��p�v���O�����ɐݒ肷��
	*/
	void Program::UseProgram() {

		glUseProgram(program);
		for (GLint i = 0; i < samperCount; ++i) {
			glUniform1i(samplerLocation + i, i);
		}

		if (depthSamplerLocation >= 0) {
			glUniform1i(depthSamplerLocation, 2);
		}
	}

	/**
	*	�e�N�X�`�����e�N�X�`���E�C���[�W���j�b�g�Ɋ��蓖�Ă�
	*
	*	@param unit		������̃e�N�X�`���E�C���[�W�E���j�b�g�ԍ�(GL_TExTURE0�`)
	*	@param type		���蓖�Ă�e�N�X�`���̎��(GL_TEXTURE_1D,GLTEXTURE_2D,etc)
	*	@param texure	���蓖�Ă�e�N�X�`���I�u�W�F�N�g
	*/
	void Program::BindTexture(GLenum unit, GLuint texture, GLenum type ) {

		if (unit >= GL_TEXTURE0 && unit < static_cast<GLenum>(GL_TEXTURE0 + samperCount)) {

			glActiveTexture(unit);
			glBindTexture(type, texture);
		}
	}

	/**
	*	�f�v�X�e�N�X�`�����e�N�X�`���E�C���[�W�E���j�b�g�Ɋ��蓖�Ă�
	*
	*	@param type		���蓖�Ă�e�N�X�`���̎�� (GL_TEXTURE_1D,GL_TEXTURE_2D,etc)
	*	@param texture	���蓖�Ă�e�N�X�`���I�u�W�F�N�g
	*/
	void Program::BindShadowTexture(GLenum type, GLuint texture) {
		if (depthSamplerLocation >= 0) {
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(type, texture);
		}
	}

	/**
	*	�g�p����J�����̃C���f�b�N�X���w�肷��
	*/
	void Program::SetViewIndex(int index) {

		if (viewIndexLocation >= 0) {
			glUniform1i(viewIndexLocation, index);
		}
	}

	/**
	*	�r���[�ˉe�ϊ��s���ݒ肷��
	*/
	void Program::SetViewProjectionMatrix(const glm::mat4& matVP) {

		this->matVP =  matVP;
		if (matVPLocation >= 0) {
			glUniformMatrix4fv(matVPLocation, 1, GL_FALSE, &matVP[0][0]);
		}
	}

	/**
	*	�x�N�g���^�̃p�����[�^��ݒ肷��
	*
	*	@param p	�p�����[�^
	*	@param n	�p�����[�^��
	*/
	void Program::SetVectorParameter(glm::vec3 p, std::string name) {
		GLint vecLocation = glGetUniformLocation(program, name.c_str());
		if (vecLocation > 0) {
			glUniform4fv(vecLocation, 1, glm::value_ptr(glm::vec4(p.x, p.y, p.z, 1.0f)));
		}
	}

	/**
	*	�u�[���^�̃p�����[�^��ݒ肷��
	*
	*	@param b	�p�����[�^
	*	@param name	�p�����[�^��
	*/
	void Program::SetBoolParameter(bool b, std::string name) {

		GLint bLocation = glGetUniformLocation(program, name.c_str());
		if (bLocation > 0) {
			glUniform1i(bLocation, b);
		}
	}

	/**
	*	float�^�p�����[�^��ݒ肷��
	*
	*	@param name �p�����[�^��
	*/
	void Program::SetFloatParameter(float f, std::string name){

		GLint fLocation = glGetUniformLocation(program, name.c_str());
		if (fLocation > 0) {
			glUniform1f(fLocation, f);
		}
	}

	/**
	*	�V�F�[�_�R�[�h���R���p�C������
	*
	*	@param type �V�F�[�_�̎��
	*	@param string �V�F�[�_�R�[�h�ւ̃|�C���^
	*
	*	@return �쐬�����V�F�[�_�I�u�W�F�N�g
	*/
	GLuint CompileShader(GLenum type, const GLchar* string) {

		GLuint shader = glCreateShader(type);
		glShaderSource(shader, 1, &string, nullptr);
		glCompileShader(shader);
		GLint compiled = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
		if (!compiled) {
			GLint infoLen = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
			if (infoLen) {
				std::vector<char> buf;
				buf.resize(infoLen);
				if (static_cast<int>(buf.size()) >= infoLen) {
					glGetShaderInfoLog(shader, infoLen, NULL, buf.data());
					std::cerr << "ERROR: �V�F�[�_�̃R���p�C���Ɏ��s\n" << buf.data() << std::endl;
				}

			}
			glDeleteShader(shader);
			return 0;
		}
		else {
			std::cout << "Successful." << std::endl;
		}
		return shader;
	}

	/**
	*	�v���O�����I�u�W�F�N�g���쐬����
	*
	*	@param vsCode ���_�V�F�[�_�R�[�h�ւ̃|�C���^
	*	@param fsCode �t���O�����g�V�F�[�_�R�[�h�ւ̃|�C���^
	*
	*	@return �쐬�����v���O�����I�u�W�F�N�g
	*/
	GLuint CreateShaderProgram(const GLchar* vsCode, const GLchar* fsCode) {

		std::cout << "	vertex shader... ";
		GLuint vs = CompileShader(GL_VERTEX_SHADER, vsCode);

		std::cout << "	fragment shader... ";
		GLuint fs = CompileShader(GL_FRAGMENT_SHADER, fsCode);


		if (!vs || !fs) {
			return 0;
		}

		GLuint program = glCreateProgram();
		glAttachShader(program, fs);
		glDeleteShader(fs);
		glAttachShader(program, vs);
		glDeleteShader(vs);
		glLinkProgram(program);
		GLint linkStatus = GL_FALSE;
		glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
		if (linkStatus != GL_TRUE) {
			GLint infoLen = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLen);
			if (infoLen) {
				std::vector<char> buf;
				buf.resize(infoLen);
				if (static_cast<int>(buf.size()) >= infoLen) {
					glGetProgramInfoLog(program, infoLen, NULL, buf.data());
					std::cerr << "ERROR: �V�F�[�_�̃����N�Ɏ��s\n" << buf.data() << std::endl;
				}
			}
			glDeleteProgram(program);
			return 0;
		}

		std::cout << std::endl;

		return program;
	}


	/**
	*	�t�@�C����ǂݍ���
	*
	*	@param filename �ǂݍ��ރt�@�C����
	*	@param buf		�ǂݍ��ݐ�o�b�t�@
	*
	*	@retval true	�ǂݍ��ݐ���
	*	@retval false	�ǂݍ��ݎ��s
	*/
	bool ReadFile(const char* filename, std::vector<char>&buf) {

		struct stat st;
		if (stat(filename, &st)) {
			return false;
		}

		FILE* fp = NULL;
		fopen_s(&fp, filename, "rb");
		if (!fp) {
			return false;
		}

		buf.resize(st.st_size + 1);
		const size_t readSize = fread(buf.data(), 1, st.st_size, fp);
		fclose(fp);
		if (readSize != st.st_size) {
			return false;
		}

		buf.back() = '\0';
		return true;

	}

	/**
	*	�t�@�C������V�F�[�_�v���O�������쐬����
	*
	*	@param vsCode ���_�V�F�[�_�t�@�C����
	*	@param fsCode �t���O�����g�V�F�[�_�t�@�C����
	*
	*	@return �쐬�����v���O�����I�u�W�F�N�g
	*/
	GLuint CreateProgramFromFile(const char* vsFilename, const char* fsFilename) {

		std::vector<char> vsBuf;
		if (!ReadFile(vsFilename, vsBuf)) {
			std::cerr << "ERROR in Shader::CreateProgramFromFile:\n" << vsFilename << "��ǂݍ��߂܂���" << std::endl;
			return 0;
		}

		std::vector<char> fsBuf;
		if (!ReadFile(fsFilename, fsBuf)) {
			std::cerr << "ERROR in Shader::CreateProgramFromFile:\n" << fsFilename << "��ǂݍ��߂܂���" << std::endl;
			return 0;
		}

		return CreateShaderProgram(vsBuf.data(), fsBuf.data());
	}

}


