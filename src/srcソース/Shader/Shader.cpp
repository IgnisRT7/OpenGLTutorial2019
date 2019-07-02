/**
 *	@file Shader.cpp 
 *
*/

#include "Shader.h"
#include <vector>	
#include <iostream>
#include <cstdint>
#include <stdio.h>
#include <sys/stat.h>


namespace Shader{

/**
* シェーダコードをコンパイルする
*
* @param type	シェーダの種類
* @param string	シェーダコードへのポインタ
*
* @return 作成したシェーダオブジェクト
*/

GLuint CompileShader(GLenum type, const GLchar* string) {
	
	GLuint shader = glCreateShader(type);
	
	// 作成されたシェーダオブジェクトにシェーダコードを関連付け
	glShaderSource(shader, 1, &string, nullptr);

	// 関連付けられたシェーダコードがコンパイルされ、シェーダオブジェクトにセットされる
	glCompileShader(shader);
	
	/* ↓コンパイルに失敗した場合にその原因を表示する処理 */
	
	GLint compiled = 0;
	
	// シェーダオブジェクトの状態を取得 
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		GLint infoLen = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
		if (infoLen) {
			std::vector<char>buf;
			buf.resize(infoLen);
			if (static_cast<int>(buf.size()) >= infoLen) {

				//bufに詳細情報を取り込み、その内容を出力
				glGetShaderInfoLog(shader, infoLen, NULL, buf.data());
				std::cerr << "ERROR: シェーダのコンパイルに失敗\n" << buf.data() << std::endl;
			}
		}

		// シェーダオブジェクトを削除
		glDeleteShader(shader);
		return 0;
	}
	return shader;

}

/**
 * プログラムオブジェクトを作成する
 *
 * @param vsCode 頂点シェーダコードへのポインタ
 * @param fsCode フラグメントシェーダコードへのポインタ
 *
 * @return 作成したプログラムオブジェクト
 */
GLuint CreateShaderProgram(const GLchar* vsCode, const GLchar* fsCode) {
	
	GLuint vs = CompileShader(GL_VERTEX_SHADER, vsCode);
	GLuint fs = CompileShader(GL_FRAGMENT_SHADER, fsCode);

	if (!vs || !fs) {
		return 0;
	}

	GLuint program = glCreateProgram();
	
	// シェーダ王ジェクトをプログラムオブジェクトに割り当て
	glAttachShader(program, fs);
	glDeleteShader(fs);
	glAttachShader(program, vs);
	glDeleteShader(vs);

	// 割り当てたシェーダオブジェクト同士をリンク
	glLinkProgram(program);
	
	// ↓コンパイルに失敗した場合にその原因を表示する処理
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
				std::cerr << "ERROR: シェーダのリンクに失敗\n" << buf.data() << std::endl;
			}
		}
		glDeleteProgram(program);
		return 0;
	} 
	return program;
}

/**
 * ファイルを読み込む
 * 
 * @param filename	読み込むファイル名
 * @param buf		読み込み先バッファ
 * 
 * @retval	ture	読み込む成功
 * @retval	false	読み込み失敗
*/
bool ReadFile(const char* filename, std::vector<char>& buf){
	
	struct stat st;
	if(stat(filename, &st)){
		return false;
	}

	FILE* fp = fopen(filename, "rb");
	if(!fp){
		return false;
	}
	// +1　は終端を入れるためのスペース
	buf.resize(st.st_size + 1);

	const size_t readSize = fread(buf.data(), 1, st.st_size, fp);
	fclose(fp);
	
	if(readSize != st.st_size){
		return false;
	}
	// freadは自動的に終端を付けたしくれないので、明示的に \0 を追加
	buf.back() = '\0';
	return true;
}


/**
 * ファイルからシェーダプログラムを作成する
 * 
 * @param vsCode 頂点シェーダファイル名
 * @param fsCode フラグメントシェーダファイル名
 * 
 * @return		作成したプログラムオブジェクト
*/

GLuint CreateProgramFromFile(const char* vsFilename, const char* fsFilename){
	std::vector<char> vsBuf;
	if(!ReadFile(vsFilename, vsBuf)){
	/*	
		std::cerr << "ERROR in Shader::CreateProgramFromFile:\n" <<
			vsFilename << "を読み込めません。" << std::endl;
	*/
		return 0;
	}

	std::vector<char> fsBuf;
	if(!ReadFile(fsFilename, fsBuf)){
	/*
		std::cerr << "ERROR in Shader::CreateProgramFromFile:\n"<<
			fsFilename << "を読み込めません。" << std::end;
	*/
		return 0;
	}	

	return CreateShaderProgram(vsBuf.data(), fsBuf.data());

}

