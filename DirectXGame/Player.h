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
	// 3Dモデル
	KamataEngine::Model* model_ = nullptr;

	// カメラ
	KamataEngine::Camera camera_;

	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;

	// 速度
	KamataEngine::Vector3 velocity_ = {};

	static inline const float kAcceleration_ = 0.01f;

	static inline const float kAttenuation_ = 0.1f;
};
