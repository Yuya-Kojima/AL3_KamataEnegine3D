#pragma once
#include "CameraController.h"
#include "KamataEngine.h"
#include "MapChipField.h"
#include "Player.h"
#include "Skydome.h"
#include "WorldMatrixTransform.h"
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

	void GenerateBlocks();

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
	KamataEngine::Model* modelSkydome_ = nullptr;

	// プレイヤー
	Player* player_ = nullptr;
	KamataEngine::Model* modelPlayer_ = nullptr;

	// マップチップフィールド
	MapChipField* mapChipField_;

	// カメラコントローラー
	CameraController* cameraController_ = nullptr;
};
