#pragma once

#define NOMINMAX
#include "CameraController.h"
#include "DeathParticles.h"
#include "Enemy.h"
#include "Fade.h"
#include "Goal.h"
#include "HitEffect.h"
#include "KamataEngine.h"
#include "MapChipField.h"
#include "Player.h"
#include "Skydome.h"
#include "WorldMatrixTransform.h"
#include <Windows.h>
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

	void CreateHitEffect(const Vector3& origin);

private:
	// ゲームのフェーズ（型）
	enum class Phase {
		kFadeIn,   // フェードイン
		kPlay,     // ゲームプレイ中
		kDeath,    // 死亡演出
		kFadeOut,  // フェードアウト
		kClear,    // クリア
		kGameOver, // ゲームオーバー
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
	KamataEngine::Model* modelSlimeOuter_ = nullptr;
	KamataEngine::Model* modelSlimeInner_ = nullptr;
	KamataEngine::Model* attackPlayer_ = nullptr;

	// マップチップフィールド
	MapChipField* mapChipField_;

	// カメラコントローラー
	CameraController* cameraController_ = nullptr;

	// Enemy
	uint32_t maxEnemyCount_;
	std::list<Enemy*> enemies_;
	KamataEngine::Model* modelEnemy_ = nullptr;

	KamataEngine::Model* modelDeathParticles = nullptr;
	DeathParticles* deathParticles_ = nullptr;

	bool finished_ = false;

	Fade* fade_ = nullptr;
	const float kFadeDuration = 1.0f;

	// ヒットエフェクト
	std::list<HitEffect*> hitEffects_;
	KamataEngine::Model* modelHitEffect_ = nullptr;

	// ゴール
	Goal goal_;
	KamataEngine::Model* goalModel_ = nullptr;
	KamataEngine::Vector3 goalPos_{};
	float clearTimer_ = 0.0f;
	const float clearMaxTime_ = 0.5f;

	KamataEngine::Model* clearTextModel_ = nullptr;
	KamataEngine::WorldTransform clearTextWT;

	// 操作方法
	KamataEngine::Sprite* operatorSprite_ = nullptr;
	uint32_t textureHandle_ = 0;

	// BGM
	uint32_t bgmHandle_ = 0;
};
