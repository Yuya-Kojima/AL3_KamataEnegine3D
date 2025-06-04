#include "Player.h"

using namespace KamataEngine;

void Player::Initialize(Model* model) {

	// 3Dモデルの初期化
	model_ = model;

	// ワールド変換データの初期化
	worldTransform_ = new WorldTransform();
	worldTransform_->Initialize();
	worldTransform_->translation_.x = 2.0f;
	worldTransform_->translation_.y = 2.0f;

	// カメラの初期化
	camera_.Initialize();
}

void Player::Update() {

	WorldTransformUpdate(*worldTransform_);

	if (Input::GetInstance()->TriggerKey(DIK_W)) {
		worldTransform_->translation_.y += 2.0f;
	} else if (Input::GetInstance()->TriggerKey(DIK_S)) {
		worldTransform_->translation_.y -= 2.0f;
	}

	if (Input::GetInstance()->TriggerKey(DIK_A)) {
		worldTransform_->translation_.x -= 2.0f;
	} else if (Input::GetInstance()->TriggerKey(DIK_D)) {
		worldTransform_->translation_.x += 2.0f;
	}
}

void Player::Draw(Camera& camera) {

	// DirectXCommonインスタンスの生成
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	Model::PreDraw(dxCommon->GetCommandList());

	// 3Dモデル描画
	model_->Draw(*worldTransform_, camera);

	Model::PostDraw();
}

Player::~Player() {

	// 3Dモデルの解放
	delete model_;

	delete worldTransform_;
}