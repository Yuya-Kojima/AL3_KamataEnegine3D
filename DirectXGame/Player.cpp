#define NOMINMAX
#include "Player.h"
#include "MapChipField.h"
#include "VectorMath.h"
#include <algorithm>
#include <cfloat>
#include <cmath>
#include <imgui.h>

using namespace KamataEngine;

namespace {

bool HitBlockExpanded3x3(MapChipField* field, const KamataEngine::Vector3& p, float assist, MapChipField::IndexSet& outIdx) {
	if (!field) {
		return false;
	}

	auto base = field->GetMapChipIndexSetByPosition(p);

	const uint32_t w = field->GetNumBlockHorizontal();
	const uint32_t h = field->GetNumBlockVirtical();

	for (int dy = -1; dy <= 1; ++dy) {
		for (int dx = -1; dx <= 1; ++dx) {
			const int ix = static_cast<int>(base.xIndex) + dx;
			const int iy = static_cast<int>(base.yIndex) + dy;
			if (ix < 0 || iy < 0) {
				continue;
			}
			if (static_cast<uint32_t>(ix) >= w || static_cast<uint32_t>(iy) >= h) {
				continue;
			}

			const uint32_t ux = static_cast<uint32_t>(ix);
			const uint32_t uy = static_cast<uint32_t>(iy);

			if (field->GetMapChipTypeByIndex(ux, uy) != MapChipType::kBlock) {
				continue;
			}

			// ブロックのRectを取得して膨らませる（探索を緩くする）
			MapChipField::Rect r = field->GetRectByIndex(ux, uy);

			const float left = r.left - assist;
			const float right = r.right + assist;
			const float bottom = r.bottom - assist;
			const float top = r.top + assist;

			if (p.x >= left && p.x <= right && p.y >= bottom && p.y <= top) {
				outIdx = {ux, uy};
				return true;
			}
		}
	}

	return false;
}

} // namespace

void Player::Initialize(Model* innerModel, Model* outerModel, Model* modelAttack, Vector3& position) {

	// 3Dモデルの初期化
	innerModel_ = innerModel;
	outerModel_ = outerModel;

	// ワールド変換データの初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;

	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;

	WorldTransformUpdate(worldTransform_);

	color_.Initialize();
	color_.SetColor({1.0f, 1.0f, 1.0f, 0.5f});

	modelAttack_ = modelAttack;
	worldTransformAttack_.Initialize();
	attackEffectVisible_ = false;

	WorldTransformUpdate(worldTransformAttack_);

	// カメラの初期化
	camera_.Initialize();

	wireVisualTransform_.Initialize();
	wireAnchorVisualTransform_.Initialize();
}

void Player::Update() {

	if (behaviorRequest_ != Behavior::kUnknown) {
		// ふるまいを変更する
		behavior_ = behaviorRequest_;

		// 各ふるまいごとの初期化を実行
		switch (behavior_) {
		case Behavior::kRoot:
		default:

			BehaviorRootInitialize();
			BehaviorRootUpdate();

			break;

		case Behavior::kAttack:

			BehaviorAttackInitialize();
			BehaviorAttackUpdate();

			break;
		}
		// ふるまいリクエストをリセット
		behaviorRequest_ = Behavior::kUnknown;
	}

	switch (behavior_) {
	case Behavior::kRoot:
	default:

		BehaviorRootUpdate();

		break;

	case Behavior::kAttack:

		BehaviorAttackUpdate();

		break;
	}
}

