/**
*	@file FramebufferObject.cpp
*/
#include "FramebufferObject.h"

/**
*	�t���[���o�b�t�@�I�u�W�F�N�g���쐬����
*
*	@param w	�t���[���o�b�t�@�I�u�W�F�N�g�̕�(�s�N�Z���P��)
*	@param h	�t���[���o�b�t�@�I�u�W�F�N�g�̍���(�s�N�Z���P��)
*	@param internalFormat	�J���[�o�b�t�@�̃s�N�Z���E�t�H�[�}�b�g
*	@param type				�t���[���o�b�t�@�̎��
*/
FramebufferObjectPtr FramebufferObject::Create(int w, int h, GLenum internalFormat, FrameBufferType type){

	FramebufferObjectPtr fbo = std::make_shared<FramebufferObject>();
	if (!fbo) {
		return nullptr;
	}

	//�e�N�X�`�����쐬����
	if (type != FrameBufferType::DepthOnly) {
		GLenum imageType = GL_UNSIGNED_BYTE;
		if (internalFormat == GL_RGBA16F) {
			imageType = GL_HALF_FLOAT;
		}
		else if (internalFormat == GL_RGBA32F) {
			imageType = GL_FLOAT;
		}

		fbo->texColor = std::make_shared<Texture::Image2D>(Texture::CreateImage2D(w, h, nullptr, GL_RGBA, imageType, internalFormat));
		fbo->texColor->SetWrapMode(GL_CLAMP_TO_EDGE);
	}
	if (type != FrameBufferType::ColorOnly) {
		fbo->texDepth = std::make_shared <Texture::Image2D>(Texture::CreateImage2D(w, h, nullptr, GL_DEPTH_COMPONENT, GL_FLOAT, GL_DEPTH_COMPONENT32F));
		fbo->texColor->SetWrapMode(GL_CLAMP_TO_EDGE);
	}

	//�t���[���o�b�t�@���쐬����
	glGenFramebuffers(1, &fbo->id);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo->id);

	if (fbo->texColor) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, fbo->texColor->Target(), fbo->texColor->Get(), 0);
	}
	if (fbo->texDepth) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, fbo->texDepth->Target(), fbo->texDepth->Get(), 0);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return fbo;
}

/**
*	�f�X�g���N�^
*/
FramebufferObject::~FramebufferObject(){

	if (id) {
		glDeleteFramebuffers(1, &id);
	}
}

/**
*
*	FBO��ID���擾����
*
*	@return FBO��ID
*/
GLuint FramebufferObject::GetFramebuffer() const{

	return id;
}

const Texture::Image2DPtr& FramebufferObject::GetColorTexture() const{

	return texColor;
}

const Texture::Image2DPtr& FramebufferObject::GetDepthTexture() const{

	return texDepth;
}
