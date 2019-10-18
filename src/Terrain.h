/**
*	@file Terrain.h
*/
#ifndef TERRAIN_H_INCLUDE
#define TERRAIN_H_INCLUDE

#include "Mesh.h"
#include "Texture.h"
#include "Light.h"
#include <glm/glm.hpp>
#include <string>
#include <vector>

namespace Terrain {

	/**
	*	高さマップ
	*
	*	1, LoadFromFile()で画像ファイルから高さ情報を読み込む
	*	2, CreateMesh()で読み込んだ高さ情報から地形メッシュを作成する
	*	3, ある地点の高さを調べるにはHeight()を使う
	*/
	class HeightMap {
	public:

		HeightMap();
		~HeightMap() = default;

		bool LoadFromFile(const char* path, float scale, float baseLevel);
		bool LoadFromTextureImage(const Texture::ImageData&, float, float);
		float Height(const glm::vec3& pos) const;
		const glm::ivec2 Size() const;
		bool CreateMesh(Mesh::Buffer& meshBuffer, const char* meshName, const char* texName = nullptr) const;
		void UpdateLightIndex(const ActorList& lights);

	private:

		std::string name;
		glm::ivec2 size = glm::ivec2(0);			///< もとになった画像ふぁぃるめい
		std::vector<float> heights;					///< ハイトマップの大きさ
		Texture::BufferPtr lightIndex[2];

		glm::vec3 CalcNormal(int x, int z) const;	///< 高さデータ

		Shader::ProgramPtr progTerrain = 0;
	};


} // namespace Terrain

#endif
