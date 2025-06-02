#pragma once
#include "KamataEngine.h"

/// <summary>
/// 単位行列作成
/// </summary>
/// <returns>単位行列</returns>
KamataEngine::Matrix4x4 MakeIdentity4x4() {
	KamataEngine::Matrix4x4 matrix;

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (i == j) {
				matrix.m[i][j] = 1.0f;
			} else {
				matrix.m[i][j] = 0.0f;
			}
		}
	}

	return matrix;
}

/// <summary>
/// 行列の積を求める
/// </summary>
/// <param name="matrix1">行列</param>
/// <param name="matrix2">行列</param>
/// <returns>積</returns>
KamataEngine::Matrix4x4 Multiply(KamataEngine::Matrix4x4 matrix1, KamataEngine::Matrix4x4 matrix2) {

	KamataEngine::Matrix4x4 result;

	result.m[0][0] = matrix1.m[0][0] * matrix2.m[0][0] + matrix1.m[0][1] * matrix2.m[1][0] + matrix1.m[0][2] * matrix2.m[2][0] + matrix1.m[0][3] * matrix2.m[3][0];

	result.m[0][1] = matrix1.m[0][0] * matrix2.m[0][1] + matrix1.m[0][1] * matrix2.m[1][1] + matrix1.m[0][2] * matrix2.m[2][1] + matrix1.m[0][3] * matrix2.m[3][1];

	result.m[0][2] = matrix1.m[0][0] * matrix2.m[0][2] + matrix1.m[0][1] * matrix2.m[1][2] + matrix1.m[0][2] * matrix2.m[2][2] + matrix1.m[0][3] * matrix2.m[3][2];

	result.m[0][3] = matrix1.m[0][0] * matrix2.m[0][3] + matrix1.m[0][1] * matrix2.m[1][3] + matrix1.m[0][2] * matrix2.m[2][3] + matrix1.m[0][3] * matrix2.m[3][3];

	result.m[1][0] = matrix1.m[1][0] * matrix2.m[0][0] + matrix1.m[1][1] * matrix2.m[1][0] + matrix1.m[1][2] * matrix2.m[2][0] + matrix1.m[1][3] * matrix2.m[3][0];

	result.m[1][1] = matrix1.m[1][0] * matrix2.m[0][1] + matrix1.m[1][1] * matrix2.m[1][1] + matrix1.m[1][2] * matrix2.m[2][1] + matrix1.m[1][3] * matrix2.m[3][1];

	result.m[1][2] = matrix1.m[1][0] * matrix2.m[0][2] + matrix1.m[1][1] * matrix2.m[1][2] + matrix1.m[1][2] * matrix2.m[2][2] + matrix1.m[1][3] * matrix2.m[3][2];

	result.m[1][3] = matrix1.m[1][0] * matrix2.m[0][3] + matrix1.m[1][1] * matrix2.m[1][3] + matrix1.m[1][2] * matrix2.m[2][3] + matrix1.m[1][3] * matrix2.m[3][3];

	result.m[2][0] = matrix1.m[2][0] * matrix2.m[0][0] + matrix1.m[2][1] * matrix2.m[1][0] + matrix1.m[2][2] * matrix2.m[2][0] + matrix1.m[2][3] * matrix2.m[3][0];

	result.m[2][1] = matrix1.m[2][0] * matrix2.m[0][1] + matrix1.m[2][1] * matrix2.m[1][1] + matrix1.m[2][2] * matrix2.m[2][1] + matrix1.m[2][3] * matrix2.m[3][1];

	result.m[2][2] = matrix1.m[2][0] * matrix2.m[0][2] + matrix1.m[2][1] * matrix2.m[1][2] + matrix1.m[2][2] * matrix2.m[2][2] + matrix1.m[2][3] * matrix2.m[3][2];

	result.m[2][3] = matrix1.m[2][0] * matrix2.m[0][3] + matrix1.m[2][1] * matrix2.m[1][3] + matrix1.m[2][2] * matrix2.m[2][3] + matrix1.m[2][3] * matrix2.m[3][3];

	result.m[3][0] = matrix1.m[3][0] * matrix2.m[0][0] + matrix1.m[3][1] * matrix2.m[1][0] + matrix1.m[3][2] * matrix2.m[2][0] + matrix1.m[3][3] * matrix2.m[3][0];

	result.m[3][1] = matrix1.m[3][0] * matrix2.m[0][1] + matrix1.m[3][1] * matrix2.m[1][1] + matrix1.m[3][2] * matrix2.m[2][1] + matrix1.m[3][3] * matrix2.m[3][1];

	result.m[3][2] = matrix1.m[3][0] * matrix2.m[0][2] + matrix1.m[3][1] * matrix2.m[1][2] + matrix1.m[3][2] * matrix2.m[2][2] + matrix1.m[3][3] * matrix2.m[3][2];

	result.m[3][3] = matrix1.m[3][0] * matrix2.m[0][3] + matrix1.m[3][1] * matrix2.m[1][3] + matrix1.m[3][2] * matrix2.m[2][3] + matrix1.m[3][3] * matrix2.m[3][3];

	return result;
}

