/**
 * @file OffscreenBuffer.cpp
*/

#include "OffscreenBuffer.h"

/**
 * オフスクリーンバッファを作成する
 * 
 * @param w	オフスクリーンバッファの幅(ピクセル単位)
 * @param h	オフスクリーンバッファの高さ(ピクセル単位)
 * 
 * @return 作成したオフスクリーンバッファのポインタ
*/

OffscreenBufferPtr OffscreenBuffer::Create(int w, int h){

	struct Impl : OffscreenBuffer{};
	OffscreenBufferPtr offscreen = std::make_shared<Impl>();

	if(!offscreen){
		return offscreen;
	}

	// テクスチャを作成する
	offscreen->tex = Texture::Create(w, h, GL_RGBA8, GL_RGBA, nullptr);

	// 深度バッファを作成する
	glGenRenderbuffers(1, &offscreen->depthbuffer);
	// 作成したバッファをOpenGLの処理対象として設定する関数
	glBindRenderbuffer(GL_RENDERBUFFER, offscreen->depthbuffer);
	// 深度バッファのフォーマットと大きさを設定
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, w, h);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// フレームバッファを作成する
	glGenFramebuffers(1, &offscreen->framebuffer);
	// 作成したバッファをOpenGLの処理対象として設定する関数
	glBindFramebuffer(GL_FRAMEBUFFER, offscreen->framebuffer);
	// 作成したカラーバッファあるいは深度バッファをフレームバッファに割り当てる関数
	glFramebufferRenderbuffer(
		GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER,offscreen->depthbuffer);
	// glGenrenderbuffers関数で作成したバッファの代わりに、2Dテクスチャをフレームバッファに割り当て 
	glFramebufferTexture2D(
		GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, offscreen->tex->Id(), 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	return offscreen;
}

/**
 * デストラクタ
*/
OffscreenBuffer::~OffscreenBuffer(){
	if(framebuffer){
		glDeleteFramebuffers(1, &framebuffer);
	}
	if(depthbuffer){
		glDeleteRenderbuffers(1, &depthbuffer);
	}
}