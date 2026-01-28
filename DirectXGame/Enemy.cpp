#include "Enemy.h"
#include "GameScene.h"
#include "Player.h"

void Enemy::Initialize(Model* model, Camera* camera, const Vector3& position) {
	model_ = model;
	camera_ = camera;
	worldTransform_.Initialize();

	// 初期座標
	worldTransform_.translation_ = position;

	// 自キャラと逆向き（例：左を向く）
	worldTransform_.rotation_.y = -std::numbers::pi_v<float> /*/ 2.0f*/;

	velocity_ = {-kWalkSpeed, 0, 0};

	walkTimer_ = 0.0f;
}

void Enemy::Update() {

	if (behaviorRequest_ != Behavior::kUnknown) {

		behavior_ = behaviorRequest_;

		switch (behavior_) {

		case Behavior::kWalk:
		default:

			BehaviorWalkInitialize();
			BehaviorWalkUpdate();

			break;

		case Behavior::kDead:

			BehaviorDeadInitialize();
			BehaviorDeadUpdate();

			break;
		}
		// ふるまいリクエストをリセット
		behaviorRequest_ = Behavior::kUnknown;
	}

	switch (behavior_) {
	case Behavior::kWalk:
	default:

		BehaviorWalkUpdate();

		break;

	case Behavior::kDead:

		BehaviorDeadUpdate();

		break;
	}
}

void Enemy::Draw() {

	// DirectXCommonインスタンスの生成
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	Model::PreDraw(dxCommon->GetCommandList());

	model_->Draw(worldTransform_, *camera_);

	Model::PostDraw();
}

void Enemy::OnCollision(const Player* player) {
	(void)player;

	if (behavior_ == Behavior::kDead) {
		// 敵がやられているなら何もしない
		return;
	}

	if (player->IsAttack()) {
		// 敵のふるまいをデス演出に変更
		behaviorRequest_ = Behavior::kDead;

		// 敵と自キャラの中間位置にエフェクトを生成
		Vector3 effectPos = (worldTransform_.translation_ + player->GetWorldPosition()) / 2.0f;
		gameScene_->CreateHitEffect(effectPos);

		isCollisionDisabled_ = true;
	}
}

AABB Enemy::GetAABB() {
	Vector3 worldPos = worldTransform_.translation_;

	AABB aabb;

	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};

	return aabb;
}

void Enemy::BehaviorWalkInitialize() {}

void Enemy::BehaviorWalkUpdate() {

	int steps = std::max(1, (int)std::ceil(std::abs(velocity_.x) / kMaxStep));
	float dx = velocity_.x / (float)steps;

	constexpr float kYawRight = -std::numbers::pi_v<float> * 2.0f; // 右向き
	constexpr float kYawLeft = std::numbers::pi_v<float>;

	for (int i = 0; i < steps; ++i) {
		if (dx != 0.0f) {
			float dir = (dx > 0.0f) ? 1.0f : -1.0f;
			Vector3 next = worldTransform_.translation_;
			next.x += dx;

			// 進行方向側の2点（胸/足）で壁ヒットを検出
			Vector3 p1 = {next.x + dir * (kWidth * 0.5f + kEPS), worldTransform_.translation_.y + kHeight * 0.25f, next.z};
			Vector3 p2 = {next.x + dir * (kWidth * 0.5f + kEPS), worldTransform_.translation_.y - kHeight * 0.25f, next.z};

			if (map_ && (IsSolidAt(p1) || IsSolidAt(p2))) {
				auto r = TileRectAt((IsSolidAt(p1) ? p1 : p2));
				float boundary = (dir > 0.0f) ? r.left : r.right;
				worldTransform_.translation_.x = boundary - dir * (kWidth * 0.5f + kEPS);

				velocity_.x *= -1.0f;
				worldTransform_.rotation_.y = (velocity_.x > 0.0f) ? kYawRight : kYawLeft;
				dx = 0.0f;
			} else {
				worldTransform_.translation_.x = next.x;
			}
		}
	}

	// タイマー加算（1フレームに1/60秒ずつ）
	walkTimer_ += 1.0f / 60.0f;

	// 回転アニメーション
	float param = std::sin((2.0f * std::numbers::pi_v<float>)*walkTimer_ / kWalkMotionTime);

	float degree = kWalkMotionAngleStart + (kWalkMotionAngleEnd - kWalkMotionAngleStart) * (param + 1.0f) / 2.0f;
	worldTransform_.rotation_.x = degree * (3.14159265f / 180.0f); // 度をラジアンに変換

	// 行列更新
	WorldTransformUpdate(worldTransform_);
}

void Enemy::BehaviorDeadInitialize() {}

void Enemy::BehaviorDeadUpdate() {

	// タイマー加算（1フレームに1/60秒ずつ）
	deadTimer_ += 1.0f / 60.0f;

	// 死亡時アニメーション
	float t = std::clamp(deadTimer_ / 1.0f, 0.0f, 1.0f);
	float e = 1.0f - (1.0f - t) * (1.0f - t) * (1.0f - t);

	// Y軸
	worldTransform_.rotation_.y += (2.0f * std::numbers::pi_v<float>)*6.0f * (1.0f / 60.0f) * (1.0f - 0.8f * e);

	// X軸
	worldTransform_.rotation_.x = (-110.0f * std::numbers::pi_v<float> / 180.0f) * e;

	// スケーリング
	float s = 1.0f - e;
	worldTransform_.scale_ = {s, s, s};

	if (deadTimer_ >= 60.0f) {
		isDead_ = true;
	}

	// 行列更新
	WorldTransformUpdate(worldTransform_);
}

bool Enemy::IsSolidAt(const Vector3& p) const {
	auto idx = map_->GetMapChipIndexSetByPosition(p);
	auto type = map_->GetMapChipTypeByIndex(idx.xIndex, idx.yIndex);
	return type == MapChipType::kBlock;
}

MapChipField::Rect Enemy::TileRectAt(const Vector3& p) const {
	auto idx = map_->GetMapChipIndexSetByPosition(p);
	return map_->GetRectByIndex(idx.xIndex, idx.yIndex);
}