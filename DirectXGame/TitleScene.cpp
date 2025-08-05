#include "TitleScene.h"

void TitleScene::Initialize() {

	model_ = Model::CreateFromOBJ("title", true);

	worldTransform_.Initialize();
	worldTransform_.translation_ = {0.0f, 0.0f, 0.0f};

	camera_.Initialize();
	camera_.translation_.z = -3.0f;

	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, kFadeDuration);
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
	camera_.UpdateMatrix();
}

void TitleScene::Draw() {
	Model::PreDraw(DirectXCommon::GetInstance()->GetCommandList());
	model_->Draw(worldTransform_, camera_);
	fade_->Draw();
	Model::PostDraw();
}

TitleScene::~TitleScene() { delete fade_; }
