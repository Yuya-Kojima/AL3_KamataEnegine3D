#include "GameScene.h"

using namespace KamataEngine;

void GameScene::Initialize() {

	// 3Dモデルデータの生成
	model_ = Model::CreateFromOBJ("cube", true);

	// デバックカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);

	// カメラの初期化
	camera_.farZ = 510.0f;
	camera_.Initialize();

	// 天球の生成と初期化
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);
	skydome_ = new Skydome();
	skydome_->Initialize(modelSkydome_);

	// プレイヤーの初期化
	modelPlayer_ = Model::CreateFromOBJ("player", true);

	Vector3 playerPosition = mapChipField_->GetMatChipPositionByIndex(1, 18);

	// マップチップフィールド
	mapChipField_ = new MapChipField;
	mapChipField_->LoadMapChipCsv("Resources/AL3_mapchip.csv");

	player_ = new Player();
	player_->Initialize(modelPlayer_, playerPosition);

	player_->SetMapChipField(mapChipField_);

	GenerateBlocks();

	// カメラコントローラーの初期化
	cameraController_ = new CameraController();
	cameraController_->Initialize();
	cameraController_->SetTarget(player_);
	cameraController_->SetTargetCamera(&camera_);
	cameraController_->SetMovableArea({11.0f, 88.0f, 6.5f, 100.0f});
	cameraController_->Reset();

	// Enemy モデルの生成
	modelEnemy_ = Model::CreateFromOBJ("enemy", true);

	// Enemy の生成と初期化
	enemy_ = new Enemy();
	enemy_->Initialize(modelEnemy_, &camera_);
}

void GameScene::Update() {

	// ブロックの更新
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {

			if (!worldTransformBlock) {
				continue;
			}

			WorldTransformUpdate(*worldTransformBlock);
		}
	}

#ifdef _DEBUG

	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		if (isDebugCameraActive_) {
			isDebugCameraActive_ = false;
		} else {
			isDebugCameraActive_ = true;
		}
	}

#endif // DEBUG

	// カメラの処理
	if (isDebugCameraActive_) {
		debugCamera_->Update();
		camera_.matView = debugCamera_->GetCamera().matView;
		camera_.matProjection = debugCamera_->GetCamera().matProjection;

		// ビュープロジェクション行列の転送
		camera_.TransferMatrix();
	} else {
		// ビュープロジェクション行列の更新と転送
		camera_.UpdateMatrix();
	}

	// 天球の更新処理
	skydome_->Update();

	// プレイヤーの更新処理
	player_->Update();

	// カメラコントローラーの初期化
	cameraController_->Update();

	// 敵
	if (enemy_) {
		enemy_->Update();
	}
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

			model_->Draw(*worldTransformBlock, camera_);
		}
	}

	// 天球の描画処理
	skydome_->Draw(camera_);

	// プレイヤーの描画
	player_->Draw(camera_);

	// 敵
	if (enemy_) {
		enemy_->Draw();
	}

	Model::PostDraw();
}

GameScene::~GameScene() {

	// モデルデータの解放
	delete model_;

	// デバックカメラの解放
	delete debugCamera_;

	// 3Dモデルの解放
	delete modelSkydome_;

	// マップチップフィールドの解放
	delete mapChipField_;

	// ブロック用ワールドトランスフォームの解放
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}

	delete modelEnemy_;
	delete enemy_;

	worldTransformBlocks_.clear();
}

void GameScene::GenerateBlocks() {

	// 要素数
	uint32_t numBlockVertical = mapChipField_->GetNumBlockVirtical();
	uint32_t numBlockHorizontal = mapChipField_->GetNumBlockHorizontal();

	// 要素数を変更する
	// 列数を設定(縦方向のブロック数)
	worldTransformBlocks_.resize(numBlockVertical);

	for (uint32_t i = 0; i < numBlockVertical; ++i) {

		// 一列の要素数を指定(横方向のブロック数)
		worldTransformBlocks_[i].resize(numBlockHorizontal);
	}

	// キューブの生成
	for (uint32_t i = 0; i < numBlockVertical; ++i) {
		for (uint32_t j = 0; j < numBlockHorizontal; ++j) {
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {

				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();

				worldTransformBlocks_[i][j] = worldTransform;
				worldTransformBlocks_[i][j]->translation_ = mapChipField_->GetMatChipPositionByIndex(j, i);
			}
		}
	}
}
