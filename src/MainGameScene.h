/**
*	@file MainGameScene.h
*/
#ifndef MAINGAMESCENE_H_INCLUDE
#define MAINGAMESCENE_H_INCLUDE

#include "GLFWEW.h"
#include "Scene.h"
#include "Sprite.h"
#include "Font.h"
#include "Mesh.h"
#include "Terrain.h"
#include "Actor.h"
#include "PlayerActor.h"
#include "Audio.h"
#include "Light.h"
#include "FramebufferObject.h"
#include <vector>
#include <random>

/**
*	メインゲーム画面
*/
class MainGameScene : public Scene {
public:

	MainGameScene() : Scene("MainGameScene") {}
	virtual ~MainGameScene() = default;

	virtual bool Initialize() override;
	virtual void ProcessInput() override;
	virtual void Update(float) override;
	virtual void Render() override;
	virtual void Finalize() override;

	bool HandleJizoEffects(int id, const glm::vec3& pos);

	virtual void Play() override;
	virtual void Stop() override;

	void SpawnKooni(int n);
	void SpawnTree(int n);
	void CreateStoneWall(glm::vec3 s);

private:

	int jizoId = -1;					///< 現在戦闘中のお地蔵様のID
	bool achivements[4] = { false,false,false,false };	///< 敵討伐状態

	bool frag = false;
	SpriteRenderer spriteRenderer;		///< スプライト描画管理クラス
	FontRenderer fontRenderer;			///< フォント描画管理クラス
	Mesh::Buffer meshBuffer;			///< メッシュバッファ管理クラス
	std::vector<Sprite> sprites;		///< スプライト群
	Terrain::HeightMap heightMap;		///< 地形ハイトデータ
	PlayerActorPtr player;				///< プレイヤーデータ
	ActorList enemies;					///< 敵リストデータ
	ActorList trees;					///< 木リストデータ
	ActorList objects;					///< オブジェクトリストデータ
	
	LightBuffer lightBuffer;
	ActorList lights;

	FramebufferObjectPtr fboMain;			///< メインのフレームバッファオブジェクト

	Audio::SoundPtr bgm;				///< BGMデータ

	std::mt19937 randGen;

	struct Camera {
		glm::vec3 target = glm::vec3(100, 0, 100);
		glm::vec3 position = glm::vec3(100, 50, 150);
		glm::vec3 up = glm::vec3(0, 1, 0);
		glm::vec3 velocity = glm::vec3(0);

		//画面パラメータ
		float width = 1280;	///< 画面の幅(ピクセル数)
		float height = 720;	///< 画面の高さ(ピクセル数)
		float near = 1;		///< 最小z値(メートル)
		float far = 500;	///< 最大z値(メートル)

		//カメラパラメータ
		float fNumber = 1.4f;				///< エフ・ナンバー = カメラのF値
		float fov = glm::radians(60.0f);	///< フィールド・オブ・ビュー = カメラの視野角(ラジアン)
		float sensorSize = 36.0f;			///< センサー・サイズ=カメラのセンサーの横幅(ミリ)

		//Update関数で計算するパラメータ
		float focallength = 50.0f;		///< フォーカル・レングス = 焦点距離(ミリ)
		float aperture = 20.0f;			///< アパーチャ = 開口(ミリ)
		float focalPlane = 10000.0f;	///< フォーカル・プレーン = ピントの合う距離

		void Update(const glm::mat4& matView);
	};
	Camera camera;
};

#endif //MAINGAMESCENE_H_INCLUDE