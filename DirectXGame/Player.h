#pragma once
#include "WorldMatrixTransform.h"
#include "KamataEngine.h"

class Player {

public:
	void Initialize(KamataEngine::Model* model);

	void Update();

	void Draw();

	~Player();

private:
	// 3Dモデル
	KamataEngine::Model* model_;

	// カメラ
	KamataEngine::Camera camera_;

	// ワールド変換データ
	KamataEngine::WorldTransform *worldTransform_;
};
