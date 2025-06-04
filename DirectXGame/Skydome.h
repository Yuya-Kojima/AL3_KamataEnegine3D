#pragma once
#include "KamataEngine.h"
#include "WorldMatrixTransform.h"

class Skydome {

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(KamataEngine::Model* model);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(KamataEngine::Camera &camera);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Skydome();

private:
	// ワールド変換データ
	KamataEngine::WorldTransform* worldTransform_;

	// モデル
	KamataEngine::Model* model_ = nullptr;

	// カメラ
	KamataEngine::Camera camera_;
};
