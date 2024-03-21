// ワールド変換行列
cbuffer ConstBufferDataTransform : register(b0)
{
    matrix matWorld;
};

// ビルボード行列
cbuffer ConstBufferDataBillboard : register(b1)
{
    matrix matBillboard;
};

// ビュー&射影変換行列
cbuffer ConstBufferDataViewProjection : register(b2)
{
    matrix matViewProjection : packoffset(c0);
    float3 cameraPos : packoffset(c4);
};

// 頂点シェーダーからピクセルシェーダーへのやり取りに使用する構造体
struct VSOutput
{
    float4 pos : POSITION;      //システム用頂点座標
    float3 rot : ROT;           //回転情報
    float4 color : COLOR;       //色
    float scale : SCALE;        //スケール
    float timer : TIMER;        //タイマー
};

struct GSOutput
{
    float4 svpos : SV_POSITION; //システム用頂点座標
    float4 worldpos : POSITION; //ワールド座標
    float4 color : COLOR;       //色
    float3 normal : NORMAL;     //法線ベクトル
    float2 uv : TEXCOORD;       //uv値
    float timer : TIMER;        //タイマー
};