// 平行移動行列
KamataEngine::Matrix4x4 MakeTranslateMatrix(const KamataEngine::Vector3& translate) {

	KamataEngine::Matrix4x4 result;

	result.m[0][0] = 1.0f;
	result.m[0][1] = 0.0f;
	result.m[0][2] = 0.0f;
	result.m[0][3] = 0.0f;
	result.m[1][0] = 0.0f;
	result.m[1][1] = 1.0f;
	result.m[1][2] = 0.0f;
	result.m[1][3] = 0.0f;
	result.m[2][0] = 0.0f;
	result.m[2][1] = 0.0f;
	result.m[2][2] = 1.0f;
	result.m[2][3] = 0.0f;
	result.m[3][0] = translate.x;
	result.m[3][1] = translate.y;
	result.m[3][2] = translate.z;
	result.m[3][3] = 1.0f;

	return result;
}

// 拡縮行列
KamataEngine::Matrix4x4 MakeScaleMatrix(const KamataEngine::Vector3& scale) {

	KamataEngine::Matrix4x4 result;

	result.m[0][0] = scale.x;
	result.m[0][1] = 0.0f;
	result.m[0][2] = 0.0f;
	result.m[0][3] = 0.0f;
	result.m[1][0] = 0.0f;
	result.m[1][1] = scale.y;
	result.m[1][2] = 0.0f;
	result.m[1][3] = 0.0f;
	result.m[2][0] = 0.0f;
	result.m[2][1] = 0.0f;
	result.m[2][2] = scale.z;
	result.m[2][3] = 0.0f;
	result.m[3][0] = 0.0f;
	result.m[3][1] = 0.0f;
	result.m[3][2] = 0.0f;
	result.m[3][3] = 1.0f;

	return result;
}

/// <summary>
/// X軸回転行列作成
/// </summary>
/// <param name="rotateX"></param>
/// <returns></returns>
KamataEngine::Matrix4x4 MakeRotateXMatrix(const float& rotateX) {

	KamataEngine::Matrix4x4 result;
	result.m[0][0] = 1.0f;
	result.m[0][1] = 0.0f;
	result.m[0][2] = 0.0f;
	result.m[0][3] = 0.0f;
	result.m[1][0] = 0.0f;
	result.m[1][1] = cosf(rotateX);
	result.m[1][2] = sinf(rotateX);
	result.m[1][3] = 0.0f;
	result.m[2][0] = 0.0f;
	result.m[2][1] = -sinf(rotateX);
	result.m[2][2] = cosf(rotateX);
	result.m[2][3] = 0.0f;
	result.m[3][0] = 0.0f;
	result.m[3][1] = 0.0f;
	result.m[3][2] = 0.0f;
	result.m[3][3] = 1.0f;

	return result;
}

/// <summary>
/// Y軸回転行列作成
/// </summary>
/// <param name="rotateY"></param>
/// <returns></returns>
KamataEngine::Matrix4x4 MakeRotateYMatrix(const float& rotateY) {

	KamataEngine::Matrix4x4 result;
	result.m[0][0] = cosf(rotateY);
	result.m[0][1] = 0.0f;
	result.m[0][2] = -sinf(rotateY);
	result.m[0][3] = 0.0f;
	result.m[1][0] = 0.0f;
	result.m[1][1] = 1.0f;
	result.m[1][2] = 0.0f;
	result.m[1][3] = 0.0f;
	result.m[2][0] = sinf(rotateY);
	result.m[2][1] = 0.0f;
	result.m[2][2] = cosf(rotateY);
	result.m[2][3] = 0.0f;
	result.m[3][0] = 0.0f;
	result.m[3][1] = 0.0f;
	result.m[3][2] = 0.0f;
	result.m[3][3] = 1.0f;

	return result;
}

