#pragma once
#include <KamataEngine.h>

class Player;

class CameraController {

	// 矩形
	struct Rect {
		float left = 0.0f;
		float right = 1.0f;
		float bottom = 0.0f;
		float top = 1.0f;
	};

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// ターゲットのセット
	/// </summary>
	void SetTarget(Player* target) { target_ = target; }

	void SetTargetCamera(KamataEngine::Camera* camera) { targetCamera_ = camera; }

	void SetMovableArea(Rect area) { movableArea_ = area; }

	void Reset();

	/// <summary>
	/// 線形補完
	/// </summary>
	/// <param name="v1"></param>
	/// <param name="v2"></param>
	/// <param name="t"></param>
	/// <returns></returns>
	KamataEngine::Vector3 Lerp(const KamataEngine::Vector3& v1, const KamataEngine::Vector3& v2, float t) {

		KamataEngine::Vector3 result;

		result.x = t * v1.x + (1.0f - t) * v2.x;
		result.y = t * v1.y + (1.0f - t) * v2.y;
		result.z = t * v1.z + (1.0f - t) * v2.z;

		return result;
	}

private:
	// カメラ
	KamataEngine::Camera camera_;

	// 追従対象
	Player* target_;

	KamataEngine::Camera* targetCamera_ = nullptr;

	// 追従対象とカメラ座標の差(オフセット)
	KamataEngine::Vector3 targetOffset_{0.0f, 0.0f, -17.0f};

	// カメラの目標座標
	KamataEngine::Vector3 targetPosition_;

	KamataEngine::Vector3 targetVelocity_;

	// 座標補間割合
	static inline const float kInterpolationRate = 0.5f;

	// カメラ移動範囲
	Rect movableArea_ = {0.0f, 100.0f, 0.0f, 100.0f};

	// 速度掛け率
	static inline const float kVelocityBias = 10.0f;

	// 追従対象の各方向へのカメラ移動範囲
	static inline const Rect margin_ = {-1, 4, 0, 5};
};
