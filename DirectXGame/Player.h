#pragma once
#include "KamataEngine.h"
#include "WorldMatrixTransform.h"
#include <numbers>

class Player {

public:
	void Initialize(KamataEngine::Model* model, KamataEngine::Vector3& position);

	void Update();

	void Draw(KamataEngine::Camera& camera);

	~Player();

private:
	enum class LRDirection {
		kRight,
		kLeft,
	};

	// 3Dモデル
	KamataEngine::Model* model_ = nullptr;

	// カメラ
	KamataEngine::Camera camera_;

	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;

	// 速度
	KamataEngine::Vector3 velocity_ = {};

	// プレイヤーの向き
	LRDirection lrDirection_ = LRDirection::kRight;

	// 旋回開始時の角度
	float turnFirstRotationY_ = 0.0f;

	// 旋回タイマー
	float turnTimer_ = 0.0f;

	// 接地状態フラグ
	bool onGround_ = true;

	// 旋回時間
	static inline const float kTimeTurn = 0.3f;

	static inline const float kAcceleration_ = 0.01f;

	static inline const float kAttenuation_ = 0.1f;

	static inline const float kLimitRunSpeed_ = 0.5f;

	// 重力加速度(下方向)
	static inline const float kGravityAcceleration = 0.5f;

	// 最大落下速度
	static inline const float kLimitFallSpeed = 3.0f;

	// ジャンプ初速(上方向)
	static inline const float kJumpAcceleration = 0.1f;
};
