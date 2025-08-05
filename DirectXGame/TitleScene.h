#pragma once
#include "Fade.h"
#include "KamataEngine.h"
#include "WorldMatrixTransform.h"

using namespace KamataEngine;

class TitleScene {

public:
	void Initialize();
	void Update();
	void Draw();

	bool IsFinished() const { return finished_; }

	~TitleScene();

private:
	enum class Phase {
		kFadeIn, // フェードイン
		kMain,   // メイン部分
		kFadeOut // フェードアウト
	};

	// 現在のフェーズ
	Phase phase_ = Phase::kFadeIn;

	Model* model_ = nullptr;
	WorldTransform worldTransform_;
	Camera camera_;

	float time_ = 0.0f;

	bool finished_ = false;

	Fade* fade_ = nullptr;

	const float kFadeDuration = 1.0f;
};
