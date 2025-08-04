#include "Enemy.h"

void Enemy::Initialize(Model* model, Camera* camera) {
	model_ = model;
	camera_ = camera;
	worldTransform_.Initialize();

	// 初期座標
	worldTransform_.translation_ = {14.0f, 1.0f, 0.0f};

	// 自キャラと逆向き（例：左を向く）
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;

	velocity_ = {-kWalkSpeed, 0, 0};

	walkTimer_ = 0.0f;
}

void Enemy::Update() { // 行列更新
	worldTransform_.translation_ += velocity_;

	// タイマー加算（1フレームに1/60秒ずつ）
	walkTimer_ += 1.0f / 60.0f;

	// 回転アニメーション
	float param = std::sin((2.0f * std::numbers::pi_v<float>)*walkTimer_ / kWalkMotionTime);

	float degree = kWalkMotionAngleStart + (kWalkMotionAngleEnd - kWalkMotionAngleStart) * (param + 1.0f) / 2.0f;
	worldTransform_.rotation_.x = degree * (3.14159265f / 180.0f); // 度をラジアンに変換

	WorldTransformUpdate(worldTransform_);
}

void Enemy::Draw() {

	// DirectXCommonインスタンスの生成
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	Model::PreDraw(dxCommon->GetCommandList());

	model_->Draw(worldTransform_, *camera_);

	Model::PostDraw();
}
