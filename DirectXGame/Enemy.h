#pragma once
#include "AABB.h"
#include "KamataEngine.h"
#include "WorldMatrixTransform.h"
#include <numbers>

using namespace KamataEngine;

class Player;

/// <summary>
/// 敵
/// </summary>
class Enemy {

public:
	void Initialize(Model* model, Camera* camera, Vector3& position);

	void Update();

	void Draw();

	// 衝突応答
	void OnCollision(const Player* player);

	/// <summary>
	/// AABBを取得
	/// </summary>
	/// <returns></returns>
	AABB GetAABB();

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

	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;
};