/**
*	シェーダプログラムを作成する
*
*	@param vsCode 頂点シェーダコードへのポインタ
*	@param fsCode フラグメントシェーダコードへのポインタ
*
*	@return 作成したプログラムオブジェクト
*/

ProgramPtr Program::Create(const char* vsFilename, const char* fsFilename) {

	struct Impl : Program {};
	ProgramPtr p = std::make_shared<Impl>();
	if (!p) {
		std::cerr << "Error: プログラム'" << vsFilename << "'の作成に失敗." << std::endl;
		return {};
	}

	p->program = CreateProgramFromFile(vsFilename, fsFilename);
	if (!p->program) {
		return{};
	}

	// サンプラーの数と位置を取得する
	GLint activeUniforms;
	glGetProgramiv(p->program, GL_ACTIVE_UNIFORMS, &activeUniforms);
	for (int i = 0; i < activeUniforms; ++i) {
		GLint size;
		GLenum type;
		GLchar name[128];
		glGetActiveUniform(p->program, i, sizeof(name),nullptr, &size, &type, name)	;
		if (type == GL_SAMPLER_2D) {
			p->samplerCount = size;
			p->samplerLocation = glGetUniformLocation(p->program, name);
			if (p->samplerLocation < 0) {
				std::cerr << "Error: プログラム'" << vsFilename << "'の作成に失敗." << std::endl;
				return{};
			}

			break;
		}

	}

	//頂点シェーダファイル名の末尾から".vert"を取り除いたものものをプログラム名とする
	p->name = vsFilename;
	p->name.resize(p->name.size() - 4);
	
	return p;

}


/**
*	デストラクタ
*/
Program::~Program() {
	if (program) {
		glDeleteProgram(program);
	}
}

/**
*	Uniform ブロックをバインディング・ポイントに割り当てる
*
*	@param blockname	割り当てるUniformブロックの名前
*	@param bindingPoint	割り当て先のバインディング・ポイント
*
*	@retval	ture	割り当て成功
*	@retval false	割り当て失敗
*/
bool Program::UniformBlockBinding(const char* blockname, GLuint bindingPoint) {
	const GLuint blockIndex = glGetUniformBlockIndex(program, blockname);
	if (blockIndex == GL_INVALID_INDEX) {
		std::cerr << "Error(" << name << "): Uniformブロック'" << blockname << "'が見つかりません" << std::endl;
		return false;
	}

	glUniformBlockBinding(program, blockIndex, bindingPoint);
	const GLenum result = glGetError();
	if (result != GL_NO_ERROR) {
		std::cerr << "Error(" << name << "): Uniformブロック'" << blockname << "'のバインドに失敗." << std::endl;
		return false;
	}
	return true;
}

/**
*	Uniform ブロックをバインディング・ポイントに割り当てる
*
*	@param object	割り当てるUniformBuffer オブジェクト
*
*	@retval true	割り当て成功
*	@retval false	割り当て失敗
*/

bool Program::UniformBlockBinding(const UniformBuffer& object){
	
	return UniformBlockBinding(object.Name().c_str(), object.BindingPoint());
}


void Program::UseProgram() {
	glUseProgram(program);
	for (GLint i = 0; i < samplerCount; ++i) {
		glUniform1i(samplerLocation + i, i);
	}
}

/**
*	テスクチャをテクスチャ・イメージ・ユニットに割り当てる
*
*	@param unit		割り当て先のテクスチャ・イメージ・ユニット番号(GL_TEXTURE0～)
*	@param type		割り当て先のテクスチャの種類(GL_TEXTURE_1D,GL_TEXTURE_2D,etc)
*	@param texture	割り当てるテクスチャオブジェクト
*/

void Program::BindTexture(GLenum unit, GLenum type, GLuint texture) {
	if (unit >= GL_TEXTURE0 && unit < static_cast<GLenum>(GL_TEXTURE0 + samplerCount)) {
		glActiveTexture(unit);
		glBindTexture(type, texture);
	}
}


}