void Player::Draw(Camera& camera) {

	// DirectXCommonインスタンスの生成
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	Model::PreDraw(dxCommon->GetCommandList());

	// 3Dモデル描画
	innerModel_->Draw(worldTransform_, camera);

	outerModel_->Draw(worldTransform_, camera, &color_);

	if (behavior_ == Behavior::kAttack) {
		modelAttack_->Draw(worldTransformAttack_, camera);
	}

	// ワイヤー可視化
	if (isWireVisualVisible_ && (wireState_ == WireState::kFlying || wireState_ == WireState::kAttached)) {

		KamataEngine::Model* wireModel = outerModel_;

		// 始点は発射口
		KamataEngine::Vector3 p = worldTransform_.translation_;
		p.y += 1.2f;
		p.x += (lrDirection_ == LRDirection::kRight) ? 0.6f : -0.6f;

		// 終点：Flying中は先端、Attachedはアンカー
		KamataEngine::Vector3 a = {};
		if (wireState_ == WireState::kFlying) {
			a = VectorMath::Add(p, VectorMath::Multiply(wireFlyLength_, wireDir_));
		} else {
			a = wireAnchor_;
		}

		// ワイヤー本体
		KamataEngine::Vector3 d = {a.x - p.x, a.y - p.y, a.z - p.z};
		float len = std::sqrt(d.x * d.x + d.y * d.y + d.z * d.z);
		if (len > 0.0001f) {

			wireVisualTransform_.translation_ = {(p.x + a.x) * 0.5f, (p.y + a.y) * 0.5f, (p.z + a.z) * 0.5f};
			wireVisualTransform_.scale_ = {kWireVisualThickness_, kWireVisualThickness_, len};

			float yaw = std::atan2(d.x, d.z);
			float xz = std::sqrt(d.x * d.x + d.z * d.z);
			float pitch = -std::atan2(d.y, xz);
			wireVisualTransform_.rotation_ = {pitch, yaw, 0.0f};

			WorldTransformUpdate(wireVisualTransform_);
			wireModel->Draw(wireVisualTransform_, camera);
		}
	}

	Model::PostDraw();
}

Player::~Player() {

	// 3Dモデルの解放
	delete innerModel_;
	delete outerModel_;
}

