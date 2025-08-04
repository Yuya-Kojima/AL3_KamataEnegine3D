#pragma once
#include "KamataEngine.h"
#include "WorldMatrixTransform.h"
#include <numbers>

using namespace KamataEngine;

/// <summary>
/// 敵
/// </summary>
class Enemy {

public:
	void Initialize(Model* model, Camera* camera);

	void Update();

	void Draw();

private:
	// ワールド変換データ
	WorldTransform worldTransform_;

	// 3Dモデル
	Model* model_ = nullptr;

	// カメラ
	Camera* camera_ = nullptr;

	// 歩行の速さ
	static inline const float kWalkSpeed = 0.05f;

	// 速度
	Vector3 velocity_ = {};

	// アニメーション用角度（度数法）
	static inline const float kWalkMotionAngleStart = -10.0f; // 少し下を向く
	static inline const float kWalkMotionAngleEnd = 10.0f;    // 少し上を向く

	// アニメーションの周期（秒）
	static inline const float kWalkMotionTime = 1.0f;

	// 経過時間（アニメーション用）
	float walkTimer_ = 0.0f;
};
