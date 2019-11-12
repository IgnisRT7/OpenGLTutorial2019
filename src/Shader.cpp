/**
* @file Shader.cpp
*/
#include "Shader.h"
#include "Geometry.h"
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <iostream>
#include <fstream>

namespace Shader {

	/**
	* シェーダコードをコンパイルする.
	*
	* @param type   シェーダの種類.
	* @param string シェーダコードへのポインタ.
	*
	* @return 作成したシェーダオブジェクト.
	*/
	GLuint Compile(GLenum type, const GLchar* string)
	{
		if (!string) {
			return 0;
		}

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
					std::cerr << "ERROR: シェーダのコンパイルに失敗\n" << buf.data() << std::endl;
				}
			}
			glDeleteShader(shader);
			return 0;
		}
		return shader;
	}

	/**
	* プログラムオブジェクトを作成する.
	*
	* @param vsCode 頂点シェーダコードへのポインタ.
	* @param fsCode フラグメントシェーダコードへのポインタ.
	*
	* @return 作成したプログラムオブジェクト.
	*/
	GLuint Build(const GLchar* vsCode, const GLchar* fsCode)
	{

		std::cout << "\tcompile vs shader..." << std::endl;
		GLuint vs = Compile(GL_VERTEX_SHADER, vsCode);
		std::cout << "\tcompile fs shader..." << std::endl;
		GLuint fs = Compile(GL_FRAGMENT_SHADER, fsCode);
		if (!vs || !fs) {
			return 0;
		}
		std::cout << "\tsuccesful compilation." << std::endl <<"\tlink status...";

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
					std::cerr << "ERROR: シェーダのリンクに失敗\n" << buf.data() << std::endl;
				}
			}
			glDeleteProgram(program);
			return 0;
		}

		std::cout << "successful." << std:: endl;
		return program;
	}

	/**
	* ファイルを読み込む.
	*
	* @param path 読み込むファイル名.
	*
	* @return 読み込んだデータ.
	*/
	std::vector<GLchar> ReadFile(const char* path)
	{
		std::basic_ifstream<GLchar> ifs;
		ifs.open(path, std::ios_base::binary);
		if (!ifs.is_open()) {
			std::cerr << "[ERROR]: Shader ReadFile\n\t" << path << " を開けません\n";
			return {};
		}
		ifs.seekg(0, std::ios_base::end);
		const std::streamoff length = ifs.tellg();
		ifs.seekg(0, std::ios_base::beg);
		std::vector<GLchar> buf((size_t)length);
		ifs.read(buf.data(), length);
		buf.push_back('\0');
		return buf;
	}

	/**
	* ファイルからプログラム・オブジェクトを作成する.
	*
	* @param vsPath 頂点シェーダーファイル名.
	* @param fsPath フラグメントシェーダーファイル名.
	*
	* @return 作成したプログラム・オブジェクト.
	*/
	GLuint BuildFromFile(const char* vsPath, const char* fsPath)
	{
		std::cout << "[Info]: Program::BuildFromFile " << std::endl <<
			"\tvs: " << vsPath << "\tfs: " << fsPath << std::endl;
		
		const std::vector<GLchar> vsCode = ReadFile(vsPath);
		const std::vector<GLchar> fsCode = ReadFile(fsPath);
		return Build(vsCode.data(), fsCode.data());
	}

	/**
	*	プログラムオブジェクトを作成する
	*
	*	@param vspath	頂点シェーダーファイル名
	*	@param fsPath	フラグメントシェーダーファイル名
	*
	*	@return 作成したプログラムオブジェクト
	*/
	ProgramPtr Program::Create(const char* vsPath, const char* fsPath)
	{
		return std::make_shared<Program>(BuildFromFile(vsPath, fsPath));
	}

	/**
	* コンストラクタ.
	*/
	Program::Program()
	{

	}

	/**
	* コンストラクタ.
	*
	* @param id プログラム・オブジェクトのID.
	*/
	Program::Program(GLuint programId)
	{
		Reset(programId);
	}

	/**
	* デストラクタ.
	*
	* プログラム・オブジェクトを削除する.
	*/
	Program::~Program()
	{
		glDeleteProgram(id);
	}

	/**
	* プログラム・オブジェクトを設定する.
	*
	* @param id プログラム・オブジェクトのID.
	*/
	void Program::Reset(GLuint programId)
	{
		glDeleteProgram(id);
		id = programId;
		if (id == 0) {
			locMatMVP = -1;
			locMatModel = -1;

			locPointLightCount = -1;
			locPointLightIndex = -1;
			locSpotLightCount = -1;
			locSpotLightIndex = -1;
			return;
		}

		locMatModel = glGetUniformLocation(id, "matModel");
		locMatMVP = glGetUniformLocation(id, "matMVP");

		locPointLightCount = glGetUniformLocation(id, "pointLightCount");
		locPointLightIndex = glGetUniformLocation(id, "pointLightIndex");
		locSpotLightCount = glGetUniformLocation(id, "spotLightCount");
		locSpotLightIndex = glGetUniformLocation(id, "spotLightIndex");
		locCameraPosition = glGetUniformLocation(id, "cameraPosition");
		locTime = glGetUniformLocation(id, "time");

		glUseProgram(id);
		const GLint texColorLoc = glGetUniformLocation(id, "texColor");

		if (texColorLoc >= 0) {
			glUniform1i(texColorLoc, 0);
		}

		//テクスチャリストのリセット処理
		for (GLint i = 0; i < 8; ++i) {
			std::string name("texColorArray[");
			name += static_cast<char>('0' + i);
			name += ']';
			const GLint texColorLoc = glGetUniformLocation(id, name.c_str());
			if (texColorLoc >= 0) {
				glUniform1i(texColorLoc, i);
			}
		}

		//法線テクスチャリストのリセット処理
		for (GLint i = 0; i < 8; ++i) {
			std::string name("texNormalArray[");
			name += static_cast<char>('0' + i);
			name += ']';
			const GLint texColorLoc = glGetUniformLocation(id, name.c_str());
			if (texColorLoc >= 0) {
				glUniform1i(texColorLoc, i + 8);
			}
		}

		const GLint locTexPointLightIndex = glGetUniformLocation(id, "texPointLightIndex");
		if (locTexPointLightIndex >= 0) {
			glUniform1i(locTexPointLightIndex, 4);
		}
		const GLint locTexSpotLightIndex = glGetUniformLocation(id, "texSpotLightIndex");
		if (locTexSpotLightIndex >= 0) {
			glUniform1i(locTexSpotLightIndex, 4);
		}

		const GLint locTexCubeMap = glGetUniformLocation(id, "texCubeMap");
		if (locTexCubeMap >= 0) {
			glUniform1i(locTexCubeMap, 6);
		}

		glUseProgram(0);
	}

	/**
	* プログラム・オブジェクトが設定されているか調べる.
	*
	* @retval true  設定されている.
	* @retval false 設定されていない.
	*/
	bool Program::IsNull() const
	{
		return id == 0;
	}

	/**
	* プログラム・オブジェクトをグラフィックス・パイプラインに割り当てる.
	*
	* プログラム・オブジェクトを使い終わったらglUseProgram(0)を実行して解除すること.
	*/
	void Program::Use()
	{
		glUseProgram(id);
	}

	/**
	* 描画に使用するVAOを設定する.
	*
	* @param vao 設定するVAOのID.
	*/
	void Program::BindVertexArray(GLuint vao)
	{
		glBindVertexArray(vao);
	}

	/**
	* 描画に使用するテクスチャを設定する.
	*
	* @param unitNo 設定するテクスチャ・イメージ・ユニットの番号(0～).
	* @param texId  設定するテクスチャのID.
	*/
	void Program::BindTexture(GLuint unitNo, GLuint texId)
	{
		glActiveTexture(GL_TEXTURE0 + unitNo);
		glBindTexture(GL_TEXTURE_2D, texId);
	}

	/**
	* 描画に使われるビュー・プロジェクション行列を設定する.
	*
	* @param matVP 設定するビュー・プロジェクション行列.
	*/
	void Program::SetViewProjectionMatrix(const glm::mat4& matVP)
	{
		this->matVP = matVP;
		if (locMatMVP >= 0) {
			glUniformMatrix4fv(locMatMVP, 1, GL_FALSE, &matVP[0][0]);
		}
	}

	/**
	*	描画に使われるモデル行列を設定する
	*
	*	@param m	設定するモデル行列
	*/
	void Program::SetModelMatrix(const glm::mat4& m){

		if (locMatModel >= 0) {
			glUniformMatrix4fv(locMatModel, 1, GL_FALSE, &m[0][0]);
		}
	}

	/**
	*	描画に使われるライトの数を設定する
	*
	*	@param count		描画に使用するポイントライトの数(0-8)
	*	@param indexList	描画に使用するポイントライト番号の配列
	*/
	void Program::SetPointLightIndex(int count, const int* indexList) {

		if (locPointLightCount >= 0) {
			glUniform1i(locPointLightCount, count);
		}

		if (locPointLightIndex >= 0 && count > 0) {
			glUniform1iv(locPointLightIndex, count, indexList);
		}
	}

	/**
	*	描画に使われるライトを設定する
	*
	*	@param count		描画に使用するスポットライトの数(0-8)
	*	@param indexList	描画に使用するスポットライト番号の配列
	*/
	void Program::SetSpotLightIndex(int count, const int* indexList) {

		if (locSpotLightCount >= 0) {
			glUniform1i(locSpotLightCount, count);
		}
		if (locSpotLightIndex >= 0) {
			glUniform1iv(locSpotLightIndex, count, indexList);
		}
	}

	/**
	*	カメラ座標を設定する
	*
	*	@param pos	カメラ座標
	*/
	void Program::SetCameraPosition(const glm::vec3& pos) {
	
		if (locCameraPosition) {
			glUniform3fv(locCameraPosition, 1, &pos.x);
		}
	}

	/**
	* 総経過時間を設定する.
	*
	* @param time 総経過時間.
	*/
	void Program::SetTime(float time){
		
		if (locTime >= 0) {
			glUniform1f(locTime, time);
			
		}
	}



} // namespace Shader