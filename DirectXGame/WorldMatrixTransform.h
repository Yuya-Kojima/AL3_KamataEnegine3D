#pragma once
#include "KamataEngine.h"

/// <summary>
/// ワールド行列更新
/// </summary>
/// <param name="worldTransform"></param>
void WorldTransformUpdate(KamataEngine::WorldTransform& worldTransform);

/// <summary>
/// ベクトルの足し算
/// </summary>
/// <param name="v1"></param>
/// <param name="v2"></param>
/// <returns></returns>
KamataEngine::Vector3 Add(const KamataEngine::Vector3& v1, const KamataEngine::Vector3& v2);

/// <summary>
/// 行列の積を求める
/// </summary>
/// <param name="matrix1">行列</param>
/// <param name="matrix2">行列</param>
/// <returns>積</returns>
KamataEngine::Matrix4x4 Multiply(KamataEngine::Matrix4x4 matrix1, KamataEngine::Matrix4x4 matrix2);

// 平行移動行列
KamataEngine::Matrix4x4 MakeTranslateMatrix(const KamataEngine::Vector3& translate);

// 拡縮行列
KamataEngine::Matrix4x4 MakeScaleMatrix(const KamataEngine::Vector3& scale);

/// <summary>
/// X軸回転行列作成
/// </summary>
/// <param name="rotateX"></param>
/// <returns></returns>
KamataEngine::Matrix4x4 MakeRotateXMatrix(const float& rotateX);

/// <summary>
/// Y軸回転行列作成
/// </summary>
/// <param name="rotateY"></param>
/// <returns></returns>
KamataEngine::Matrix4x4 MakeRotateYMatrix(const float& rotateY);

/// <summary>
/// Z軸回転行列作成
/// </summary>
/// <param name="rotateZ"></param>
/// <returns></returns>
KamataEngine::Matrix4x4 MakeRotateZMatrix(const float& rotateZ);

/// <summary>
/// 回転行列生成
/// </summary>
/// <param name="rotate"></param>
/// <returns></returns>
KamataEngine::Matrix4x4 MakeRotateMatrix(const KamataEngine::Vector3& rotate);

/// <summary>
/// アフィン行列作成
/// </summary>
/// <param name="scale">拡縮</param>
/// <param name="rotate">回転</param>
/// <param name="translate">平行移動</param>
/// <returns>アフィン行列</returns>
KamataEngine::Matrix4x4 MakeAffineMatrix(KamataEngine::Vector3 scale, KamataEngine::Vector3 rotate, KamataEngine::Vector3 translate);

class WorldMatrixTransform {};
