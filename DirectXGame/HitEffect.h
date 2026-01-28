#pragma once
#include "WorldMatrixTransform.h"
#include <KamataEngine.h>

using namespace KamataEngine;

class HitEffect {

public:
	void Initialize(const Vector3& origin);

	void Update();

	void Draw();

	static void SetModel(Model* model) { model_ = model; }

	static void SetCamera(Camera* camera) { camera_ = camera; }

	static HitEffect* Create(const Vector3& origin);

	bool isDead() const { return state_ == State::kDead; }

private:
	enum class State {
		kSpread,
		kFade,
		kDead,
	};

	State state_ = State::kSpread;

	float timer_;
	static constexpr float kSpreadTime = 0.15f; // 拡大フェーズの長さ
	static constexpr float kFadeTime = 0.20f;   // フェードフェーズの長さ
	float opacity_ = 1.0f;

	// モデル(借りてくる用)
	static Model* model_;

	// カメラ(借りてくる用)
	static Camera* camera_;

	// 円形エフェクト
	WorldTransform circleWorldTransform_;

	// 楕円エフェクト
	std::array<WorldTransform, 2> ellipseWorldTransforms_;
};
