#pragma once
#include "KamataEngine.h"

using namespace KamataEngine;

// <summary>
/// ベクトルの足し算
/// </summary>
/// <param name="v1"></param>
/// <param name="v2"></param>
/// <returns></returns>
Vector3 Add(const Vector3& v1, const Vector3& v2) {

	Vector3 result;

	result.x = v1.x + v2.x;
	result.y = v1.y + v2.y;
	result.z = v1.z + v2.z;

	return result;
}

/// <summary>
/// ベクトルの引き算
/// </summary>
/// <param name="v1"></param>
/// <param name="v2"></param>
/// <returns></returns>
Vector3 Subtract(const Vector3& v1, const Vector3& v2) {

	Vector3 result;

	result.x = v1.x - v2.x;
	result.y = v1.y - v2.y;
	result.z = v1.z - v2.z;

	return result;
}

/// <summary>
/// Vector3とfloatの積を求める
/// </summary>
/// <param name="f"></param>
/// <param name="vector"></param>
/// <returns></returns>
Vector3 Multiply(const float& f, const Vector3 vector) {

	Vector3 result;

	result = {vector.x * f, vector.y * f, vector.z * f};

	return result;
}

//===========================
// 単項演算子
//===========================
Vector3 operator-(const Vector3& v) { return {-v.x, -v.y, -v.z}; }
Vector3 operator+(const Vector3& v) { return v; }

//===========================
// 二項演算子
//===========================
Vector3 operator+(const Vector3& v1, const Vector3& v2) { return Add(v1, v2); }
Vector3 operator-(const Vector3& v1, const Vector3& v2) { return Subtract(v1, v2); }
Vector3 operator*(float s, const Vector3& v) { return Multiply(s, v); }
Vector3 operator*(const Vector3& v, float s) { return s * v; }
Vector3 operator/(const Vector3& v, float s) { return Multiply(1.0f / s, v); }
