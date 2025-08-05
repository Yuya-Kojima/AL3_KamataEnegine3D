#pragma once
#include "KamataEngine.h"
#include "WorldMatrixTransform.h"

using namespace KamataEngine;

class TitleScene {

public:
	void Initialize();
	void Update();
	void Draw();

	bool IsFinished() const { return finished_; }

private:
	Model* model_ = nullptr;
	WorldTransform worldTransform_;
	Camera camera_;

	float time_ = 0.0f;

	bool finished_ = false;
};
