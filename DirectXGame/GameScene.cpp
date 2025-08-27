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
	modelSlimeInner_ = Model::CreateFromOBJ("slime_inner", true);
	modelSlimeOuter_ = Model::CreateFromOBJ("slime_outer",true);
	attackPlayer_ = Model::CreateFromOBJ("attackEffect", true);

	Vector3 playerPosition = mapChipField_->GetMatChipPositionByIndex(1, 18);

	player_ = new Player();
	player_->Initialize(modelSlimeInner_,modelSlimeOuter_, attackPlayer_, playerPosition);

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

		newEnemy->SetMapChipField(mapChipField_);

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

	// ゴール
	goalModel_ = Model::CreateFromOBJ("goal", true);
	goalPos_ = mapChipField_->GetMatChipPositionByIndex(30, 18);
	goal_.Initialize(goalPos_);

	clearTextModel_ = Model::CreateFromOBJ("clear", true);
	clearTextWT.Initialize();
	clearTextWT.translation_ = goalPos_;
	clearTextWT.translation_.x += 10.0f;
	clearTextWT.translation_.y += 3.0f;
	clearTextWT.translation_.z -= 2.0f;
	clearTextWT.scale_ *= 2.0f;
	WorldTransformUpdate(clearTextWT);
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

	case Phase::kClear: {

		clearTimer_ += 1.0f / 60.0f;

		float t = std::min(clearTimer_ / 2.0f, 1.0f);
		float pulse = 1.0f + 0.2f * sinf(t * 3.14159f);
		goal_.SetScale({pulse, pulse, pulse});

		if (clearTimer_ >= 2.0f) {
			fade_->Start(Fade::Status::FadeOut, kFadeDuration);
			phase_ = Phase::kFadeOut;
		}

		goal_.Update();

		// クリア時のテキスト
		// イージング
		auto Saturate = [](float x) { return x < 0 ? 0 : (x > 1 ? 1 : x); };

		auto EaseOutBack = [&](float t) {
			t = Saturate(t);
			float y = t - 1.0f;
			const float s = 1.70158f;
			return 1.0f + (s + 1.0f) * y * y * y + s * y * y;
		};

		KamataEngine::Vector3 end = goalPos_;
		end.y += 3.0f;
		end.z -= 2.0f;

		float u = clearTimer_ / clearMaxTime_;

		if (u > 1.0f) {
			u = 1.0f;
		}

		float e = EaseOutBack(u);

		KamataEngine::Vector3 offset = {10.0f, 0.0f, 0.0f};

		float pos = end.x + offset.x * (1.0f - e);

		clearTextWT.translation_.y = 3.0f + std::sin(clearTimer_);

		clearTextWT.translation_.x = pos;

		WorldTransformUpdate(clearTextWT);

		break;
	}

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

	if (player_->GetIsClear()) {
		clearTextModel_->Draw(clearTextWT, camera_);
	}
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

	// ゴール
	goal_.Draw(&camera_, goalModel_);

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

	delete goalModel_;
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

	// 自キャラとゴールの当たり判定
	if (!player_->GetIsClear()) {

		aabb2 = goal_.GetAABB();

		if (IsCollision(aabb1, aabb2)) {
			player_->MarkClear();
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

		if (player_->GetIsClear()) {

			phase_ = Phase::kClear;
			clearTimer_ = 0.0f;
		}

		break;

	case Phase::kDeath:
		// デス演出フェーズの処理（今は何もしない）
		break;

	case Phase::kClear:

		break;
	}
}

void GameScene::CreateHitEffect(const Vector3& origin) {

	HitEffect* newHitEffect = HitEffect::Create(origin);
	hitEffects_.push_back(newHitEffect);
}
