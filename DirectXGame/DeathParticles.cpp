#include "DeathParticles.h"

void DeathParticles::Initialize(Model* model, Camera* camera, Vector3 position) {

	model_ = model;
	camera_ = camera;

	// ワールド変換の初期化
	for (WorldTransform& worldTransform : worldTransforms_) {
		worldTransform.Initialize();
		worldTransform.translation_ = position;
	}

	objectColor_.Initialize();
	color_ = {1, 1, 1, 1};
}

void DeathParticles::Update() {
	for (uint32_t i = 0; i < kNumParticles; ++i) {

		if (isFinished_) {
			return;
		}

		color_.w = std::clamp(1.0f - counter_ / kDuration, 0.0f, 1.0f);
		// 色変更オブジェクトに色の数値を設定する
		objectColor_.SetColor(color_);	

		// 基本となる速度ベクトル
		Vector3 velocity = {kSpeed, 0, 0};

		// 回転角を計算する
		float angle = kAngleUnit * i;

		// Z軸まわり回転行列
		Matrix4x4 matrixRotation = MakeRotateZMatrix(angle);

		// 回転して速度ベクトルを得る
		velocity = Transform(velocity, matrixRotation);

		// 移動処理
		worldTransforms_[i].translation_ += velocity;

		// ワールド変換更新
		WorldTransformUpdate(worldTransforms_[i]);
	}
	// カウンターを1フレーム分の秒数進める
	counter_ += 1.0f / 60.0f;

	// 有効時間の上限に達したら
	if (counter_ >= kDuration) {
		counter_ = kDuration;
		// 終了扱いにする
		isFinished_ = true;
	}
}

void DeathParticles::Draw() {

	if (isFinished_) {
		return;
	}

	// DirectXCommonインスタンスの生成
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	Model::PreDraw(dxCommon->GetCommandList());

	for (const WorldTransform& worldTransform : worldTransforms_) {
		model_->Draw(worldTransform, *camera_, &objectColor_);
	}

	Model::PostDraw();
}

Vector3 Transform(const Vector3& vec, const Matrix4x4& mat) {
	{
		KamataEngine::Vector3 result;
		result.x = vec.x * mat.m[0][0] + vec.y * mat.m[1][0] + vec.z * mat.m[2][0];
		result.y = vec.x * mat.m[0][1] + vec.y * mat.m[1][1] + vec.z * mat.m[2][1];
		result.z = vec.x * mat.m[0][2] + vec.y * mat.m[1][2] + vec.z * mat.m[2][2];
		return result;
	}
}