/// <summary>
/// Z軸回転行列作成
/// </summary>
/// <param name="rotateZ"></param>
/// <returns></returns>
KamataEngine::Matrix4x4 MakeRotateZMatrix(const float& rotateZ) {

	KamataEngine::Matrix4x4 result;
	result.m[0][0] = cosf(rotateZ);
	result.m[0][1] = sinf(rotateZ);
	result.m[0][2] = 0.0f;
	result.m[0][3] = 0.0f;
	result.m[1][0] = -sinf(rotateZ);
	result.m[1][1] = cosf(rotateZ);
	result.m[1][2] = 0.0f;
	result.m[1][3] = 0.0f;
	result.m[2][0] = 0.0f;
	result.m[2][1] = 0.0f;
	result.m[2][2] = 1.0f;
	result.m[2][3] = 0.0f;
	result.m[3][0] = 0.0f;
	result.m[3][1] = 0.0f;
	result.m[3][2] = 0.0f;
	result.m[3][3] = 1.0f;

	return result;
}

/// <summary>
/// 回転行列生成
/// </summary>
/// <param name="rotate"></param>
/// <returns></returns>
KamataEngine::Matrix4x4 MakeRotateMatrix(const KamataEngine::Vector3& rotate) {

	KamataEngine::Matrix4x4 rotateX = MakeRotateXMatrix(rotate.x);

	KamataEngine::Matrix4x4 rotateY = MakeRotateYMatrix(rotate.y);

	KamataEngine::Matrix4x4 rotateZ = MakeRotateZMatrix(rotate.z);

	KamataEngine::Matrix4x4 result = Multiply(rotateX, Multiply(rotateY, rotateZ));

	return result;
}

/// <summary>
/// アフィン行列作成
/// </summary>
/// <param name="scale">拡縮</param>
/// <param name="rotate">回転</param>
/// <param name="translate">平行移動</param>
/// <returns>アフィン行列</returns>
KamataEngine::Matrix4x4 MakeAffineMatrix(KamataEngine::Vector3 scale, KamataEngine::Vector3 rotate, KamataEngine::Vector3 translate) {
	KamataEngine::Matrix4x4 scaleMatrix = MakeScaleMatrix(scale);

	KamataEngine::Matrix4x4 rotateMatrix = MakeRotateMatrix(rotate);

	KamataEngine::Matrix4x4 translateMatrix = MakeTranslateMatrix(translate);

	KamataEngine::Matrix4x4 worldMatrix;
	worldMatrix = Multiply(Multiply(scaleMatrix, rotateMatrix), translateMatrix);

	return worldMatrix;
}

/// <summary>
/// 透視投影行列作成
/// </summary>
/// <param name="fovY">画角</param>
/// <param name="aspectRatio">アスペクト比</param>
/// <param name="nearClip">近平面</param>
/// <param name="farClip">遠平面</param>
/// <returns>透視投影行列</returns>
KamataEngine::Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {
	KamataEngine::Matrix4x4 matrix;
	float f = 1.0f / tanf(fovY / 2.0f);

	matrix.m[0][0] = f / aspectRatio;
	matrix.m[0][1] = 0.0f;
	matrix.m[0][2] = 0.0f;
	matrix.m[0][3] = 0.0f;
	matrix.m[1][0] = 0.0f;
	matrix.m[1][1] = f;
	matrix.m[1][2] = 0.0f;
	matrix.m[1][3] = 0.0f;
	matrix.m[2][0] = 0.0f;
	matrix.m[2][1] = 0.0f;
	matrix.m[2][2] = farClip / (farClip - nearClip);
	matrix.m[2][3] = 1.0f;
	matrix.m[3][0] = 0.0f;
	matrix.m[3][1] = 0.0f;
	matrix.m[3][2] = -nearClip * farClip / (farClip - nearClip);
	matrix.m[3][3] = 0.0f;

	return matrix;
}

