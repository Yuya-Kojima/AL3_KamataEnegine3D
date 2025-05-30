#include "GameScene.h"

using namespace KamataEngine;

void GameScene::Initialize() {

	// 3Dモデルデータの生成
	model_ = Model::Create();

	// デバックカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);

	// カメラの生成と初期化
	camera_ = new Camera();
	camera_->Initialize();

	// 要素数
	const uint32_t kNumBlockVirtical = 10;
	const uint32_t kNumBlockHorizontal = 20;

	// ブロック一個分の横幅
	const float kBlockWidth = 2.0f;
	const float kBlockHeight = 2.0f;

	// 要素数を変更する
	// 列数を設定(縦方向のブロック数)
	worldTransformBlocks_.resize(kNumBlockVirtical);

	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {

		// 一列の要素数を指定(横方向のブロック数)
		worldTransformBlocks_[i].resize(kNumBlockHorizontal);
	}

	// キューブの生成
	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
		for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {
			if (i == j) {
				worldTransformBlocks_[i][j] = new WorldTransform();
				worldTransformBlocks_[i][j]->Initialize();
				worldTransformBlocks_[i][j]->translation_.x = kBlockWidth * j;
				worldTransformBlocks_[i][j]->translation_.y = kBlockHeight * i;
			}
		}
	}
}

void GameScene::Update() {

	// ブロックの更新
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {

			if (!worldTransformBlock) {
				continue;
			}

			// アフィン行列作成
			Matrix4x4 affineMatrix;

			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					if (i == j) {
						affineMatrix.m[j][i] = 1.0f;
					} else {
						affineMatrix.m[j][i] = 0.0f;
					}
				}
			}

			affineMatrix.m[3][0] = worldTransformBlock->translation_.x;
			affineMatrix.m[3][1] = worldTransformBlock->translation_.y;

			worldTransformBlock->matWorld_ = affineMatrix; // 資料14pまで

			// 定数バッファに転送する
			worldTransformBlock->TransferMatrix();
		}
	}

	// デバックカメラの更新
	debugCamera_->Update();

#ifdef _DEBUG

	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		isDebugCameraActive_ = true; // 27pまで
	}

#endif // DEBUG
}

void GameScene::Draw() {

	// DirectXCommonインスタンスの生成
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	Model::PreDraw(dxCommon->GetCommandList());

	// ブロックの描画
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {

			if (!worldTransformBlock) {
				continue;
			}

			model_->Draw(*worldTransformBlock, *camera_);
		}
	}

	Model::PostDraw();
}

GameScene::~GameScene() {

	// モデルデータの解放
	delete model_;

	// カメラの解放
	delete camera_;

	// デバックカメラの解放
	delete debugCamera_;

	// ブロック用ワールドトランスフォームの解放
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}

	worldTransformBlocks_.clear();
}
