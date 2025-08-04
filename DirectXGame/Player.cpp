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

	// 移動入力
	Move();

	// 衝突情報を初期化
	CollisionMapInfo collisionMapInfo;

	// 移動量に速度の値をコピー
	collisionMapInfo.moveAmount = velocity_;

	// マップ衝突チェック
	CheckMapCollision(collisionMapInfo);

	// 判定結果を反映して移動させる
	ApplyCollisionResult(collisionMapInfo);

	// 天井接触判定
	HandleCeilingCollision(collisionMapInfo);

	// 壁に接触した場合
	HandleWallCollision(collisionMapInfo);

	// 接地判定
	UpdateGroundState(collisionMapInfo);

	//// 着地フラグ
	// bool landing = false;

	//// 地面との当たり判定
	//// 下降中
	// if (velocity_.y < 0) {
	//	// y座標が地面以下になったら着地
	//	if (worldTransform_.translation_.y <= 1.0f) {
	//		landing = true;
	//	}
	// }

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

	// 行列更新
	WorldTransformUpdate(worldTransform_);

	DebugText::GetInstance()->ConsolePrintf("onGround: %d, vel.x: %.3f, trans.y: %.3f\n", onGround_ ? 1 : 0, velocity_.x, worldTransform_.translation_.y);
}

void Player::Draw(Camera& camera) {

	// DirectXCommonインスタンスの生成
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	Model::PreDraw(dxCommon->GetCommandList());

	// 3Dモデル描画
	model_->Draw(worldTransform_, camera);
	// DebugText::GetInstance()->ConsolePrintf("world.x: %.3f  world.y: %.3f\n", worldTransform_.translation_.x, worldTransform_.translation_.y);

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
	MapChipType mapChipTypeNext;

	// 真上の当たり判定を行う
	bool hit = false;

	// 左上点の判定
	MapChipField::IndexSet indexSet;

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);

	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	// 右上点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);

	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);

		// ブロックの矩形を取得
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);

		float epsilon = 0.001f;
		float yMoveAmount = (rect.bottom - worldTransform_.translation_.y) - kHeight / 2.0f - epsilon;

		// Y方向の移動量を制限（0以下にならないように）
		info.moveAmount.y = std::max(0.0f, yMoveAmount);

		// フラグを立てる
		info.isHitCeiling = true;
	}
}

void Player::CheckMapCollisionDown(CollisionMapInfo& info) {

	// 下降中？
	if (info.moveAmount.y >= 0) {
		return;
	}

	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {

		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.moveAmount, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;

	// 真下の当たり判定を行う
	bool hit = false;

	// 左下点の判定
	MapChipField::IndexSet indexSet;

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);

	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	// 右下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);

	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);

		// ブロックの矩形を取得
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);

		float epsilon = 0.001f;

		//                 /<-                          青矢印                        -> /   / <-   緑矢印  -> /
		float yMoveAmount = rect.top - worldTransform_.translation_.y + (kHeight / 2.0f) + epsilon;
		// Y方向の移動量を制限（0以下にならないように）
		info.moveAmount.y = std::min(0.0f, yMoveAmount);

		// フラグを立てる
		info.isGrounded = true;
	}
}

void Player::CheckMapCollisionRight(CollisionMapInfo& info) {

	if (info.moveAmount.x <= 0) {
		return;
	}

	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {

		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.moveAmount, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;

	// 真右の当たり判定を行う
	bool hit = false;

	// 右上点の判定
	MapChipField::IndexSet indexSet;

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex - 1, indexSet.yIndex);

	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	// 右下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex - 1, indexSet.yIndex);

	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {

		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);

		// ブロックの矩形を取得
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);

		float epsilon = 0.01f;
		float xMoveAmount = (rect.left - worldTransform_.translation_.x) - kWidth / 2.0f - epsilon;

		// X方向の移動量を制限（0以下にならないように）
		// info.moveAmount.x = std::max(0.0f, xMoveAmount);

		info.moveAmount.x = std::min(info.moveAmount.x, std::max(0.0f, xMoveAmount));

		info.isHitWall = true;
	}
}

