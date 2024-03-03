/*
* @file SimpleDrawer.h
* @brief シンプルな描画を関数一つで行う機能を提供するクラス
*/

#pragma once
#include "RDirectX.h"
#include "SimpleDrawInfo.h"
#include <mutex>

class SimpleDrawer
{
public:
	static SimpleDrawer* GetInstance() {
		static SimpleDrawer instance;
		return &instance;
	}

	static void ClearData();

	/// <summary>
	/// 2D画面に線を描画する
	/// </summary>
	/// <param name="x1">開始位置のスクリーン座標X</param>
	/// <param name="y1">開始位置のスクリーン座標Y</param>
	/// <param name="x2">終了位置のスクリーン座標X</param>
	/// <param name="y2">終了位置のスクリーン座標Y</param>
	/// <param name="layer">描画優先度</param>
	/// <param name="color">色</param>
	/// <param name="thickness">線の太さ</param>
	static void DrawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, float layer, Color color, float thickness = 1.0f);


	/// <summary>
	/// 2D画面に四角形を描画する
	/// </summary>
	/// <param name="x1">左上のスクリーン座標X</param>
	/// <param name="y1">左上のスクリーン座標Y</param>
	/// <param name="x2">右下のスクリーン座標X</param>
	/// <param name="y2">右下のスクリーン座標Y</param>
	/// <param name="layer">描画優先度</param>
	/// <param name="color">色</param>
	/// <param name="fillFlag">内側を塗りつぶすか</param>
	/// <param name="thickness">塗りつぶさない場合の線の太さ</param>
	static void DrawBox(int32_t x1, int32_t y1, int32_t x2, int32_t y2, float layer, Color color, bool fillFlag, float thickness = 1.0f);

	/// <summary>
	/// 2D画面に四角形を描画する
	/// </summary>
	/// <param name="x1">左上のスクリーン座標X</param>
	/// <param name="y1">左上のスクリーン座標Y</param>
	/// <param name="x2">右下のスクリーン座標X</param>
	/// <param name="y2">右下のスクリーン座標Y</param>
	/// <param name="layer">描画優先度</param>
	/// <param name="color">色</param>
	/// <param name="fillFlag">内側を塗りつぶすか</param>
	/// <param name="thickness">塗りつぶさない場合の線の太さ</param>
	static void DrawBox(float x1, float y1, float x2, float y2, float layer, Color color, bool fillFlag, float thickness = 1.0f);


	/// <summary>
	/// 2D画面に円を描画する
	/// </summary>
	/// <param name="x">中心のスクリーン座標X</param>
	/// <param name="y">中心のスクリーン座標Y</param>
	/// <param name="layer">描画優先度</param>
	/// <param name="color">色</param>
	/// <param name="fillFlag">内側を塗りつぶすか</param>
	/// <param name="thickness">塗りつぶさない場合の線の太さ</param>
	static void DrawCircle(int32_t x, int32_t y, int32_t r, float layer, Color color, bool fillFlag, float thickness = 1.0f);
	
	static void DrawString(float x, float y, float layer, std::string text, Color color = Color(1.0f, 1.0f, 1.0f, 1.0f), std::string fontTypeFace = "", uint32_t fontSize = 20, Vector2 anchor = {0, 0});

	//後回し描画によって提供されるものを全部描画する
	static void DrawAll();

private:
	SimpleDrawer() {
		Init();
	};
	~SimpleDrawer() = default;
	SimpleDrawer(const SimpleDrawer& a) = default;
	SimpleDrawer& operator=(const SimpleDrawer&) { return *this; }

	std::list<std::shared_ptr<SimpleDrawInfo>> mInfoList;
	std::unordered_map<std::string, std::shared_ptr<SRBufferPtr>> mRecycleBuffs;

	std::unordered_map<float, std::vector<DrawBoxInfo>> mBoxInfoMap;
	std::unordered_map<float, DrawBuffers> mBoxBuffersMap;
	RootSignature mBoxRS;
	GraphicsPipeline mBoxPSO;

	std::unordered_map<float, std::vector<DrawLineInfo>> mLineInfoMap;
	std::unordered_map<float, DrawBuffers> mLineBuffersMap;
	RootSignature mLineRS;
	GraphicsPipeline mLinePSO;
	SRConstBuffer<ViewProjectionBuffer> mLineVPBuff;

	std::unordered_map<DrawCustomData, std::vector<DrawCircleInfo>> mCircleInfoMap;
	std::unordered_map<DrawCustomData, DrawCircleVertIndex> mCircleVertIndexMap;
	std::unordered_map<DrawCustomData, DrawBuffers> mCircleBuffersMap;
	RootSignature mCircleRS;
	GraphicsPipeline mCirclePSO;
	SRConstBuffer<ViewProjectionBuffer> mCircleVPBuff;
	void CalcCircleVertAndIndex(DrawCustomData cData);

	std::unordered_map<float, std::vector<DrawLine3DInfo>> mLine3DInfoMap;
	std::unordered_map<float, DrawBuffers> mLine3DBuffersMap;
	RootSignature mLine3DRS;
	GraphicsPipeline mLine3DPSO;

	RootSignature mRootSignature;
	GraphicsPipeline mPipelineState;

	RootSignature mRootSignatureForString;
	GraphicsPipeline mPipelineStateForString;

	std::mutex mMutex;

	void Init();
};

