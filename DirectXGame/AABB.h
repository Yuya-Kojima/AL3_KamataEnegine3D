#pragma once
#include "KamataEngine.h"

struct AABB {
	KamataEngine::Vector3 min; // 最小点
	KamataEngine::Vector3 max; // 最大点
};

/// <summary>
/// AABBの当たり判定
/// </summary>
/// <param name="a"></param>
/// <param name="b"></param>
/// <returns></returns>
bool IsCollision(const AABB& a, const AABB& b);