void Player::CheckMapCollisionLeft(CollisionMapInfo& info) {

	if (info.moveAmount.x >= 0) {
		return;
	}

	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {

		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.moveAmount, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;

	// 真右の当たり判定を行う
	bool hit = false;

	// 左上点の判定
	MapChipField::IndexSet indexSet;

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex + 1, indexSet.yIndex);

	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	// 左下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex + 1, indexSet.yIndex);

	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {

		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);

		// ブロックの矩形を取得
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);

		float epsilon = 0.001f;
		float xMoveAmount = rect.right - worldTransform_.translation_.x + (kWidth / 2.0f) + epsilon;

		// X方向の移動量を制限（0以下にならないように）
		info.moveAmount.x = std::min(0.0f, xMoveAmount);

		info.isHitWall = true;
	}
}

Vector3 Player::CornerPosition(const KamataEngine::Vector3& center, Corner corner) {

	// Vector3 offsetTable[kNumCorner] = {
	//     {+kWidth / 2.0f, -kHeight / 2.0f, 0}, // kRightBottom
	//     {-kWidth / 2.0f, -kHeight / 2.0f, 0}, // kLeftBottom
	//     {+kWidth / 2.0f, +kHeight / 2.0f, 0}, // kRightTop
	//     {-kWidth / 2.0f, +kHeight / 2.0f, 0}  // kLeftTop
	// };

	// return center + offsetTable[static_cast<uint32_t>(corner)];

	if (corner == kRightBottom) {
		return center + Vector3{+kWidth / 2.0f, -kHeight / 2.0f, 0};
	} else if (corner == kLeftBottom) {
		return center + Vector3{-kWidth / 2.0f, -kHeight / 2.0f, 0};
	} else if (corner == kRightTop) {
		return center + Vector3{+kWidth / 2.0f, +kHeight / 2.0f, 0};
	} else {
		return center + Vector3{-kWidth / 2.0f, +kHeight / 2.0f, 0};
	}
}

void Player::ApplyCollisionResult(const CollisionMapInfo& info) {

	// 移動
	worldTransform_.translation_ += info.moveAmount;
}

void Player::HandleCeilingCollision(const CollisionMapInfo& info) {

	if (info.isHitCeiling) {
		velocity_.y = 0;
	}
}

void Player::UpdateGroundState(const CollisionMapInfo& info) {

	// 自キャラが接地状態？
	if (onGround_) {

		// ジャンプ開始
		if (velocity_.y > 0.0f) {
			onGround_ = false;
		} else {
			MapChipType mapChipType;

			//	// 真下の当たり判定を行う
			bool hit = false;

			std::array<Vector3, kNumCorner> positionsNew;

			for (uint32_t i = 0; i < positionsNew.size(); ++i) {

				positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.moveAmount, static_cast<Corner>(i)) + Vector3(0.0f, -kGroundAdhesionOffset, 0.0f);
			}

			//	// 左下点の判定
			MapChipField::IndexSet indexSet;

			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

			if (mapChipType == MapChipType::kBlock) {
				hit = true;
			}

			// 右下点の判定
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

			if (mapChipType == MapChipType::kBlock) {
				hit = true;
			}

			// 落下開始
			if (!hit) {
				// 空中状態に切り替える
				onGround_ = false;
			}
		}
	} else {
		if (info.isGrounded) {
			// 着地に切り替え
			onGround_ = true;

			// 着地時にX速度を減衰
			velocity_.x *= (1.0f - kAttenuationLanding);

			// Y速度を0にする
			velocity_.y = 0.0f;
		}
	}
}

void Player::HandleWallCollision(const CollisionMapInfo& info) {

	// 壁接触による減速
	if (info.isHitWall) {
		velocity_.x *= (1.0f - kAttenuationWall);
	}
}
