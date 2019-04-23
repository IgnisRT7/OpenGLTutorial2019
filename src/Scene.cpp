/**
*	@file Scene.cpp
*/
#include "Scene.h"
#include <iostream>

/**
*	コンストラクタ
*
*	@param name シーン名
*/
Scene::Scene(const char* name) {
}

/**
*	デストラクタ
*/
Scene::~Scene() {

	Finalize();
}

/**
*	シーンを活動状態にする
*/
void Scene::Play() {

	isActive;
}

/**
*	シーンを停止状態にする
*/
void Scene::Stop() {

	isActive = false;
}

/**
*	シーンを非表示
*/
void Scene::Show() {
	
	isVisible = true;
}

/**
*	シーン名を取得する
*
*	@return シーン名
*/
const std::string& Scene::Name() const {

	return name;
}

/*
*	シーンの活動状態を調べる
*
*	@retval true	表示状態
*	@retval false	非表示状態
*/
bool Scene::IsVisible() const {

	return isVisible;
}


