/**
*	@file TerrainGenerator.cpp
*/
#include "TerrainGenerator.h"
#include <iostream>

namespace TerrainGenerator {

	enum GenerateType {
		STD_RAND,
		SIN_COS,
		PARLIN,
	};

	 bool Controller::Generate(){

		 std::vector<uint8_t> buf;
		 buf.reserve(size.x*size.y);
		 GenerateType type = GenerateType::PARLIN;

		 double recip = 1.0f / 256.0f;
		 int center = size.y / 2;

		 int n = -1;
		 int test = 1 - 3 * n * n + 2 * abs(n) * n * n;

		 for (int x = 0; x < size.x; x++) {

			 //1 - 3x² + 2|x|³
			 float wavelet = 1 - 3 * x * x + 2 * abs(x) * x * x;
			 //float ax1 = 


			 auto lineA = (rand() % 10 - 5) * x;
			 auto lineB = (rand() % 10 - 5) * x;

			 for (int z = 0; z < size.y; z++) {


				 //
				 auto waveletA = wavelet * lineA;
				 auto waveletB = wavelet * lineB;

				 switch (type) {
				 case GenerateType::STD_RAND:
					 buf.push_back(static_cast<uint8_t>(std::rand() % 256));
					 break;

				 case GenerateType::SIN_COS: 
					 buf.push_back(static_cast<uint8_t>(glm::sin(glm::radians(3.0f) * z ) * 128));
					 break;

				 case GenerateType::PARLIN:			 
					 buf.push_back(static_cast<uint8_t>(waveletA + (waveletB - waveletA) * (x / 128.0f)));
					 break;
				 
				 }
			 }
		 }
			 
			 /*= {

			 100,	100,0,	0,	100,100,0,0,100,	100,0,	0,	100,100,0,0,
			 100,	100,0,	0,	100,100,0,0,100,	100,0,	0,	100,100,0,0,
			 0,		0,	100,100,0,	0,100,100,0,		0,100,100,	0,	0,100,100,
			 0,		0,	100,100,0,	0,100,100,0,		0,100,100,	0,	0,100,100,
			 100,100,	0,	0,	100,100,0,0,100,	100,0,	0,	100,100,0,0,
			 100,100,	0,	0,	100,100,0,0,100,	100,0,	0,	100,100,0,0,
			 0,		0,100,100,	0,	0,100,100,0,		0,100,100,	0,	0,100,100,
			 0,		0,100,100,	0,	0,100,100,0,		0,100,100,	0,	0,100,100,
			 100,	100,0,	0,	100,100,0,0,100,	100,0,	0,	100,100,0,0,
			 100,	100,0,	0,	100,100,0,0,100,	100,0,	0,	100,100,0,0,
			 0,		0,	100,100,0,	0,100,100,0,		0,100,100,	0,	0,100,100,
			 0,		0,	100,100,0,	0,100,100,0,		0,100,100,	0,	0,100,100,
			 100,100,	0,	0,	100,100,0,0,100,	100,0,	0,	100,100,0,0,
			 100,100,	0,	0,	100,100,0,0,100,	100,0,	0,	100,100,0,0,
			 0,		0,100,100,	0,	0,100,100,0,		0,100,100,	0,	0,100,100,
			 0,		0,100,100,	0,	0,100,100,0,		0,100,100,	0,	0,100,100,
		 };*/

		 imageData.format = 6403;
		 imageData.type = 5121;
		 imageData.height = size.y;
		 imageData.width = size.x;
		 imageData.data.swap(buf);

		 return true;
	 }

}