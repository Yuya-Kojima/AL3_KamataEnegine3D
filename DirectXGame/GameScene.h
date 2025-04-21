#pragma once
#include "KamataEngine.h"
#include "Player.h"

// ゲームシーン
class GameScene {
public:
	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw();

	// 終了
	void Finalize();

private:
	// テクスチャハンドル
	uint32_t textureHandle_ = 0;

	// 3Dモデルデータ
	KamataEngine::Model* model_ = nullptr;

	// カメラ
	KamataEngine::Camera *camera_=nullptr;

	// 自キャラ
	Player* player_ = nullptr;
};
