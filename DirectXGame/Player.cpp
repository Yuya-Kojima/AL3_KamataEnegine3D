#define NOMINMAX
#include "Player.h"
#include <algorithm>

using namespace KamataEngine;

void Player::Initialize(Model* model, Vector3& position) {

	// 3Dモデルの初期化
	model_ = model;

	// ワールド変換データの初期化
	// worldTransform_ = new WorldTransform();
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;

	// カメラの初期化
	camera_.Initialize();
}

void Player::Update() {

	if (onGround_) {
		if (Input::GetInstance()->PushKey(DIK_RIGHT) || Input::GetInstance()->PushKey(DIK_LEFT)) {

			// 左右加速
			Vector3 acceleration = {};

			if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
				// 左移動中の右入力
				if (velocity_.x < 0.0f) {
					velocity_.x *= (1.0f - kAttenuation_);
				}
				acceleration.x += kAcceleration_;

				if (lrDirection_ != LRDirection ::kRight) {
					lrDirection_ = LRDirection::kRight;

					// 旋回開始時の角度を記録
					turnFirstRotationY_ = worldTransform_.rotation_.y;

					// 旋回タイマーに時間を設定する
					turnTimer_ = kTimeTurn;
				}
			} else if (Input::GetInstance()->PushKey(DIK_LEFT)) {
				// 右移動中の左入力
				if (velocity_.x > 0.0f) {
					// 速度と逆方向に入力中は急ブレーキ
					velocity_.x *= (1.0f - kAttenuation_);
				}
				acceleration.x -= kAcceleration_;
				if (lrDirection_ != LRDirection ::kLeft) {
					lrDirection_ = LRDirection::kLeft;

					// 旋回開始時の角度を記録
					turnFirstRotationY_ = worldTransform_.rotation_.y;

					// 旋回タイマーに時間を設定する
					turnTimer_ = kTimeTurn;
				}
			}

			// 加速　減速
			velocity_ = Add(velocity_, acceleration);

			// 最大速度制限
			velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed_, kLimitRunSpeed_);
		} else {

			// 非入力時は移動減衰
			velocity_.x *= (1.0f - kAttenuation_);
		}

		if (Input::GetInstance()->PushKey(DIK_UP)) {

			// ジャンプ初速
			velocity_.y += kJumpAcceleration;
		}
	} else {

		// 落下速度
		velocity_.y += -kGravityAcceleration;

		// 落下速度制限
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
	}

	// 着地フラグ
	bool landing = false;

	// 地面との当たり判定
	// 下降中
	if (velocity_.y < 0) {
		// y座標が地面以下になったら着地
		if (worldTransform_.translation_.y <= 1.0f) {
			landing = true;
		}
	}
	if (onGround_) {
		// ジャンプ開始
		if (velocity_.y > 0.0f) {
			// 空中状態に移行
			onGround_ = false;
		}
	} else {
		// 着地
		if (landing) {
			// めりこみ排斥
			worldTransform_.translation_.y = 1.0f;
			// 摩擦で横方向速度が減衰する
			velocity_.x *= (1.0f - kAttenuation_);
			// 下方向速度をリセット
			velocity_.y = 0.0f;
			// 接地状態に移行
			onGround_ = true;
		}
	}

	// 旋回制御
	if (turnTimer_ > 0.0f) {

		turnTimer_ -= 0.03f;

		// 左右の自キャラ角度テーブル
		float destinationRotationYTable[] = {
		    std::numbers::pi_v<float> / 2.0f,
		    std::numbers::pi_v<float> * 3.0f / 2.0f,
		};

		// 状況に応じた
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];

		float t = 1.0f - (turnTimer_ / kTimeTurn);

		// EaseInOut
		float easedT = t * t * (3.0f - 2.0f * t);

		// 自キャラの角度を設定する
		worldTransform_.rotation_.y = (1.0f - easedT) * turnFirstRotationY_ + easedT * destinationRotationY;
	}

	// 移動
	worldTransform_.translation_ = Add(worldTransform_.translation_, velocity_);

	// 行列更新
	WorldTransformUpdate(worldTransform_);
}

void Player::Draw(Camera& camera) {

	// DirectXCommonインスタンスの生成
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	Model::PreDraw(dxCommon->GetCommandList());

	// 3Dモデル描画
	model_->Draw(worldTransform_, camera);

	Model::PostDraw();
}

Player::~Player() {

	// 3Dモデルの解放
	delete model_;

	//	delete worldTransform_;
}