void Player::Move() {

	auto* in = Input::GetInstance();

	const bool shiftDown = in->PushKey(DIK_LSHIFT);
	const bool shiftTrig = in->TriggerKey(DIK_LSHIFT);
	const bool shiftRel = (!shiftDown && wasShiftDown_);

	// 押した瞬間：発射開始（Noneのときだけ）
	if (shiftTrig && wireState_ == WireState::kNone) {
		TryStartWire();
	}

	// 押している間だけ更新（←ここが最重要）
	if (shiftDown) {
		if (wireState_ == WireState::kFlying || wireState_ == WireState::kAttached) {
			UpdateWire();
		}
	}

	// 離した瞬間：Attachedならリリース、Flyingなら失敗終了
	if (shiftRel) {
		if (wireState_ == WireState::kAttached) {
			ReleaseWire();
		} else if (wireState_ == WireState::kFlying) {
			EndWire();
		}
	}

	wasShiftDown_ = shiftDown;

	// Attached中は通常移動入力は止める（ワイヤー物理だけで動かす）
	if (wireState_ == WireState::kAttached) {
		return;
	}

	if (onGround_) {
		if (Input::GetInstance()->PushKey(DIK_RIGHT) || Input::GetInstance()->PushKey(DIK_LEFT) || Input::GetInstance()->PushKey(DIK_A) || Input::GetInstance()->PushKey(DIK_D)) {

			// 左右加速
			Vector3 acceleration = {};

			if (Input::GetInstance()->PushKey(DIK_RIGHT) || Input::GetInstance()->PushKey(DIK_D)) {
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
			} else if (Input::GetInstance()->PushKey(DIK_LEFT) || Input::GetInstance()->PushKey(DIK_A)) {
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
			velocity_ = VectorMath::Add(velocity_, acceleration);

			// 最大速度制限
			velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed_, kLimitRunSpeed_);
		} else {

			// 非入力時は移動減衰
			velocity_.x *= (1.0f - kAttenuation_);
		}

		if (Input::GetInstance()->TriggerKey(DIK_UP) || Input::GetInstance()->TriggerKey(DIK_W)) {

			// ジャンプ初速
			velocity_.y += kJumpAcceleration;
			isDoubleJumped_ = false;
		}
	} else {

		const bool holdRight = Input::GetInstance()->PushKey(DIK_RIGHT) || Input::GetInstance()->PushKey(DIK_D);
		const bool holdLeft = Input::GetInstance()->PushKey(DIK_LEFT) || Input::GetInstance()->PushKey(DIK_A);

		if (holdRight && !holdLeft) {
			velocity_.x += kAirAcceleration_;
		} else if (holdLeft && !holdRight) {
			velocity_.x -= kAirAcceleration_;
		} else {
			velocity_.x *= (1.0f - kAirAttenuation_);
		}
		// 空中用の最大速度でクランプ
		velocity_.x = std::clamp(velocity_.x, -kLimitAirRunSpeed_, kLimitAirRunSpeed_);

		// 落下速度
		velocity_.y += -kGravityAcceleration;

		// 落下速度制限
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);

		if (!onGround_ && isTouchWall_ && velocity_.y < 0.0f) {
			velocity_.y = std::max(velocity_.y, -kLimitWallSlideSpeed_);
		}

		const bool trigJump = Input::GetInstance()->TriggerKey(DIK_UP) || Input::GetInstance()->TriggerKey(DIK_W);

		// 二段ジャンプ
		if (Input::GetInstance()->TriggerKey(DIK_UP) || Input::GetInstance()->TriggerKey(DIK_W)) {
			if (trigJump) {
				// 1) 壁ジャンプを先に判定
				if (isTouchWall_) {
					// 見ている向きから“反対方向”へ押し出す
					if (lrDirection_ == LRDirection::kRight) {
						velocity_.x = -kLimitRunSpeed_ * 0.8f; // 右向き＝右壁想定→左へ
					} else {
						velocity_.x = kLimitRunSpeed_ * 0.8f; // 左向き＝左壁想定→右へ
					}
					velocity_.y = kJumpAcceleration * 0.7f; // 縦の押し上げ
				}
				// 2) 壁に触れていないなら二段ジャンプ
				else if (!isDoubleJumped_) {
					velocity_.y = kJumpAcceleration;
					isDoubleJumped_ = true;
				}
			}
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

			// 二段ジャンプフラグリセット
			isDoubleJumped_ = false;

			isTouchWall_ = false;
		}
	}
}

void Player::HandleWallCollision(const CollisionMapInfo& info) {

	// 壁接触による減速
	if (info.isHitWall) {
		velocity_.x *= (1.0f - kAttenuationWall);
	}
	isTouchWall_ = info.isHitWall;
}

KamataEngine::Vector3 Player::GetWorldPosition() const {
	Vector3 worldPos;

	// ワールド行列の平行移動成分を取り出す（ワールド座標）
	worldPos.x = worldTransform_.matWorld_.m[3][0]; // X
	worldPos.y = worldTransform_.matWorld_.m[3][1]; // Y
	worldPos.z = worldTransform_.matWorld_.m[3][2]; // Z

	return worldPos;
}

AABB Player::GetAABB() { // 中心座標（ワールド座標）
	Vector3 worldPos = GetWorldPosition();

	AABB aabb;

	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};

	return aabb;
}

void Player::OnCollision(const Enemy* enemy) {
	(void)enemy;

	if (IsAttack()) {
		return;
	}

	// 敵に当たったらデスフラグを立てる
	isDead_ = true;
}

void Player::BehaviorRootInitialize() {}

void Player::BehaviorRootUpdate() {

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

	// 攻撃キーを押したら
	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		// 攻撃ビヘイビアをリクエスト
		behaviorRequest_ = Behavior::kAttack;
	}

	// アニメーション
	idleTime_ += (1.0f / 60.0f) * 5.0f;

	const float s = std::sin(idleTime_);

	worldTransform_.scale_.x = 1.0f - 0.06f * s;
	worldTransform_.scale_.y = 1.0f + 0.06f * s;
	worldTransform_.scale_.z = 1.0f - 0.06f * s;

	// 行列更新
	WorldTransformUpdate(worldTransform_);
}

