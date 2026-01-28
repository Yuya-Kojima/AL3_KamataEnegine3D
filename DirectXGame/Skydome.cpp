#include "Skydome.h"

using namespace KamataEngine;

void Skydome::Initialize(Model* model) {

	// カメラの初期化
	camera_.Initialize();

	// モデルの生成
	model_ = model;

	// ワールドトランスフォームの初期化
	worldTransform_ = new WorldTransform();
	worldTransform_->Initialize();
}

void Skydome::Update() { WorldTransformUpdate(*worldTransform_); }

void Skydome::Draw(Camera& camera) {

	// DirectXCommonインスタンスの生成
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	Model::PreDraw(dxCommon->GetCommandList());

	// 3Dモデル描画
	model_->Draw(*worldTransform_, camera);

	Model::PostDraw();
}

Skydome::~Skydome() {

	delete model_;

	delete worldTransform_;
}
