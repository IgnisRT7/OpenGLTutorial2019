/***
*	@file Mesh.h
*/
#ifndef MESH_H_INCLUDED
#define MESH_H_INCLUDED

#include <GL/glew.h>
#include "BufferObject.h"
#include "Texture.h"
#include "Shader.h"
#include <glm/glm.hpp>
#include <vector>
#include <unordered_map>
#include <string>
#include <memory>
#include "json11/json11.hpp"

namespace Mesh {

	struct Mesh;
	using MeshPtr = std::shared_ptr<Mesh>;
	class Buffer;
	using MeshBufferPtr = std::shared_ptr<?>;

	//スケルタルメッシュ用先行宣言
	struct Node;
	struct ExtendedFile;
	using ExtendedFilePtr = std::shared_ptr<ExtendedFile>;
	class SkeletalMesh;
	using SkeletalMeshPtr = std::shared_ptr<SkeletalMesh>;

	struct Vertex {
		glm::vec3 position;
		glm::vec2 texCoord;
		glm::vec3 normal;
	};

	/**
	*	プリミティブの材質
	*/
	struct Material {
		glm::vec4 baseColor = glm::vec4(1);	/// 色データ
		Texture::Image2DPtr texture;		/// テクスチャデータ
		Shader::ProgramPtr program;			/// 使用するプログラム
		Shader::ProgramPtr progSkeltalMesh;	/// スケルタルメッシュ用プログラム
		
	};

	/**
	*	頂点データの描画パラメータ
	*/
	struct Primitive {
		GLenum mode;
		GLsizei count;
		GLenum type;
		const GLvoid* indices;
		GLint baseVertex = 0;
		std::shared_ptr<VertexArrayObject> vao;
		int material = 0;
	};

	/**
	*	メッシュ
	*/
	struct Mesh {
		std::string name;	//メッシュ名
		std::vector<Primitive> primitives;
	};

	/**
	*	ファイル
	*/
	struct File {
		std::string name;	//ファイル名
		std::vector<Mesh> meshes;
		std::vector<Material> materials;
	};
	using FilePtr = std::shared_ptr<File>;

	/**
	*	メッシュ管理クラス
	*/
	class Buffer {
	public:

		Buffer() = default;
		~Buffer() = default;

		bool Init(GLsizeiptr vboSize, GLsizeiptr iboSize);
		GLintptr AddVertexData(const void* data, size_t size);
		GLintptr AddIndexData(const void* data, size_t size);
		Primitive CreatePrimitive(size_t count, GLenum type, size_t ioffset, size_t voffset)const;
		Material CreateMaterial(const glm::vec4& color, Texture::Image2DPtr texture) const;
		bool AddMesh(const char* name, const Primitive& primitive, const Material& material);
		bool SetAttribute(Primitive*, int, const json11::Json&, const json11::Json&, const std::vector<std::vector<char> >&);
		bool LoadMesh(const char* path);
		FilePtr GetFile(const char* name) const;
		void SetViewProjectionMatrix(const glm::mat4&) const;

		void AddCube(const char* name);

		//スケルタル・アニメーションに対応したメッシュの読み込みと取得
		bool LoadSkeletalmesh(const char* path);
		SkeletalMeshPtr GetSkeletalMesh(const char* meshName) const;

	private:

		BufferObject vbo;
		BufferObject ibo;
		GLintptr vboEnd = 0;
		GLintptr iboEnd = 0;
		std::unordered_map<std::string, FilePtr> files;
		Shader::ProgramPtr progStaticMesh;

		//スケルタル・アニメーションに対応したメッシュを保持するメンバ変数
		Shader::ProgramPtr progSkeletalMesh;
		struct MeshIndex {
			ExtendedFilePtr file;
			const Node* node = nullptr;
		};
		std::unordered_map<std::string, MeshIndex> meshes;
		std::unordered_map<std::string, ExtendedFilePtr> extendedFiles;
	};

	void Draw(const FilePtr&, const glm::mat4& matM);

}// namespace Mesh




#endif