void Player::BehaviorAttackInitialize() {
	attackParameter_ = 0;
	attackPhase_ = AttackPhase::Charge;
	velocity_ = {};
}

void Player::BehaviorAttackUpdate() {

	// 衝突情報を初期化
	CollisionMapInfo collisionMapInfo;

	// 移動量に速度の値をコピー
	collisionMapInfo.moveAmount = velocity_;

	attackParameter_++;

	switch (attackPhase_) {
		// 溜め
	case AttackPhase::Charge:
	default: {

		float t = static_cast<float>(attackParameter_) / chargeTimer_;
		worldTransform_.scale_.z = EaseOut(1.0f, 0.3f, t);
		worldTransform_.scale_.y = EaseOut(1.0f, 1.6f, t);

		// 前進方向へ移行
		if (attackParameter_ >= chargeTimer_) {
			attackPhase_ = AttackPhase::Dash;
			attackParameter_ = 0; // カウンターリセット
		}

		break;
	}

		// 突進
	case AttackPhase::Dash: {

		float t = static_cast<float>(attackParameter_) / dashTimer_;
		worldTransform_.scale_.z = EaseOut(0.3f, 1.3f, t);
		worldTransform_.scale_.y = EaseIn(1.6f, 0.7f, t);

		// 余韻動作へ移行
		if (attackParameter_ >= dashTimer_) {
			attackPhase_ = AttackPhase::Recover;
			attackParameter_ = 0; // カウンターリセット
		}

		break;
	}

		// 余韻
	case AttackPhase::Recover: {

		float t = static_cast<float>(attackParameter_) / recoverTimer_;
		worldTransform_.scale_.z = EaseOut(1.3f, 1.0f, t);
		worldTransform_.scale_.y = EaseIn(0.7f, 1.0f, t);

		// 攻撃動作終了
		if (attackParameter_ >= recoverTimer_) {
			attackPhase_ = AttackPhase::Charge;
			attackParameter_ = 0; // カウンターリセット
			behaviorRequest_ = Behavior::kRoot;
		}

		break;
	}
	}

	// 攻撃動作用の速度
	Vector3 velocity{};

	// 攻撃フェーズごとの更新処理
	switch (attackPhase_) {

	case AttackPhase::Dash:
		// if (worldTransform_.rotation_.y > std::numbers::pi_v<float> / 2.0f && worldTransform_.rotation_.y < std::numbers::pi_v<float> / 2.0f) {
		//	velocity = +attackVelocity_;
		// } else {
		//	velocity = -attackVelocity_;
		// }

		float score = std::cos(worldTransform_.rotation_.y - std::numbers::pi_v<float> * 0.5f);
		velocity = (score >= 0.0f) ? (+attackVelocity_) : (-attackVelocity_);
		break;

		break;
	}

	collisionMapInfo.moveAmount = velocity;

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

	worldTransformAttack_.translation_ = worldTransform_.translation_;

	if (lrDirection_ == LRDirection::kRight) {
		worldTransformAttack_.rotation_.y = -std::numbers::pi_v<float> / 2.0f;
	} else {
		worldTransformAttack_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
	}

	// worldTransformAttack_.rotation_.y += std::numbers::pi_v<float>;
	WorldTransformUpdate(worldTransformAttack_);
}

void Player::TryStartWire() {

	wireAnchor_ = {};
	wireLength_ = 0.0f;
	isWireActive_ = false;

	wireStartPos_ = GetWireMuzzlePos();
	wireDir_ = GetAimDirForWire();

	wireFlyLength_ = 0.0f;
	wireState_ = WireState::kFlying;
}

void Player::EndWire() {
	isWireActive_ = false;
	wireState_ = WireState::kNone;
	wireFlyLength_ = 0.0f;
}

