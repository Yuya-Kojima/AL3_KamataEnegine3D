#pragma once
#include "KamataEngine.h"
#include "WorldMatrixTransform.h"

class Player {

public:
	void Initialize(KamataEngine::Model* model);

	void Update();

	void Draw(KamataEngine::Camera& camera);

	~Player();

private:
	// 3Dモデル
	KamataEngine::Model* model_;

	// カメラ
	KamataEngine::Camera camera_;

	// ワールド変換データ
	KamataEngine::WorldTransform* worldTransform_;
};
