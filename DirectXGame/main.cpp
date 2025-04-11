#include "GameScene.h"
#include "KamataEngine.h"
#include <Windows.h>

using namespace KamataEngine;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// エンジンの初期化
	KamataEngine::Initialize(L"LE2B_09_コジマ_ユウヤ_AL3");

	// DirectXCommonインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// ゲームシーンのインスタンスを生成
	GameScene* gameScene = new GameScene();

	// ゲームシーンの初期化
	gameScene->Initialize();

	// メインループ
	while (true) {
		// エンジンの更新
		if (KamataEngine::Update()) {
			break;
		}

		// ゲームシーンの更新
		gameScene->Update();

		// 描画開始
		dxCommon->PreDraw();

		gameScene->Draw();

		// 描画終了
		dxCommon->PostDraw();
	}

	// 解放処理

	// エンジンの終了処理
	KamataEngine::Finalize();

	// ゲームシーンの解放
	delete gameScene;

	// nullptrの代入
	gameScene = nullptr;

	return 0;
}
