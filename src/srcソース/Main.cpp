/**
* @file Main.cpp
*/

#include "GLFWEW/GLFWEW.h"
#include "Texture/Texture.h"
#include "Shader/Shader.h"
#include "OffscreenBuffer/OffscreenBuffer.h"
#include "glm/gtc/matrix_transform.hpp"
#include <iostream>
#include <vector>

// 頂点データOff
struct Vertex {
	glm::vec3 position;	// 座標
 	glm::vec4 color;	// 色
	glm::vec2 texCoord;	// テクスチャ座標
};

// 頂点データ
const Vertex vertices[] = {
	{ {-0.5f, -0.3f, 0.5f }, {1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f } },
	{ { 0.3f, -0.3f, 0.5f }, {1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f } },
	{ { 0.3f,  0.5f, 0.5f }, {1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f } },
	{ {-0.5f,  0.5f, 0.5f }, {1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f } },

	{ {-0.3f,  0.3f, 0.1f }, {0.0f, 0.0f, 1.0f, 1.0f }, { 0.0f, 1.0f } },
	{ {-0.3f, -0.5f, 0.1f }, {0.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f } },
	{ { 0.5f, -0.5f, 0.1f }, {0.0f, 0.0f, 1.0f, 1.0f }, { 1.0f, 0.0f } },
	{ { 0.5f, -0.5f, 0.1f }, {1.0f, 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f } },
	{ { 0.5f,  0.3f, 0.1f }, {1.0f, 1.0f, 0.0f, 1.0f }, { 1.0f, 1.0f } },
	{ {-0.3f,  0.3f, 0.1f }, {1.0f, 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f } },

	{ {-1.0f, -1.0f, 0.5f }, {1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f } },
	{ { 1.0f, -1.0f, 0.5f }, {1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f } },
	{ { 1.0f,  1.0f, 0.5f }, {1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f } },
	{ {-1.0f,  1.0f, 0.5f }, {1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f } },

};

// インデックスデータ
const GLuint indices[] = {
	0, 1, 2, 2, 3, 0,
	4, 5, 6, 7, 8, 9,
	10, 11, 12, 12, 13, 10,

};

// 頂点シェーダのパラメータ型
struct VertexData
{
	glm::mat4 matMVP;
	glm::vec4 lightPosition;
	glm::vec4 lightColor;
	glm::vec4 ambientColor;

};


/**
 * 部分描画データ
 *
 */
struct RenderingPart {
	GLvoid * offset;	// 描画開始インデックスのバイトオフセット
	GLsizei size;		// 描画するインデックス数
 };

/**
* RenderingPart を作成する
*
* @param offset 描画開始インデックスのオフセット(インデックス単位)
* @param size	描画するインデックス数
*
* @return	作成した部分描画オブジェクト
*/

constexpr RenderingPart MakeRenderingPart(GLsizei offset, GLsizei size) {
	return { reinterpret_cast<GLvoid*>(offset * sizeof(GLuint)), size };
}
	
/**
*	部分描画データリスト
*/
static const RenderingPart renderingParts[] = {
	MakeRenderingPart(0 ,12),
	MakeRenderingPart(12, 6),
};



/**
* Vertex Buffer Object を作成する
*
* @param size	頂点データのサイズ
* @param data	頂点データへのポインタ
*
* @return 作成した VBO
*/

GLuint CreateVBO(GLsizeiptr size, const GLvoid* data) {
	GLuint vbo = 0;
	// OpenGLが管理するメモリ領域を管理するオブジェクトを作成
	glGenBuffers(1, &vbo);

	// バッファオブジェクトを特定の用途に割り当てる
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	// バッファオブジェクトにデータを転送する
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

	// バッファオブジェクトの第２引数を０で渡すことで割り当てを解除
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return vbo;
}

/**
* Index Buffer Object を作成する
*
* @param size	インデックスデータのサイズ
* @param data	インデックスデータへのポインタ
*
* @return 作成したIBO
*/

GLuint CreateIBO(GLsizeiptr size, const GLvoid* data) {
	GLuint ibo = 0;
	glGenBuffers(1, &ibo);
	// GL_ELEMENT_ARRAY_BUFFER ->インデックスデータを示す定数
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	return ibo;

}


/**
* 頂点アトリビュートを設定する
*
* @param index	頂点アトリビュートのインデックス
* @param cls	頂点のデータ型名
* @oaram mbr	頂点アトリビュートに設定するclsのメンバ変数
*/
#define SetVertexAttribPointer(index, cls, mbr) SetVertexAttribPointerI( \
	index, \
	sizeof(cls::mbr) / sizeof(float), \
	sizeof(cls) ,\
	reinterpret_cast<GLvoid*>(offsetof(cls, mbr)))

void SetVertexAttribPointerI(
	GLuint index, GLint size, GLsizei stride, const GLvoid* pointer) {

	// 指定した頂点アトリビュートを有効にする
	glEnableVertexAttribArray(index);

	// 頂点データのどの部分に頂点シェーダの入力先を割り当てるかを設定する
	glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, stride, pointer);
}

