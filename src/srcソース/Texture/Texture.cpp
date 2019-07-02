/**
 * @file Textute.cpp
*/

#include "Texture.h"
#include <iostream>
#include <vector>
#include <stdint.h>
#include <stdio.h>
#include <sys/stat.h>

/**
* バイト列から数値を復元する
*
* @param p		バイト列へのポインタ
* @param offset	数値のオフセット
* @param size	数値のバイト数(1～4)
*
* @return		復元した数値
*/
uint32_t Get(const uint8_t* p, size_t offset, size_t size) {
	uint32_t n = 0;
	p += offset;

	// 8ビットずつ取り出し元のビット位置に戻す
	for (size_t i = 0; i < size; ++i) {
		n += p[i] << (i * 8);
	}

	return n;
}


/**
 * 	デストラクタ
*/
Texture::~Texture(){
	if(texId){
		// テクスチャを破棄
		glDeleteTextures(1, &texId);
	}
}


/**
 * 2D テクスチャを作成する
 * 
 * @param width		テクスチャの幅(ピクセル数)
 * @param height	テクスチャの高さ(ピクセル数)
 * @param iformat	テクスチャのデータ形式
 * @param format	アクセスする要素
 * @param data		テクスチャデータのポインタ
 * 
 * @reuturn 作成に成功した場合はテクスチャのポインタを返す
 * 			失敗した場合はnullptrを返す
*/
TexturePtr Texture::Create(
	int width, int height, GLenum iformat, GLenum format, const void* data){
		struct Impl : Texture {};
		TexturePtr p = std::make_shared<Impl>();

		p->width = width;
		p->height = height;

		// テクスチャオブジェクトの作成
		glGenTextures(1, &p->texId);
		// 作成したオブジェクトを特定のテクスチャ処理スロットに割り当て
		glBindTexture(GL_TEXTURE_2D, p->texId);
		
		// テクスチャの属性と画像データを転送
		glTexImage2D(
			GL_TEXTURE_2D, 0, iformat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		
		const GLenum result = glGetError();
		if(result != GL_NO_ERROR){
			std::cerr << "ERROR テクスチャ作成に失敗 : 0x " << std::hex << result << std::endl;
			return {};
		}

		// パラメータの設定 glTexParamter* *が表現型
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glBindTexture(GL_TEXTURE_2D, 0);
		
		return p;


}



/**
* ファイルから2Dテクスチャを読み込む
*
* @param filename	ファイル名
*
* @return	作成に成功した場合はテクスチャポインタを返す
*			失敗した場合はnullptrを返す
*/
TexturePtr Texture::LoadFromFile(const char* filename) {
	
	// ファイルサイズを取得する
	struct stat st;
	if ( stat( filename, &st ) ) {
		return {};
	}

	// ファイルサイズがBMPファイルの情報を保持できるサイズ未満なら空のオブジェクトを返す
	
	// ビットマップファイルヘッダのバイト数
	const size_t bmpFileHeaderSize = 14;

	// ビットマップ情報ヘッダのバイト数
	const size_t windowsV1HeaderSize = 40;
	if (st.st_size < bmpFileHeaderSize + windowsV1HeaderSize) {
		return {};
	}

	// ファイルを開く
	FILE * fp = fopen(filename, "rb");
	if (!fp) {
		return {};
	}

	// ファイルを読み込む
	std::vector<uint8_t> buf;
	buf.resize(st.st_size);
	const size_t readSize = fread(buf.data(), 1, st.st_size, fp);
	fclose(fp);
	if (readSize != st.st_size) {
		return{};
	}

	//BMPファイルでなければ空のオブジェクトを返す
	const uint8_t* pHeader = buf.data();
	if (pHeader[0] != 'B' || pHeader[1] != 'M') {
		return {};
	}

	// BMPファイルの情報を取得する
	const size_t offsetBytes = Get(pHeader, 10, 4);
	const uint32_t infoSize = Get(pHeader, 14, 4);
	const uint32_t width = Get(pHeader, 18, 4);
	const uint32_t height = Get(pHeader, 22, 4);
	const uint32_t bitCount = Get(pHeader, 28, 2);
	const uint32_t compression = Get(pHeader, 30, 4);
	const size_t pixelBytes = bitCount / 8;
	if (infoSize != windowsV1HeaderSize || bitCount != 24 || compression) {
		return {};
	}

	// 画像データが正しく格納されているか検査
	if (infoSize != windowsV1HeaderSize || bitCount != 24 || compression) {
		return {};
	}
	// 実際の横のバイト数
	const size_t stride = ((width * pixelBytes + 3) / 4) * 4;
	const size_t imageSize = stride * height;
	if (buf.size() < offsetBytes + imageSize) {
		return {};
	}

	return Create(width, height, GL_RGB8, GL_BGR, buf.data() + offsetBytes);
}
