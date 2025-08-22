#include "HitEffect.h"
#include <algorithm>
#include <numbers>
#include <random>

// 静的メンバ変数の実体
Model* HitEffect::model_ = nullptr;
Camera* HitEffect::camera_ = nullptr;

namespace {
inline float EaseOutCubic(float t) {
	t = std::clamp(t, 0.0f, 1.0f);
	float u = 1.0f - t;
	return 1.0f - u * u * u;
}
} // namespace

void HitEffect::Initialize(const Vector3& origin) {

	// 円形
	circleWorldTransform_.Initialize();
	circleWorldTransform_.translation_ = origin;
	circleWorldTransform_.rotation_.y = std::numbers::pi_v<float>;
	circleWorldTransform_.scale_ = {0.6f, 0.6f, 1.0f};
	WorldTransformUpdate(circleWorldTransform_);

	// 乱数
	static std::random_device seedGenerator;
	static std::mt19937_64 randomEngine(seedGenerator());

	std::uniform_real_distribution<float> rotationDistribution(-std::numbers::pi_v<float>, std::numbers::pi_v<float>);

	// 楕円
	for (WorldTransform& worldTransform : ellipseWorldTransforms_) {
		float randomYaw = rotationDistribution(randomEngine);
		worldTransform.scale_ = {1.5f, 0.1f, 1.0f};
		worldTransform.rotation_ = {0.0f, std::numbers::pi_v<float>, randomYaw};
		worldTransform.translation_ = origin;

		worldTransform.Initialize();
		WorldTransformUpdate(worldTransform);
	}
}

void HitEffect::Update() {

	timer_ += 1.0f / 60.0f;

	switch (state_) {
	case HitEffect::State::kSpread: {

		float t = timer_ / kSpreadTime;
		float e = EaseOutCubic(t);

		const Vector3 s0{0.6f, 0.6f, 1.0f};
		const Vector3 s1{1.6f, 1.6f, 1.0f};
		circleWorldTransform_.scale_.x = s0.x + (s1.x - s0.x) * e;
		circleWorldTransform_.scale_.y = s0.y + (s1.y - s0.y) * e;

		// 円形エフェクト
		WorldTransformUpdate(circleWorldTransform_);

		// 楕円

		const float ex0 = 1.5f, ex1 = 3.0f;
		const float ey0 = 0.1f, ey1 = 0.2f;

		for (WorldTransform& worldTransform : ellipseWorldTransforms_) {
			worldTransform.scale_.x = ex0 + (ex1 - ex0) * e;
			worldTransform.scale_.y = ey0 + (ey1 - ey0) * e;

			WorldTransformUpdate(worldTransform);
		}

		if (t >= 1.0f) {
			state_ = State::kFade;
			timer_ = 0.0f;
		}

	} break;
	case HitEffect::State::kFade: {

		float t = timer_ / kFadeTime;
		float e = EaseOutCubic(t);
		opacity_ = 1.0f - e;

		if (t >= 1.0f) {
			state_ = State::kDead;
		}

	} break;
	case HitEffect::State::kDead:
	default:
		break;
	}
}

void HitEffect::Draw() {

	// DirectXCommonインスタンスの生成
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	Model::PreDraw(dxCommon->GetCommandList());

	model_->SetAlpha(opacity_);

	for (WorldTransform& worldTransform : ellipseWorldTransforms_) {
		model_->Draw(worldTransform, *camera_);
	}

	model_->Draw(circleWorldTransform_, *camera_);

	Model::PostDraw();
}

HitEffect* HitEffect::Create(const Vector3& origin) {

	// インスタンス生成
	HitEffect* instance = new HitEffect();

	// newの失敗を検出
	assert(instance);

	// インスタンスの初期化
	instance->Initialize(origin);

	// 初期化したインスタンスを返す
	return instance;
}
