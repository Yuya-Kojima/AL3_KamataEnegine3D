#include "Player.h"

using namespace KamataEngine;

void Player::Initialize(Model* model) {

	// 3Dモデルの初期化
	model_ = model;

	// ワールド変換データの初期化
	worldTransform_ = new WorldTransform();
	worldTransform_->Initialize();

	// カメラの初期化
	camera_.Initialize();
}

void Player::Update() {}

void Player::Draw() {

	// DirectXCommonインスタンスの生成
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	Model::PreDraw(dxCommon->GetCommandList());

	// 3Dモデル描画
	model_->Draw(*worldTransform_, camera_);

	Model::PostDraw();
}

Player::~Player() {

	// 3Dモデルの解放
	delete model_;

	delete worldTransform_;
}