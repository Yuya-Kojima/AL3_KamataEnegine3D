#include "Player.h"

using namespace KamataEngine;

void Player::Initialize(Model* model, Vector3& position) {

	// 3Dモデルの初期化
	model_ = model;

	// ワールド変換データの初期化
	// worldTransform_ = new WorldTransform();
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	// worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;

	// カメラの初期化
	camera_.Initialize();
}

void Player::Update() {

	if (Input::GetInstance()->PushKey(DIK_RIGHT) || Input::GetInstance()->PushKey(DIK_LEFT)) {

		// 左右加速
		Vector3 acceleration = {};

		if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
			acceleration.x += kAcceleration_;
		} else if (Input::GetInstance()->PushKey(DIK_LEFT)) {
			acceleration.x -= kAcceleration_;
		}

		velocity_ = Add(velocity_, acceleration);
	} else {

		// 非入力時は移動減衰
		velocity_.x *= (1.0f - kAttenuation_);
	}

	// 移動
	worldTransform_.translation_ = Add(worldTransform_.translation_, velocity_);

	// 行列更新
	WorldTransformUpdate(worldTransform_);
}

void Player::Draw(Camera& camera) {

	// DirectXCommonインスタンスの生成
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	Model::PreDraw(dxCommon->GetCommandList());

	// 3Dモデル描画
	model_->Draw(worldTransform_, camera);

	Model::PostDraw();
}

Player::~Player() {

	// 3Dモデルの解放
	delete model_;

	//	delete worldTransform_;
}