void Player::UpdateWire() {

	if (wireState_ == WireState::kFlying) {

		const KamataEngine::Vector3 tipOld = VectorMath::Add(wireStartPos_, VectorMath::Multiply(wireFlyLength_, wireDir_));

		const float dt = 1.0f / 60.0f;
		wireFlyLength_ += kWireShootSpeed_ * dt;

		const KamataEngine::Vector3 tipNew = VectorMath::Add(wireStartPos_, VectorMath::Multiply(wireFlyLength_, wireDir_));

		if (!mapChipField_) {
			return;
		}

		KamataEngine::Vector3 seg = VectorMath::Subtract(tipNew, tipOld);
		float segLen = VectorMath::Length(seg);

		int steps = 1;
		if (segLen > 0.0001f) {
			steps = static_cast<int>(std::ceil(segLen / 0.20f));
			steps = std::clamp(steps, 1, 16);
		}

		const float t = std::clamp(wireFlyLength_ / kWireMaxDistance_, 0.0f, 1.0f);
		const float assist = 0.30f + 0.30f * t;

		MapChipField::IndexSet hitIdx{};
		bool hit = false;

		for (int i = 1; i <= steps; ++i) {
			const float a = static_cast<float>(i) / static_cast<float>(steps);
			KamataEngine::Vector3 p = VectorMath::Add(tipOld, VectorMath::Multiply(a, seg));

			auto idx = mapChipField_->GetMapChipIndexSetByPosition(p);
			if (idx.xIndex < mapChipField_->GetNumBlockHorizontal() && idx.yIndex < mapChipField_->GetNumBlockVirtical() &&
			    mapChipField_->GetMapChipTypeByIndex(idx.xIndex, idx.yIndex) == MapChipType::kBlock) {
				hitIdx = idx;
				hit = true;
				break;
			}

			if (HitBlockExpanded3x3(mapChipField_, p, assist, hitIdx)) {
				hit = true;
				break;
			}
		}

		if (!hit && wireFlyLength_ >= kWireMaxDistance_) {
			wireState_ = WireState::kNone;
			isWireActive_ = false;
			return;
		}

		if (!hit) {
			return;
		}

		KamataEngine::Vector3 center = mapChipField_->GetMatChipPositionByIndex(hitIdx.xIndex, hitIdx.yIndex);
		center.y += MapChipField::GetBlockHeight() * 0.5f;

		const float surfaceOffset = 0.45f;
		wireAnchor_ = VectorMath::Subtract(center, VectorMath::Multiply(surfaceOffset, wireDir_));

		KamataEngine::Vector3 d = VectorMath::Subtract(wireAnchor_, wireStartPos_);
		wireLength_ = VectorMath::Length(d);

		wireState_ = WireState::kAttached;
		isWireActive_ = true;
		return;
	}

	if (wireState_ != WireState::kAttached) {
		return;
	}

	Vector3 toPlayer = VectorMath::Subtract(worldTransform_.translation_, wireAnchor_);
	float dist = VectorMath::Length(toPlayer);
	if (dist < 0.0001f) {
		return;
	}

	Vector3 dir = VectorMath::Multiply(1.0f / dist, toPlayer);

	if (Input::GetInstance()->PushKey(DIK_W)) {
		wireLength_ = std::max(kWireMinLength_, wireLength_ - kWireReelSpeed_);
	}
	if (Input::GetInstance()->PushKey(DIK_S)) {
		wireLength_ = std::min(kWireMaxDistance_, wireLength_ + kWireReelSpeed_);
	}

	velocity_.y += -kGravityAcceleration * 0.6f;

	float stretch = dist - wireLength_;
	if (stretch > 0.0f) {

		float accel = stretch * kWirePullK_;
		const float kMaxPullAccel = 4.0f;
		accel = std::min(accel, kMaxPullAccel);

		velocity_ = VectorMath::Add(velocity_, VectorMath::Multiply(-accel, dir));

		float radialV = VectorMath::Dot(velocity_, dir);
		const float kRadialDamp = 0.15f;
		velocity_ = VectorMath::Subtract(velocity_, VectorMath::Multiply(radialV * kRadialDamp, dir));
	}

	{
		const float kMaxWireSpeed = 0.85f;
		float v = VectorMath::Length(velocity_);
		if (v > kMaxWireSpeed && v > 0.0001f) {
			velocity_ = VectorMath::Multiply(kMaxWireSpeed / v, velocity_);
		}
	}

	velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
}

