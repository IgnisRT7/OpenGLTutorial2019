#pragma once
/**
*	@Texture.h
*/
#include <GL/glew.h>
#include <memory>

/**
*	�u�`�����Ƃ̈Ⴂ�܂Ƃ�
*
*	(Image2D, Image2DPtr) == (Texture, TexturePtr)
*	Image2D**Create(const char* path) == Texture::LoadFromFile(const char*)
*	�֐��ǉ� : Texture::Reset(LGuint)
*/

class Texture;
using TexturePtr = std::shared_ptr<Texture>;	///< �e�N�X�`���|�C���^

/**
*	�e�N�X�`���N���X
*/
class Texture {
public:
	
	static TexturePtr Create(int width, int height, GLenum iformat, GLenum format, const void* data);
	static TexturePtr LoadFromFile(const char*);
	GLuint Id() const { return texId; }
	GLsizei Width() const { return width; }
	GLsizei Height() const { return height; }
	void Reset(GLuint texId);

private:

	Texture() = default;
	~Texture();
	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;

	GLuint texId = 0;
	int width = 0;
	int height = 0;
};
