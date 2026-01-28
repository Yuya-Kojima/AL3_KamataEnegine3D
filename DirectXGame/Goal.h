#pragma once
#include "AABB.h"
#include "WorldMatrixTransform.h"
#include <KamataEngine.h>

using namespace KamataEngine;

class Goal {

public:
	void Initialize(const Vector3& pos);

	AABB GetAABB() const;

	Vector3 GetWorldPosition() const;

	void Update();

	void Draw(Camera* camera, Model* model);

	void SetScale(const Vector3& scale) { worldTransform_.scale_ = scale; }

private:
	Vector3 pos_;
	Vector3 size_;
	WorldTransform worldTransform_;

	const float kWidth_ = 1.0f;
	const float kHeight_ = 2.0f;
};
