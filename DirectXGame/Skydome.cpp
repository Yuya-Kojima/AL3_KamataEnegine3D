#include "Skydome.h"

using namespace KamataEngine;

void Skydome::Initialize(Model* model) {

	// カメラの初期化
	camera_.Initialize();

	// モデルの生成
	// model_ = Model::Create();
	// model_ = Model::CreateFromOBJ("skydome",true);
	model_ = model;

	// ワールドトランスフォームの初期化
	//worldTransform_ = new WorldTransform();
	worldTransform_.Initialize();
}

void Skydome::Update() {}

void Skydome::Draw() {

	// DirectXCommonインスタンスの生成
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	Model::PreDraw(dxCommon->GetCommandList());

	// 3Dモデル描画
	model_->Draw(worldTransform_, camera_);

	Model::PostDraw();
}

Skydome::~Skydome() {

	delete model_;

	//delete worldTransform_;
}
