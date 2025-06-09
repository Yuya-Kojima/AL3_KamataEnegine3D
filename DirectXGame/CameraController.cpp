#define NOMINMAX
#include "CameraController.h"
#include "Player.h"
#include <algorithm>

using namespace KamataEngine;

inline KamataEngine::Vector3 operator*(const KamataEngine::Vector3& v, float s) { return {v.x * s, v.y * s, v.z * s}; }

void CameraController::Initialize() {

	// カメラの初期化
	camera_.Initialize();
}

void CameraController::Update() {

	targetVelocity_ = target_->GetVelocity();

	// 追従対象のワールドトランスフォームを参照
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();

	// 追従対象とオフセットと追従対象の速度からカメラの目標座標を計算
	targetPosition_ = Add(Add(targetWorldTransform.translation_, targetOffset_), targetVelocity_ * kVelocityBias);

	// 座標補間
	targetCamera_->translation_ = Lerp(targetCamera_->translation_, targetPosition_, kInterpolationRate);

	// 画面外に出ないように
	targetCamera_->translation_.x = std::clamp(targetCamera_->translation_.x, target_->GetWorldTransform().translation_.x + margin_.left, target_->GetWorldTransform().translation_.x + margin_.right);
	targetCamera_->translation_.y = std::clamp(targetCamera_->translation_.y, target_->GetWorldTransform().translation_.y + margin_.bottom, target_->GetWorldTransform().translation_.y + margin_.top);

	// 移動範囲制限
	targetCamera_->translation_.x = std::clamp(targetCamera_->translation_.x, movableArea_.left, movableArea_.right);
	targetCamera_->translation_.y = std::clamp(targetCamera_->translation_.y, movableArea_.bottom, movableArea_.top);

	// 行列を更新
	targetCamera_->UpdateMatrix();
}

void CameraController::Reset() {

	// 追従対象のワールドトランスフォームを参照
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();

	// 追従対象とオフセットからカメラの座標を計算
	camera_.translation_ = Add(targetWorldTransform.translation_, targetOffset_);
}
