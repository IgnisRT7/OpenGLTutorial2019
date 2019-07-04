/**
* @file Texture.h
*/
#ifndef TEXTURE_H_INCLUDED
#define TEXTURE_H_INCLUDED
#include <GL/glew.h>
#include <memory>
#include <string>
#include <glm/glm.hpp>
#include <vector>

namespace Texture {

	class Image2D;
	using Image2DPtr = std::shared_ptr<Image2D>;

	/**
	*	画像データ
	*/
	class ImageData {
	public:
		glm::vec4 GetColor(int x, int y) const;

	public:
		GLint width = 0;			///< 横のピクセル数
		GLint height = 0;			///< 縦のピクセル数
		GLenum format = GL_NONE;	///< 記録されている色の種類
		GLenum type = GL_NONE;		///< それぞれの色情報のビット配置
		std::vector<uint8_t> data;	///< バイトデータ
	};


	GLuint CreateImage2D(GLsizei width, GLsizei height, const GLvoid* data, GLenum format, GLenum type);
	GLuint LoadImage2D(const char* path);

	/**
	*	画像クラス
	*/
	class Image2D {
	public:

		static Image2DPtr Create(const char*);

		Image2D() = default;
		explicit Image2D(GLuint texId);
		~Image2D();
		void Reset(GLuint texId);
		bool IsNull() const;
		GLuint Get() const;
		GLint Width() const { return width; }
		GLint Height() const { return height; }
		const std::string& Name() const { return name; }
		void Name(std::string& s) { name = s; }

	private:

		std::string name;
		GLuint id = 0;
		GLint width = 0;
		GLint height = 0;
	};

} // namespace Texture

#endif // TEXTURE_H_INCLUDED