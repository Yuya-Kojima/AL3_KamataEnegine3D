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

	// マップチップフィールド
	mapChipField_ = new MapChipField;
	mapChipField_->LoadMapChipCsv("Resources/AL3_mapchip.csv");

	// プレイヤーの初期化
	modelPlayer_ = Model::CreateFromOBJ("player", true);
	attackPlayer_ = Model::CreateFromOBJ("attackEffect", true);

	Vector3 playerPosition = mapChipField_->GetMatChipPositionByIndex(1, 18);

	player_ = new Player();
	player_->Initialize(modelPlayer_, attackPlayer_, playerPosition);

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
	for (int32_t i = 0; i < 5; ++i) {
		Enemy* newEnemy = new Enemy();

		// 一体ずつ異なる座標をセット（例：X座標を10ずつずらす）
		Vector3 enemyPosition = {14.0f + i * 10.0f, 1.0f, 0.0f};

		newEnemy->Initialize(modelEnemy_, &camera_, enemyPosition);

		newEnemy->SetGameScene(this);

		enemies_.push_back(newEnemy);
	}

	// DeathParticles モデルの生成
	modelDeathParticles = Model::CreateFromOBJ("deathParticle", true);

	deathParticles_ = new DeathParticles();
	deathParticles_->Initialize(modelDeathParticles, &camera_, player_->GetWorldPosition());

	phase_ = Phase::kFadeIn; // フェーズ初期化

	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, kFadeDuration);

	// ヒットエフェクト
	modelHitEffect_ = Model::CreateFromOBJ("hitEffect", true);
	HitEffect::SetModel(modelHitEffect_);
	HitEffect::SetCamera(&camera_);
}

void GameScene::Update() {

	switch (phase_) {
	case Phase::kFadeIn:
		fade_->Update();

		// 天球の更新処理
		skydome_->Update();

		// プレイヤーの更新処理
		player_->Update();

		// 敵
		for (Enemy* enemy : enemies_) {
			enemy->Update();
		}

		cameraController_->Update();
		camera_.UpdateMatrix();

		// ブロックの更新
		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
			for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {

				if (!worldTransformBlock) {
					continue;
				}

				WorldTransformUpdate(*worldTransformBlock);
			}
		}

		if (fade_->IsFinished()) {
			phase_ = Phase::kPlay;
		}

		// ヒットエフェクト
		for (HitEffect* hitEffect : hitEffects_) {
			hitEffect->Update();
		}

		break;
	case Phase::kPlay:

		// 天球の更新処理
		skydome_->Update();

		// プレイヤーの更新処理
		player_->Update();

		// 敵
		for (Enemy* enemy : enemies_) {
			enemy->Update();
		}

		enemies_.remove_if([](Enemy* enemy) {
			if (enemy->GetIsDead()) {
				delete enemy;
				return true;
			}

			return false;
		});

		// カメラコントローラーの初期化
		cameraController_->Update();

#ifdef _DEBUG

		if (Input::GetInstance()->TriggerKey(DIK_RETURN)) {
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

		// ブロックの更新
		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
			for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {

				if (!worldTransformBlock) {
					continue;
				}

				WorldTransformUpdate(*worldTransformBlock);
			}
		}

		// ヒットエフェクト
		for (HitEffect* hitEffect : hitEffects_) {
			hitEffect->Update();
		}

		hitEffects_.remove_if([](HitEffect* hitEffect) {
			if (hitEffect->isDead()) {
				delete hitEffect;
				return true;
			}
			return false;
		});

		// 全ての当たり判定を行う
		CheckAllCollisions();

		ChangePhase();

		break;
	case Phase::kDeath:

		// 天球の更新処理
		skydome_->Update();

		// 敵
		for (Enemy* enemy : enemies_) {
			enemy->Update();
		}

		// デスパーティクルの更新
		if (deathParticles_) {
			deathParticles_->Update();
		}

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

		// ブロックの更新
		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
			for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {

				if (!worldTransformBlock) {
					continue;
				}

				WorldTransformUpdate(*worldTransformBlock);
			}
		}

		if (deathParticles_ && deathParticles_->IsFinished()) {
			fade_->Start(Fade::Status::FadeOut, kFadeDuration);
			phase_ = Phase::kFadeOut;
		}

		// ヒットエフェクト
		for (HitEffect* hitEffect : hitEffects_) {
			hitEffect->Update();
		}

		break;

	case Phase::kFadeOut:
		fade_->Update();
		if (fade_->IsFinished()) {
			finished_ = true;
		}
		break;
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
	if (phase_ == Phase::kFadeIn || phase_ == Phase::kPlay) {
		player_->Draw(camera_);
	}

	// 敵
	for (Enemy* enemy : enemies_) {
		enemy->Draw();
	}

	if (deathParticles_) {
		deathParticles_->Draw();
	}

	fade_->Draw();

	// ヒットエフェクト
	for (HitEffect* hitEffect : hitEffects_) {
		hitEffect->Draw();
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
	for (Enemy* enemy : enemies_) {
		delete enemy;
	}
	enemies_.clear();

	worldTransformBlocks_.clear();

	// デスパーティクルの解放
	delete deathParticles_;
	deathParticles_ = nullptr;

	// ヒットエフェクトモデルの解放
	delete modelHitEffect_;
	for (HitEffect* hitEffect : hitEffects_) {
		delete hitEffect;
	}
	hitEffects_.clear();
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

void GameScene::CheckAllCollisions() {

	// 判定対象1と2の座標
	AABB aabb1, aabb2;

	// 自キャラの座標
	aabb1 = player_->GetAABB();

	// 自キャラと敵弾全ての当たり判定
	for (Enemy* enemy : enemies_) {

		if (enemy->IsCollisionDisabled())
			continue; // コリジョン無効の敵はスキップ

		// 敵弾の座標
		aabb2 = enemy->GetAABB();

		// AABB同士の交差判定
		if (IsCollision(aabb1, aabb2)) {
			// 自キャラの衝突時関数を呼び出す
			player_->OnCollision(enemy);

			// 敵の衝突時関数を呼び出す
			enemy->OnCollision(player_);
		}
	}
}

void GameScene::ChangePhase() {

	switch (phase_) {
	case Phase::kPlay:
		if (player_->IsDead()) {
			// フェーズ切り替え
			phase_ = Phase::kDeath;

			// デスパーティクルの位置をプレイヤーの位置に
			const Vector3 deathParticlesPosition = player_->GetWorldPosition();

			// デスパーティクルの生成と初期化
			deathParticles_ = new DeathParticles();
			deathParticles_->Initialize(modelDeathParticles, &camera_, deathParticlesPosition);
		}
		break;

	case Phase::kDeath:
		// デス演出フェーズの処理（今は何もしない）
		break;
	}
}

void GameScene::CreateHitEffect(const Vector3& origin) {

	HitEffect* newHitEffect = HitEffect::Create(origin);
	hitEffects_.push_back(newHitEffect);
}
