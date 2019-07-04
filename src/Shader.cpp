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
		GLuint vs = Compile(GL_VERTEX_SHADER, vsCode);
		GLuint fs = Compile(GL_FRAGMENT_SHADER, fsCode);
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
					std::cerr << "ERROR: シェーダのリンクに失敗\n" << buf.data() << std::endl;
				}
			}
			glDeleteProgram(program);
			return 0;
		}
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
			std::cerr << "ERROR: " << path << " を開けません\n";
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
		const std::vector<GLchar> vsCode = ReadFile(vsPath);
		const std::vector<GLchar> fsCode = ReadFile(fsPath);
		return Build(vsCode.data(), fsCode.data());
	}

	/**
	* ライトリストを初期化する.
	*
	* 全ての光源の明るさを0にする.
	*/
	void LightList::Init()
	{
		ambient.color = glm::vec3(0);
		directional.color = glm::vec3(0);
		for (int i = 0; i < 8; ++i) {
			point.color[i] = glm::vec3(0);
		}
		for (int i = 0; i < 4; ++i) {
			spot.color[i] = glm::vec3(0);
		}
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
		lights.Init();
	}

	/**
	* コンストラクタ.
	*
	* @param id プログラム・オブジェクトのID.
	*/
	Program::Program(GLuint programId)
	{
		lights.Init();
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
			locPointLightPos = -1;
			locPointLightCol = -1;
			locDirLightDir = -1;
			locDirLightCol = -1;
			locAmbLightCol = -1;
			locSpotLightPos = -1;
			locSpotLightDir = -1;
			locSpotLightCol = -1;
			return;
		}

		locMatModel = glGetUniformLocation(id, "matModel");
		locMatMVP = glGetUniformLocation(id, "matMVP");

		locPointLightPos = glGetUniformLocation(id, "pointLight.position");
		locPointLightCol = glGetUniformLocation(id, "pointLight.color");
		locDirLightDir = glGetUniformLocation(id, "directionalLight.direction");
		locDirLightCol = glGetUniformLocation(id, "directionalLight.color");
		locAmbLightCol = glGetUniformLocation(id, "ambientLight.color");
		locSpotLightPos = glGetUniformLocation(id, "spotLight.posAndInnerCutOff");
		locSpotLightDir = glGetUniformLocation(id, "spotLight.dirAndCutOff");
		locSpotLightCol = glGetUniformLocation(id, "spotLight.color");

		const GLint texColorLoc = glGetUniformLocation(id, "texColor");
		if (texColorLoc >= 0) {
			glUseProgram(id);
			glUniform1i(texColorLoc, 0);
			glUseProgram(0);
		}
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
	* 描画に使われるライトを設定する.
	*
	* @param lights 設定するライト.
	*
	* この関数を使う前に、Use()を実行しておくこと.
	*/
	void Program::SetLightList(const LightList& lights)
	{
		this->lights = lights;

		// ライトの色情報をGPUメモリに転送する.
		if (locAmbLightCol >= 0) {
			glUniform3fv(locAmbLightCol, 1, &lights.ambient.color.x);
		}
		if (locDirLightCol >= 0) {
			glUniform3fv(locDirLightCol, 1, &lights.directional.color.x);
		}
		if (locPointLightCol >= 0) {
			glUniform3fv(locPointLightCol, 8, &lights.point.color[0].x);
		}
		if (locSpotLightCol >= 0) {
			glUniform3fv(locSpotLightCol, 4, &lights.spot.color[0].x);
		}
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

} // namespace Shader