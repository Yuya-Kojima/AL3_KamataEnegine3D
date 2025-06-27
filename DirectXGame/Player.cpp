#define NOMINMAX
#include "Player.h"
#include "MapChipField.h"
#include "VectorMath.h"
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

		// 移動入力
		Move();

		// 衝突情報を初期化
		CollisionMapInfo collisionMapInfo;

		// 移動量に速度の値をコピー
		collisionMapInfo.moveAmount = velocity_;

		// マップ衝突チェック
		CheckMapCollision(collisionMapInfo);

		// if (Input::GetInstance()->PushKey(DIK_RIGHT) || Input::GetInstance()->PushKey(DIK_LEFT)) {

		//	// 左右加速
		//	Vector3 acceleration = {};

		//	if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
		//		// 左移動中の右入力
		//		if (velocity_.x < 0.0f) {
		//			velocity_.x *= (1.0f - kAttenuation_);
		//		}
		//		acceleration.x += kAcceleration_;

		//		if (lrDirection_ != LRDirection ::kRight) {
		//			lrDirection_ = LRDirection::kRight;

		//			// 旋回開始時の角度を記録
		//			turnFirstRotationY_ = worldTransform_.rotation_.y;

		//			// 旋回タイマーに時間を設定する
		//			turnTimer_ = kTimeTurn;
		//		}
		//	} else if (Input::GetInstance()->PushKey(DIK_LEFT)) {
		//		// 右移動中の左入力
		//		if (velocity_.x > 0.0f) {
		//			// 速度と逆方向に入力中は急ブレーキ
		//			velocity_.x *= (1.0f - kAttenuation_);
		//		}
		//		acceleration.x -= kAcceleration_;
		//		if (lrDirection_ != LRDirection ::kLeft) {
		//			lrDirection_ = LRDirection::kLeft;

		//			// 旋回開始時の角度を記録
		//			turnFirstRotationY_ = worldTransform_.rotation_.y;

		//			// 旋回タイマーに時間を設定する
		//			turnTimer_ = kTimeTurn;
		//		}
		//	}

		//	// 加速　減速
		//	velocity_ = Add(velocity_, acceleration);

		//	// 最大速度制限
		//	velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed_, kLimitRunSpeed_);
		//} else {

		//	// 非入力時は移動減衰
		//	velocity_.x *= (1.0f - kAttenuation_);
		//}

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

void Player::Move() {

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
	}
}

void Player::CheckMapCollision(CollisionMapInfo& info) {
	CheckMapCollisionUp(info);
	CheckMapCollisionDown(info);
	CheckMapCollisionRight(info);
	CheckMapCollisionLeft(info);
}

void Player::CheckMapCollisionUp(CollisionMapInfo& info) {

	// 移動後の四つの角の座標
	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {

		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.moveAmount, static_cast<Corner>(i));
	}

	if (info.moveAmount.y <= 0) {
		return;
	}

	MapChipType mapChipType;

	// 真上の当たり判定を行う
	bool hit = false;

	// 左上点の判定
	MapChipField::IndexSet indexSet;

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// 右上点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
}

Vector3 Player::CornerPosition(const KamataEngine::Vector3& center, Corner corner) {

	Vector3 offsetTable[kNumCorner] = {
	    {+kWidth / 2.0f, -kHeight / 2.0f, 0}, // kRightBottom
	    {-kWidth / 2.0f, -kHeight / 2.0f, 0}, // kLeftBottom
	    {+kWidth / 2.0f, +kHeight / 2.0f, 0}, // kRightTop
	    {-kWidth / 2.0f, +kHeight / 2.0f, 0}  // kLeftTop
	};

	return center + offsetTable[static_cast<uint32_t>(corner)];
}
