#pragma once
#include "KamataEngine.h"
#include "Player.h"
#include "Skydome.h"
#include <vector>

// ゲームシーン
class GameScene {
public:
	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

private:
	// モデルデータ
	KamataEngine::Model* model_ = nullptr;

	// ブロック用ワールドトランスフォーム
	std::vector<std::vector<KamataEngine::WorldTransform*>> worldTransformBlocks_;

	// カメラ
	KamataEngine::Camera camera_;

	// デバックカメラ有効
	bool isDebugCameraActive_ = false;

	// デバックカメラ
	KamataEngine::DebugCamera* debugCamera_ = nullptr;

	// 天球
	Skydome* skydome_ = nullptr;

	// プレイヤー
	Player* player_ = nullptr;

	// 3Dモデル
	KamataEngine::Model* modelSkydome_ = nullptr;
};
