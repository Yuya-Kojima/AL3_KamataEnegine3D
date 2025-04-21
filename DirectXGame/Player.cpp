#include "Player.h"

using namespace KamataEngine;

void Player::Initialize(Model* model, uint32_t textureHandle, Camera* camera) {

	// nullチェック
	assert(model);

	// テクスチャハンドルの初期化
	textureHandle_ = textureHandle;

	// モデルの初期化
	model_ = model;

	// カメラの初期化
	camera_ = camera;

	// ワールド変換の初期化
	worldTransform_.Initialize();
}

void Player::Update() {

	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();
}

void Player::Draw() {

	// DirectXCommonインスタンスの生成
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// 3Dモデルの描画前処理
	Model::PreDraw(dxCommon->GetCommandList());

	// 3Dモデルの描画
	model_->Draw(worldTransform_, *camera_, textureHandle_);

	// 3Dモデルの描画後処理
	Model::PostDraw();
}