/**
* Vertex Array Object を作成する
*
* @param vbo	VAOに関連付けられるVBO
*
* @return 作成したVAO
*/
GLuint CreateVAO(GLuint vbo, GLuint ibo) {
	GLuint vao = 0;

	// VAOの作成
	glGenVertexArrays(1, &vao);

	// 指定されたVAOをOpenGL「現在の処理対象」に設定する
	glBindVertexArray(vao);

	// 頂点アトリビュートを設定するには、事前に対応するVBOを割り当てる
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);


	SetVertexAttribPointer(0, Vertex, position);
	SetVertexAttribPointer(1, Vertex, color);
	SetVertexAttribPointer(2, Vertex, texCoord);

	// VAOのバインドを解除
	glBindVertexArray(0);
	// バッファオブジェクトの削除
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ibo);


	return vao;
}

/**
*	Uniform Block Object を作成する
*
*	@param size Uniform Block のサイズ
*	@param data Uniform Block に転送するデータへのポインタ
*
*	@return 作成したUBO
*/

GLuint CreateUBO(GLsizeiptr size, const GLvoid* data = nullptr) {
	
	GLuint ubo;
	glGenBuffers(1, &ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferData(GL_UNIFORM_BUFFER, size, data, GL_STREAM_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	return ubo;
}


// エントリーポイント
int main(){

	GLFWEW::Window& window = GLFWEW::Window::Instance();
	if(!window.Init(800, 600, "OpenGL Tutorial")){
		return 1;
	}

	// オブジェクトの作成
	const GLuint vbo = CreateVBO(sizeof(vertices), vertices);
	const GLuint ibo = CreateIBO(sizeof(indices), indices);
	const GLuint vao = CreateVAO(vbo, ibo);
	const GLuint ubo = CreateUBO(sizeof(VertexData));

	//const GLuint shaderProgram = Shader::CreateProgramFromFile("Res/Tutorial.vert", "Res/Tutorial.frag");
	const Shader::ProgramPtr progTutorial = Shader::Program::Create("Res/Tutorial.vert", "Res/Tutorial.frag");


	if (!vbo | !ibo | !vao | !ubo | !progTutorial) {
		return 1;
	}

	progTutorial->UniformBlockBinding("VertexData", 0);

	
	// テクスチャデータ
	TexturePtr tex = Texture::LoadFromFile("Res/sample1.bmp");

	if(!tex){
		return 1;
	}
	const OffscreenBufferPtr offscreen = OffscreenBuffer::Create(800, 600);

	//OpenGLが持つさまざまな機能を有効にする関数
	glEnable(GL_DEPTH_TEST);

	// メインループ
	while(!window.ShouldClose()){
		glBindFramebuffer(GL_FRAMEBUFFER, offscreen->GetFramebuffer());

		// glClear関数でバックバッファを消去するときに使われる色の指定
		glClearColor(0.0f , 0.3f, 0.5f, 1.0f);
		
		// 実際にバックバッファを消去する関数 引数には消去するバッファの種類を表す
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//視点を回転移動させる
		static float degree = 0.0f;
		degree += 0.1f;
		if (degree >= 360.0f) { degree -= 360.0f; }
		const glm::vec3 viewPos = glm::rotate(
			glm::mat4(), glm::radians(degree), glm::vec3(0, 1, 0)) * glm::vec4(2, 3, 3, 1);
	

		/* 頂点データの描画 */
		// 描画に使用するプリグラムオブジェクトの設定//
		progTutorial->UseProgram();
		 
		/* 座標変換行列を作成してシェーダに転送する */

		const glm::mat4x4 matProj =
		// 透視投影を行なう座標変換行列を作成
		glm::perspective(glm::radians(45.0f), 800.0f /600.0f, 0.1f, 100.0f);
		const glm::mat4x4 matView =
		// ワールド座標からビュー座標への変換行列の作成
		glm::lookAt(viewPos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
 
		VertexData vertexData;
		vertexData.matMVP = matProj * matView;
		vertexData.lightPosition = glm::vec4(1, 1, 1, 1);
		vertexData.lightColor = glm::vec4(2, 2, 2, 1);
		vertexData.ambientColor = glm::vec4(0.05f, 0.1f, 0.2f, 1);
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(VertexData), &vertexData);


		progTutorial->BindTexture(GL_TEXTURE0, GL_TEXTURE_2D, tex->Id());


		// 描画に使用するVAOを設定する
		glBindVertexArray(vao);

		// インデックスデータによる描画関数
		glDrawElements(
			GL_TRIANGLES, renderingParts[0].size,
			GL_UNSIGNED_INT, renderingParts[0].offset);

		glBindFramebuffer(GL_FRAMEBUFFER,0);
		glClearColor(0.5f, 0.3f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		progTutorial->BindTexture(GL_TEXTURE0, GL_TEXTURE_2D, offscreen->GetTexture());

		vertexData = {};
		vertexData.ambientColor = glm::vec4(1, 1, 1, 1);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(VertexData), &vertexData);

		glDrawElements(
			GL_TRIANGLES, renderingParts[1].size,
			GL_UNSIGNED_INT, renderingParts[1].offset);

		window.SwapBuffers();

	}

	// UBOの削除
	glDeleteBuffers(1, &ubo);


	// VAOを削除する。作成時と応用に削除する個数を指定できます。
	glDeleteVertexArrays(1, &vao);



	return 0;

}