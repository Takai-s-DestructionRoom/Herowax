#include "../Include/Material.hlsli"
#include "../Include/Transform.hlsli"

// ビュー&射影変換行列
cbuffer ConstBufferDataViewProjection : register(b2)
{
    matrix matViewProjection;
};

// 円形ゲージに必要なもの
cbuffer ConstBufferDataCircleGauge : register(b3)
{
    float radian;  //(0~2πまで)
};

// 頂点シェーダーの出力構造体
struct VSOutput
{
    float4 svpos : SV_POSITION; // システム用頂点座標
    float3 normal : NORMAL; //法線ベクトル
    float2 uv : TEXCOORD; // uv値
};