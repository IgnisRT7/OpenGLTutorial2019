/**
*	@file Terrain.h
*/
#ifndef TERRAIN_H_INCLUDE
#define TERRAIN_H_INCLUDE

#include "Mesh.h"
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

		HeightMap() = default;
		~HeightMap() = default;

		bool LoadFromFile(const char* path, float scale, float baseLevel);
		float Height(const glm::vec3& pos) const;
		const glm::ivec2 Size() const;
		bool CreateMesh(Mesh::Buffer& meshBuffer, const char* meshName, const char* texName = nullptr) const;

	private:

		std::string name;
		glm::ivec2 size = glm::ivec2(0);			///< ���ƂɂȂ����摜�ӂ�����߂�
		std::vector<float> heights;					///< �n�C�g�}�b�v�̑傫��

		glm::vec3 CalcNormal(int x, int z) const;	///< �����f�[�^
	};


} // namespace Terrain

#endif
