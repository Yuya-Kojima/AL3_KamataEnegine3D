#include "Goal.h"
#include <numbers>

void Goal::Initialize(const Vector3& pos) {

	worldTransform_.Initialize();
	worldTransform_.translation_ = pos;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;

	WorldTransformUpdate(worldTransform_);

	size_ = {1.0f, 2.0f, 1.0f};
}

AABB Goal::GetAABB() const {

	Vector3 worldPos = GetWorldPosition();

	AABB aabb;

	aabb.min = {worldPos.x - kWidth_ / 2.0f, worldPos.y - kHeight_ / 2.0f, worldPos.z - kWidth_ / 2.0f};
	aabb.max = {worldPos.x + kWidth_ / 2.0f, worldPos.y + kHeight_ / 2.0f, worldPos.z + kWidth_ / 2.0f};

	return aabb;
}

Vector3 Goal::GetWorldPosition() const {
	Vector3 worldPos;

	// ワールド行列の平行移動成分を取り出す（ワールド座標）
	worldPos.x = worldTransform_.matWorld_.m[3][0]; // X
	worldPos.y = worldTransform_.matWorld_.m[3][1]; // Y
	worldPos.z = worldTransform_.matWorld_.m[3][2]; // Z

	return worldPos;
}

void Goal::Update() { WorldTransformUpdate(worldTransform_); }

void Goal::Draw(Camera* camera, Model* model) {

	// DirectXCommonインスタンスの生成
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	Model::PreDraw(dxCommon->GetCommandList());

	// 3Dモデル描画
	model->Draw(worldTransform_, *camera);

	Model::PostDraw();
}
