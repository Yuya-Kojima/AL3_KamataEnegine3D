#include "TitleScene.h"
#include <numbers>

void TitleScene::Initialize() {

	model_ = Model::CreateFromOBJ("title", true);

	startModel_ = Model::CreateFromOBJ("start", true);

	worldTransform_.Initialize();
	worldTransform_.translation_ = {-3.0f, 1.0f, 3.0f};

	startWorldTransform_.Initialize();
	startWorldTransform_.translation_ = {-4.0f, -2.0f, 10.0f};

	camera_.Initialize();
	camera_.translation_.z = -3.0f;

	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, kFadeDuration);

	backgroundModel_ = Model::CreateFromOBJ("background", true);

	worldTransformBack_.Initialize();
	worldTransformBack_.translation_ = {0.0f, 0.0f, 15.0f};
	worldTransformBack_.scale_ = {15.0f, 8.0f, 1.0f};
}

void TitleScene::Update() {

	switch (phase_) {
	case Phase::kFadeIn:
		fade_->Update();
		if (fade_->IsFinished()) {
			phase_ = Phase::kMain;
		}
		break;

	case Phase::kMain:
		// モデルの上下揺れだけ処理
		time_ += 1.0f / 60.0f;
		{
			float amplitude = 0.1f;
			float speed = 2.0f;
			worldTransform_.translation_.y = std::sin(time_ * speed) * amplitude;
		}

		blinkT_ += 1.0f / 60.0f;
		showPress_ = (std::sin(blinkT_ * 6.0f) > 0.0f);

		if (Input::GetInstance()->PushKey(DIK_SPACE)) {
			fade_->Start(Fade::Status::FadeOut, kFadeDuration);
			phase_ = Phase::kFadeOut;
		}
		break;

	case Phase::kFadeOut:
		fade_->Update();
		if (fade_->IsFinished()) {
			finished_ = true; // ゲームシーンなどへ遷移させるフラグ
		}
		break;
	}

	WorldTransformUpdate(worldTransform_);
	WorldTransformUpdate(startWorldTransform_);
	WorldTransformUpdate(worldTransformBack_);
	camera_.UpdateMatrix();
}

void TitleScene::Draw() {

	Model::PreDraw(DirectXCommon::GetInstance()->GetCommandList());

	backgroundModel_->Draw(worldTransformBack_, camera_);

	model_->Draw(worldTransform_, camera_);

	if (showPress_) {
		startModel_->Draw(startWorldTransform_, camera_);
	}

	fade_->Draw();
	Model::PostDraw();
}

TitleScene::~TitleScene() {
	delete model_;
	delete backgroundModel_;
	delete fade_;
}