/// <summary>
/// 正射影行列(平行投影行列)作成
/// </summary>
/// <param name="left">左端</param>
/// <param name="top">上端</param>
/// <param name="right">右端</param>
/// <param name="bottom">下端</param>
/// <param name="nearClip">近平面</param>
/// <param name="farClip">遠平面</param>
/// <returns>正射影行列</returns>
KamataEngine::Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip) {
	KamataEngine::Matrix4x4 matrix;

	matrix.m[0][0] = 2.0f / (right - left);
	matrix.m[0][1] = 0.0f;
	matrix.m[0][2] = 0.0f;
	matrix.m[0][3] = 0.0f;
	matrix.m[1][0] = 0.0f;
	matrix.m[1][1] = 2.0f / (top - bottom);
	matrix.m[1][2] = 0.0f;
	matrix.m[1][3] = 0.0f;
	matrix.m[2][0] = 0.0f;
	matrix.m[2][1] = 0.0f;
	matrix.m[2][2] = 1.0f / (farClip - nearClip);
	matrix.m[2][3] = 0.0f;
	matrix.m[3][0] = (left + right) / (left - right);
	matrix.m[3][1] = (top + bottom) / (bottom - top);
	matrix.m[3][2] = nearClip / (nearClip - farClip);
	matrix.m[3][3] = 1.0f;

	return matrix;
}

