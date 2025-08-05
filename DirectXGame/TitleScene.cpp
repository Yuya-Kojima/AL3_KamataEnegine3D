#include "TitleScene.h"

void TitleScene::Initialize() {

	model_ = Model::CreateFromOBJ("title", true);

	worldTransform_.Initialize();
	worldTransform_.translation_ = {0.0f, 0.0f, 0.0f};

	camera_.Initialize();
	camera_.translation_.z = -3.0f;
}

void TitleScene::Update() {

	if (Input::GetInstance()->PushKey(DIK_SPACE)) {
		finished_ = true;
	}

	time_ += 1.0f / 60.0f;

	float amplitude = 0.1f; // 揺れの大きさ
	float speed = 2.0f;     // 揺れる速さ（周期）
	worldTransform_.translation_.y = std::sin(time_ * speed) * amplitude;

	WorldTransformUpdate(worldTransform_);
	camera_.UpdateMatrix();
}

void TitleScene::Draw() {
	Model::PreDraw(DirectXCommon::GetInstance()->GetCommandList());
	model_->Draw(worldTransform_, camera_);
	Model::PostDraw();
}
