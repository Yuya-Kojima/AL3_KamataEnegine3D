#pragma once
#include "KamataEngine.h"
#include "WorldMatrixTransform.h"
#include <algorithm>
#include <array>
#include <numbers>

using namespace KamataEngine;

Vector3 Transform(const Vector3& vec, const Matrix4x4& mat);

class DeathParticles {
public:
	void Initialize(Model* model, Camera* camera, Vector3 position);
	void Update();
	void Draw();
	bool IsFinished() const { return isFinished_; }

private:
	Model* model_ = nullptr;
	Camera* camera_ = nullptr;

	static inline const uint32_t kNumParticles = 8;
	std::array<WorldTransform, kNumParticles> worldTransforms_;

	static inline const float kDuration = 1.0f;                                     // 寿命（秒）
	static inline const float kSpeed = 0.1f;                                        // 移動の速さ
	static inline const float kAngleUnit = 2.0f * std::numbers::pi_v<float> / 8.0f; // 分割角度

	// 終了フラグ
	bool isFinished_ = false;

	// 経過時間カウント
	float counter_ = 0.0f;

	// 色変更オブジェクト
	ObjectColor objectColor_;
	// 色の数値
	Vector4 color_;
};
