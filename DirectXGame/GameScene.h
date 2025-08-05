#pragma once
#include "CameraController.h"
#include "DeathParticles.h"
#include "Enemy.h"
#include "Fade.h"
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

	/// <summary>
	/// すべての当たり判定を行う
	/// </summary>
	void CheckAllCollisions();

	void ChangePhase();

	bool IsFinished() const { return finished_; }

private:
	// ゲームのフェーズ（型）
	enum class Phase {
		kFadeIn, // フェードイン
		kPlay,   // ゲームプレイ中
		kDeath,  // 死亡演出
		kFadeOut // フェードアウト
	};

	// ゲームの現在フェーズ（変数）
	Phase phase_ = Phase::kFadeIn;

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

	// Enemy
	std::list<Enemy*> enemies_;
	KamataEngine::Model* modelEnemy_ = nullptr;

	KamataEngine::Model* modelDeathParticles = nullptr;
	DeathParticles* deathParticles_ = nullptr;

	bool finished_ = false;

	Fade* fade_ = nullptr;
	const float kFadeDuration = 1.0f;
};
