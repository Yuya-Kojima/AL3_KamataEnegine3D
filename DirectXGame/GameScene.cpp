#include "GameScene.h"

using namespace KamataEngine;

void GameScene::Initialize() {

	// ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("uvChecker.png");

	// 3Dモデルデータの生成
	model_ = Model::Create();

	// カメラの初期化
	camera_ = new Camera();
	camera_->Initialize();

	// 自キャラの生成
	player_ = new Player();

	// 自キャラの初期化
	player_->Initialize(model_, textureHandle_, camera_);
}

void GameScene::Update() {

	// 自キャラの更新処理
	player_->Update();
}

void GameScene::Draw() {

	// DirectXCommonインスタンスの生成
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// 3Dモデル描画前処理
	Model::PreDraw(dxCommon->GetCommandList());

	// 自キャラの描画処理
	player_->Draw();

	// 3Dモデル描画後処理
	Model::PostDraw();
}

void GameScene::Finalize() {

	delete model_;
	delete player_;
}
