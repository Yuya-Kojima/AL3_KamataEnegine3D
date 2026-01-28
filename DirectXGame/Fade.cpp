#include "Fade.h"

void Fade::Initialize() {

	textureHandle = TextureManager::Load("black1x1.png");

	// スプライト生成
	sprite_ = Sprite::Create(textureHandle, {0.0f, 0.0f});

	// 画面サイズに合わせてスプライトの大きさを設定（仮に1280x720とする）
	sprite_->SetSize(Vector2(1280, 720));

	// 黒色に設定（透明度1.0f = 完全不透明）
	sprite_->SetColor(Vector4(0, 0, 0, 1));
}

void Fade::Update() {

	float alpha = 0.0f;

	switch (status_) {
	case Status::None:
		// 何もしない
		break;

	case Status::FadeIn:

		counter_ += 1.0f / 60.0f;

		if (counter_ >= duration_) {
			counter_ = duration_;
			status_ = Status::None; // フェードイン完了
		}

		alpha = 1.0f - std::clamp(counter_ / duration_, 0.0f, 1.0f);
		sprite_->SetColor(Vector4(0, 0, 0, alpha));

		break;

	case Status::FadeOut:

		// 1フレーム分の秒数をカウントアップ
		counter_ += 1.0f / 60.0f;

		// フェード継続時間に達したら打ち止め
		counter_ = std::min(counter_, duration_);

		// アルファ値を 0.0f 〜 1.0f に変化させる
		alpha = std::clamp(counter_ / duration_, 0.0f, 1.0f);
		sprite_->SetColor(Vector4(0, 0, 0, alpha));

		break;
	}
}

void Fade::Draw() {

	if (status_ == Status::None) {
		return;
	}

	Sprite::PreDraw(DirectXCommon::GetInstance()->GetCommandList());
	sprite_->Draw();
	Sprite::PostDraw();
}

void Fade::Start(Status status, float duration) {
	status_ = status;
	duration_ = duration;
	counter_ = 0.0f;
}

void Fade::Stop() { status_ = Status::None; }

bool Fade::IsFinished() const {
	switch (status_) {
	case Status::FadeIn:
	case Status::FadeOut:
		return counter_ >= duration_;
	}

	return true;
}