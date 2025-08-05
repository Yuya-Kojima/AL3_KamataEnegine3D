#pragma once

#define NOMINMAX

#include "KamataEngine.h"
#include <algorithm>

using namespace KamataEngine;

class Fade {
public:
	// フェードの状態
	enum class Status {
		None,   // フェードなし
		FadeIn, // フェードイン中
		FadeOut // フェードアウト中
	};

	void Initialize();

	void Update();

	void Draw();

	void Start(Status status, float duration);

	void Stop();

	// フェード終了判定
	bool IsFinished() const;

private:
	Status status_ = Status::None;

	Sprite* sprite_ = nullptr;
	uint32_t textureHandle = 0;

	// フェードの持続時間
	float duration_ = 0.0f;

	// 経過時間カウンター
	float counter_ = 0.0f;
};