/// <summary>
/// 4x4逆行列生成
/// </summary>
/// <param name="matrix">4x4正則行列</param>
/// <returns>逆行列</returns>
KamataEngine::Matrix4x4 Inverse(KamataEngine::Matrix4x4 matrix) {
	KamataEngine::Matrix4x4 inverseMatrix;

	float det = matrix.m[0][0] * matrix.m[1][1] * matrix.m[2][2] * matrix.m[3][3] + matrix.m[0][0] * matrix.m[1][2] * matrix.m[2][3] * matrix.m[3][1] +
	            matrix.m[0][0] * matrix.m[1][3] * matrix.m[2][1] * matrix.m[3][2] - matrix.m[0][0] * matrix.m[1][3] * matrix.m[2][2] * matrix.m[3][1] -
	            matrix.m[0][0] * matrix.m[1][2] * matrix.m[2][1] * matrix.m[3][3] - matrix.m[0][0] * matrix.m[1][1] * matrix.m[2][3] * matrix.m[3][2] -
	            matrix.m[0][1] * matrix.m[1][0] * matrix.m[2][2] * matrix.m[3][3] - matrix.m[0][2] * matrix.m[1][0] * matrix.m[2][3] * matrix.m[3][1] -
	            matrix.m[0][3] * matrix.m[1][0] * matrix.m[2][1] * matrix.m[3][2] + matrix.m[0][3] * matrix.m[1][0] * matrix.m[2][2] * matrix.m[3][1] +
	            matrix.m[0][2] * matrix.m[1][0] * matrix.m[2][1] * matrix.m[3][3] + matrix.m[0][1] * matrix.m[1][0] * matrix.m[2][3] * matrix.m[3][2] +
	            matrix.m[0][1] * matrix.m[1][2] * matrix.m[2][0] * matrix.m[3][3] + matrix.m[0][2] * matrix.m[1][3] * matrix.m[2][0] * matrix.m[3][1] +
	            matrix.m[0][3] * matrix.m[1][1] * matrix.m[2][0] * matrix.m[3][2] - matrix.m[0][3] * matrix.m[1][2] * matrix.m[2][0] * matrix.m[3][1] -
	            matrix.m[0][2] * matrix.m[1][1] * matrix.m[2][0] * matrix.m[3][3] - matrix.m[0][1] * matrix.m[1][3] * matrix.m[2][0] * matrix.m[3][2] -
	            matrix.m[0][1] * matrix.m[1][2] * matrix.m[2][3] * matrix.m[3][0] - matrix.m[0][2] * matrix.m[1][3] * matrix.m[2][1] * matrix.m[3][0] -
	            matrix.m[0][3] * matrix.m[1][1] * matrix.m[2][2] * matrix.m[3][0] + matrix.m[0][3] * matrix.m[1][2] * matrix.m[2][1] * matrix.m[3][0] +
	            matrix.m[0][2] * matrix.m[1][1] * matrix.m[2][3] * matrix.m[3][0] + matrix.m[0][1] * matrix.m[1][3] * matrix.m[2][2] * matrix.m[3][0];

	float invDet = 1 / det;

	// 逆行列を求めるための補助行列を計算
	inverseMatrix.m[0][0] = (matrix.m[1][1] * matrix.m[2][2] * matrix.m[3][3] - matrix.m[1][1] * matrix.m[2][3] * matrix.m[3][2] - matrix.m[1][2] * matrix.m[2][1] * matrix.m[3][3] +
	                         matrix.m[1][2] * matrix.m[2][3] * matrix.m[3][1] + matrix.m[1][3] * matrix.m[2][1] * matrix.m[3][2] - matrix.m[1][3] * matrix.m[2][2] * matrix.m[3][1]) *
	                        invDet;

	inverseMatrix.m[0][1] = (-(matrix.m[0][1] * matrix.m[2][2] * matrix.m[3][3]) + matrix.m[0][1] * matrix.m[2][3] * matrix.m[3][2] + matrix.m[0][2] * matrix.m[2][1] * matrix.m[3][3] -
	                         matrix.m[0][2] * matrix.m[2][3] * matrix.m[3][1] - matrix.m[0][3] * matrix.m[2][1] * matrix.m[3][2] + matrix.m[0][3] * matrix.m[2][2] * matrix.m[3][1]) *
	                        invDet;

	inverseMatrix.m[0][2] = (matrix.m[0][1] * matrix.m[1][2] * matrix.m[3][3] - matrix.m[0][1] * matrix.m[1][3] * matrix.m[3][2] - matrix.m[0][2] * matrix.m[1][1] * matrix.m[3][3] +
	                         matrix.m[0][2] * matrix.m[1][3] * matrix.m[3][1] + matrix.m[0][3] * matrix.m[1][1] * matrix.m[3][2] - matrix.m[0][3] * matrix.m[1][2] * matrix.m[3][1]) *
	                        invDet;

	inverseMatrix.m[0][3] = (-(matrix.m[0][1] * matrix.m[1][2] * matrix.m[2][3]) + matrix.m[0][1] * matrix.m[1][3] * matrix.m[2][2] + matrix.m[0][2] * matrix.m[1][1] * matrix.m[2][3] -
	                         matrix.m[0][2] * matrix.m[1][3] * matrix.m[2][1] - matrix.m[0][3] * matrix.m[1][1] * matrix.m[2][2] + matrix.m[0][3] * matrix.m[1][2] * matrix.m[2][1]) *
	                        invDet;
	inverseMatrix.m[1][0] = (-(matrix.m[1][0] * matrix.m[2][2] * matrix.m[3][3]) + matrix.m[1][0] * matrix.m[2][3] * matrix.m[3][2] + matrix.m[1][2] * matrix.m[2][0] * matrix.m[3][3] -
	                         matrix.m[1][2] * matrix.m[2][3] * matrix.m[3][0] - matrix.m[1][3] * matrix.m[2][0] * matrix.m[3][2] + matrix.m[1][3] * matrix.m[2][2] * matrix.m[3][0]) *
	                        invDet;

	inverseMatrix.m[1][1] = (matrix.m[0][0] * matrix.m[2][2] * matrix.m[3][3] - matrix.m[0][0] * matrix.m[2][3] * matrix.m[3][2] - matrix.m[0][2] * matrix.m[2][0] * matrix.m[3][3] +
	                         matrix.m[0][2] * matrix.m[2][3] * matrix.m[3][0] + matrix.m[0][3] * matrix.m[2][0] * matrix.m[3][2] - matrix.m[0][3] * matrix.m[2][2] * matrix.m[3][0]) *
	                        invDet;

	inverseMatrix.m[1][2] = (-(matrix.m[0][0] * matrix.m[1][2] * matrix.m[3][3]) + matrix.m[0][0] * matrix.m[1][3] * matrix.m[3][2] + matrix.m[0][2] * matrix.m[1][0] * matrix.m[3][3] -
	                         matrix.m[0][2] * matrix.m[1][3] * matrix.m[3][0] - matrix.m[0][3] * matrix.m[1][0] * matrix.m[3][2] + matrix.m[0][3] * matrix.m[1][2] * matrix.m[3][0]) *
	                        invDet;

	inverseMatrix.m[1][3] = (matrix.m[0][0] * matrix.m[1][2] * matrix.m[2][3] - matrix.m[0][0] * matrix.m[1][3] * matrix.m[2][2] - matrix.m[0][2] * matrix.m[1][0] * matrix.m[2][3] +
	                         matrix.m[0][2] * matrix.m[1][3] * matrix.m[2][0] + matrix.m[0][3] * matrix.m[1][0] * matrix.m[2][2] - matrix.m[0][3] * matrix.m[1][2] * matrix.m[2][0]) *
	                        invDet;

	inverseMatrix.m[2][0] = (matrix.m[1][0] * matrix.m[2][1] * matrix.m[3][3] - matrix.m[1][0] * matrix.m[2][3] * matrix.m[3][1] - matrix.m[1][1] * matrix.m[2][0] * matrix.m[3][3] +
	                         matrix.m[1][1] * matrix.m[2][3] * matrix.m[3][0] + matrix.m[1][3] * matrix.m[2][0] * matrix.m[3][1] - matrix.m[1][3] * matrix.m[2][1] * matrix.m[3][0]) *
	                        invDet;

	inverseMatrix.m[2][1] = (-(matrix.m[0][0] * matrix.m[2][1] * matrix.m[3][3]) + matrix.m[0][0] * matrix.m[2][3] * matrix.m[3][1] + matrix.m[0][1] * matrix.m[2][0] * matrix.m[3][3] -
	                         matrix.m[0][1] * matrix.m[2][3] * matrix.m[3][0] - matrix.m[0][3] * matrix.m[2][0] * matrix.m[3][1] + matrix.m[0][3] * matrix.m[2][1] * matrix.m[3][0]) *
	                        invDet;

	inverseMatrix.m[2][2] = (matrix.m[0][0] * matrix.m[1][1] * matrix.m[3][3] - matrix.m[0][0] * matrix.m[1][3] * matrix.m[3][1] - matrix.m[0][1] * matrix.m[1][0] * matrix.m[3][3] +
	                         matrix.m[0][1] * matrix.m[1][3] * matrix.m[3][0] + matrix.m[0][3] * matrix.m[1][0] * matrix.m[3][1] - matrix.m[0][3] * matrix.m[1][1] * matrix.m[3][0]) *
	                        invDet;

	inverseMatrix.m[2][3] = (-(matrix.m[0][0] * matrix.m[1][1] * matrix.m[2][3]) + matrix.m[0][0] * matrix.m[1][3] * matrix.m[2][1] + matrix.m[0][1] * matrix.m[1][0] * matrix.m[2][3] -
	                         matrix.m[0][1] * matrix.m[1][3] * matrix.m[2][0] - matrix.m[0][3] * matrix.m[1][0] * matrix.m[2][1] + matrix.m[0][3] * matrix.m[1][1] * matrix.m[2][0]) *
	                        invDet;

	inverseMatrix.m[3][0] = (-(matrix.m[1][0] * matrix.m[2][1] * matrix.m[3][2]) + matrix.m[1][0] * matrix.m[2][2] * matrix.m[3][1] + matrix.m[1][1] * matrix.m[2][0] * matrix.m[3][2] -
	                         matrix.m[1][1] * matrix.m[2][2] * matrix.m[3][0] - matrix.m[1][2] * matrix.m[2][0] * matrix.m[3][1] + matrix.m[1][2] * matrix.m[2][1] * matrix.m[3][0]) *
	                        invDet;

	inverseMatrix.m[3][1] = (matrix.m[0][0] * matrix.m[2][1] * matrix.m[3][2] - matrix.m[0][0] * matrix.m[2][2] * matrix.m[3][1] - matrix.m[0][1] * matrix.m[2][0] * matrix.m[3][2] +
	                         matrix.m[0][1] * matrix.m[2][2] * matrix.m[3][0] + matrix.m[0][2] * matrix.m[2][0] * matrix.m[3][1] - matrix.m[0][2] * matrix.m[2][1] * matrix.m[3][0]) *
	                        invDet;

	inverseMatrix.m[3][2] = (-(matrix.m[0][0] * matrix.m[1][1] * matrix.m[3][2]) + matrix.m[0][0] * matrix.m[1][2] * matrix.m[3][1] + matrix.m[0][1] * matrix.m[1][0] * matrix.m[3][2] -
	                         matrix.m[0][1] * matrix.m[1][2] * matrix.m[3][0] - matrix.m[0][2] * matrix.m[1][0] * matrix.m[3][1] + matrix.m[0][2] * matrix.m[1][1] * matrix.m[3][0]) *
	                        invDet;

	inverseMatrix.m[3][3] = (matrix.m[0][0] * matrix.m[1][1] * matrix.m[2][2] - matrix.m[0][0] * matrix.m[1][2] * matrix.m[2][1] - matrix.m[0][1] * matrix.m[1][0] * matrix.m[2][2] +
	                         matrix.m[0][1] * matrix.m[1][2] * matrix.m[2][0] + matrix.m[0][2] * matrix.m[1][0] * matrix.m[2][1] - matrix.m[0][2] * matrix.m[1][1] * matrix.m[2][0]) *
	                        invDet;

	return inverseMatrix;
}