KamataEngine::Vector3 Player::GetWireMuzzlePos() const {
	KamataEngine::Vector3 p = worldTransform_.translation_;
	p.y += 1.2f;
	p.x += (lrDirection_ == LRDirection::kRight) ? 0.6f : -0.6f;
	return p;
}

KamataEngine::Vector3 Player::GetAimDirForWire() const {

	auto* in = Input::GetInstance();

	const bool right = in->PushKey(DIK_RIGHT) || in->PushKey(DIK_D);
	const bool left = in->PushKey(DIK_LEFT) || in->PushKey(DIK_A);

	KamataEngine::Vector3 d{0.0f, 0.0f, 0.0f};

	// 3方向のみ：↖ ↑ ↗
	if (right && !left) {
		// ↗
		d = {1.0f, 1.0f, 0.0f};
	} else if (left && !right) {
		// ↖
		d = {-1.0f, 1.0f, 0.0f};
	} else {
		// ↑（左右なし or 両押し）
		d = {0.0f, 1.0f, 0.0f};
	}

	// 正規化
	const float len = std::sqrt(d.x * d.x + d.y * d.y);
	if (len > 0.0001f) {
		d.x /= len;
		d.y /= len;
	}

	return d;
}

void Player::ReleaseWire() {

	if (wireState_ != WireState::kAttached) {
		EndWire();
		return;
	}

	{
		const float kMaxWireSpeed = 0.85f;
		float v = VectorMath::Length(velocity_);
		if (v > kMaxWireSpeed && v > 0.0001f) {
			velocity_ = VectorMath::Multiply(kMaxWireSpeed / v, velocity_);
		}
	}

	Vector3 toPlayer = VectorMath::Subtract(worldTransform_.translation_, wireAnchor_);
	float dist = VectorMath::Length(toPlayer);

	if (dist > 0.0001f) {

		Vector3 radial = VectorMath::Multiply(1.0f / dist, toPlayer);

		float radialV = VectorMath::Dot(velocity_, radial);
		Vector3 tangential = VectorMath::Subtract(velocity_, VectorMath::Multiply(radialV, radial));

		const float kReleaseScale = 0.90f;
		tangential = VectorMath::Multiply(kReleaseScale, tangential);

		float tLen = VectorMath::Length(tangential);
		const float kMaxReleaseSpeed = 0.85f;
		if (tLen > kMaxReleaseSpeed && tLen > 0.0001f) {
			tangential = VectorMath::Multiply(kMaxReleaseSpeed / tLen, tangential);
		}

		velocity_ = tangential;
	}

	EndWire();
}

void Player::DetachToFlying() {

	// 今の発射口へ更新
	wireStartPos_ = GetWireMuzzlePos();

	KamataEngine::Vector3 d = velocity_;
	const float vlen = std::sqrt(d.x * d.x + d.y * d.y + d.z * d.z);

	if (vlen > 0.05f) {
		d.x /= vlen;
		d.y /= vlen;
		d.z /= vlen;
		d.z = 0.0f;

		// zを殺した後に再正規化
		const float len2 = std::sqrt(d.x * d.x + d.y * d.y);
		if (len2 > 0.0001f) {
			d.x /= len2;
			d.y /= len2;
		}

	} else {
		d = GetAimDirForWire();
	}

	wireDir_ = d;

	wireFlyLength_ = 0.0f;
	wireState_ = WireState::kFlying;
	isWireActive_ = false;
}
