/**
*	@file TerrainGenerator
*/

#pragma once
#include "Texture.h"

namespace TerrainGenerator {

	class Controller{
	public:

		Controller() = default;
		Controller(const Controller&) = delete;
		const Controller& operator=(const Controller&) = delete;

		bool Generate();

		const Texture::ImageData& ImageData() const { return imageData; }

	private:

		glm::ivec2 size = {128,128};
		Texture::ImageData imageData;
	};


}