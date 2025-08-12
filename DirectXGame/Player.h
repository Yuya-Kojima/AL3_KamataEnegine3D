#pragma once
#include "AABB.h"
#include "Easing.h"
#include "KamataEngine.h"
#include "WorldMatrixTransform.h"
#include <numbers>

class MapChipField;

class Enemy;

class Player {

	struct CollisionMapInfo {
		bool isHitCeiling = false;        // 天井衝突フラグ
		bool isGrounded = false;          // 着地フラグ
		bool isHitWall = false;           // 壁接触フラグ
		KamataEngine::Vector3 moveAmount; // 移動量
	};

	enum class LRDirection {
		kRight,
		kLeft,
	};

	enum Corner {
		kRightBottom, // 右下
		kLeftBottom,  // 左下
		kRightTop,    // 右上
		kLeftTop,     // 左上

		kNumCorner, // 要素数
	};

public:
	void Initialize(KamataEngine::Model* model, KamataEngine::Vector3& position);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="camera"></param>
	void Draw(KamataEngine::Camera& camera);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Player();

	KamataEngine::WorldTransform& GetWorldTransform() { return worldTransform_; }

	const KamataEngine::Vector3& GetVelocity() const { return velocity_; }

	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	/// <summary>
	/// 移動
	/// </summary>
	void Move();

	/// <summary>
	/// マップ衝突判定
	/// </summary>
	/// <param name="info"></param>
	void CheckMapCollision(CollisionMapInfo& info);

	/// <summary>
	/// マップ衝突判定　上方向
	/// </summary>
	/// <param name="info"></param>
	void CheckMapCollisionUp(CollisionMapInfo& info);

	/// <summary>
	/// マップ衝突判定　下方向
	/// </summary>
	/// <param name="info"></param>
	void CheckMapCollisionDown(CollisionMapInfo& info);

	/// <summary>
	/// マップ衝突判定　右方向
	/// </summary>
	/// <param name="info"></param>
	void CheckMapCollisionRight(CollisionMapInfo& info);

	/// <summary>
	/// マップ衝突判定　左方向
	/// </summary>
	/// <param name="info"></param>
	void CheckMapCollisionLeft(CollisionMapInfo& info);

	/// <summary>
	/// 指定した角の座標計算
	/// </summary>
	/// <param name="center"></param>
	/// <param name="corner"></param>
	/// <returns></returns>
	KamataEngine::Vector3 CornerPosition(const KamataEngine::Vector3& center, Corner corner);

	/// <summary>
	/// 判定結果を反映して移動させる
	/// </summary>
	/// <param name="info"></param>
	void ApplyCollisionResult(const CollisionMapInfo& info);

	/// <summary>
	/// 天井に接触している場合の処理
	/// </summary>
	/// <param name="info"></param>
	void HandleCeilingCollision(const CollisionMapInfo& info);

	/// <summary>
	/// 接地状態の切り替え処理
	/// </summary>
	/// <param name="info"></param>
	void UpdateGroundState(const CollisionMapInfo& info);

	/// <summary>
	/// 壁に接触している場合の処理
	/// </summary>
	/// <param name="info"></param>
	void HandleWallCollision(const CollisionMapInfo& info);

	/// <summary>
	/// ワールド座標を取得
	/// </summary>
	/// <returns></returns>
	KamataEngine::Vector3 GetWorldPosition();

	/// <summary>
	/// AABBを取得
	/// </summary>
	/// <returns></returns>
	AABB GetAABB();

	// 衝突応答
	void OnCollision(const Enemy* enemy);

	// デスフラグのgetter
	bool IsDead() const { return isDead_; }

	// 通常行動初期化
	void BehaviorRootInitialize();

	// 通常行動更新
	void BehaviorRootUpdate();

	// 攻撃行動初期化
	void BehaviorAttackInitialize();

	// 攻撃行動更新
	void BehaviorAttackUpdate();

private:
	enum class Behavior {
		kRoot,    // 通常状態
		kAttack,  // 攻撃中
		kUnknown, // 変更リクエストがない状態
	};

	// 攻撃フェーズ
	enum class AttackPhase {
		Charge,  // 溜め
		Dash,    // 突進
		Recover, // 余韻
	};

	AttackPhase attackPhase_;

	// 3Dモデル
	KamataEngine::Model* model_ = nullptr;

	// カメラ
	KamataEngine::Camera camera_;

	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;

	// 速度
	KamataEngine::Vector3 velocity_ = {};

	// プレイヤーの向き
	LRDirection lrDirection_ = LRDirection::kRight;

	// 旋回開始時の角度
	float turnFirstRotationY_ = 0.0f;

	// 旋回タイマー
	float turnTimer_ = 0.0f;

	// 接地状態フラグ
	bool onGround_ = true;

	// 旋回時間
	static inline const float kTimeTurn = 0.3f;

	static inline const float kAcceleration_ = 0.01f;

	static inline const float kAttenuation_ = 0.1f;

	static inline const float kLimitRunSpeed_ = 0.2f;

	// 重力加速度(下方向)
	static inline const float kGravityAcceleration = 0.05f;

	// 最大落下速度
	static inline const float kLimitFallSpeed = 0.3f;

	// ジャンプ初速(上方向)
	static inline const float kJumpAcceleration = 0.65f;

	// マップチップによるフィールド
	MapChipField* mapChipField_ = nullptr;

	// キャラクターの当たり判定サイズ
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;

	// 着地時の速度減衰率
	static inline const float kAttenuationLanding = 0.1f;

	// 地面との吸着判定時の微小オフセット
	static inline const float kGroundAdhesionOffset = 0.01f;

	static inline const float kAttenuationWall = 0.3f;

	bool isDead_ = false; // デスフラグ

	// ふるまい
	Behavior behavior_ = Behavior::kRoot;

	// 次のふるまいリクエスト
	Behavior behaviorRequest_ = Behavior::kUnknown;

	// 攻撃ギミックの経過時間カウンター
	uint32_t attackParameter_ = 0;

	// 溜め動作時間
	uint32_t chargeTimer_ = 10;

	// 突進時間
	uint32_t dashTimer_ = 5;

	// 余韻時間
	uint32_t recoverTimer_ = 5;

	// 攻撃中速度
	KamataEngine::Vector3 attackVelocity_ = {1.0f, 0.0f, 0.0f};
};
