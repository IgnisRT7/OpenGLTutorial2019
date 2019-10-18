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
	*	�����}�b�v
	*
	*	1, LoadFromFile()�ŉ摜�t�@�C�����獂������ǂݍ���
	*	2, CreateMesh()�œǂݍ��񂾍�����񂩂�n�`���b�V�����쐬����
	*	3, ����n�_�̍����𒲂ׂ�ɂ�Height()���g��
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
		glm::ivec2 size = glm::ivec2(0);			///< ���ƂɂȂ����摜�ӂ�����߂�
		std::vector<float> heights;					///< �n�C�g�}�b�v�̑傫��
		Texture::BufferPtr lightIndex[2];

		glm::vec3 CalcNormal(int x, int z) const;	///< �����f�[�^

		Shader::ProgramPtr progTerrain = 0;
	};


} // namespace Terrain

#endif
