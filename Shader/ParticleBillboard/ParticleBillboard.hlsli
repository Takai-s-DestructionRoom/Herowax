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

// 平行光源型
struct DirectionalLight
{
    uint active;
    float3 lightVec;
    float3 lightColor;
};

//点光源型
struct PointLight
{
    uint active;
    float3 pos;
    float3 color;
    float3 atten;
};

//スポットライト型
struct SpotLight
{
    uint active;
    float3 pos;
    float3 dir;
    float3 color;
    float3 atten;
    float2 factorAngleCos;
};

// 光源情報
static const int DIRECTIONAL_LIGHT_NUM = 8;
static const int POINT_LIGHT_NUM = 8;
static const int SPOT_LIGHT_NUM = 8;
cbuffer ConstBufferDataLight : register(b3)
{
    float3 ambientColor;
    DirectionalLight directionalLights[DIRECTIONAL_LIGHT_NUM];
    PointLight pointLights[POINT_LIGHT_NUM];
    SpotLight spotLights[SPOT_LIGHT_NUM];
};

// 頂点シェーダーからピクセルシェーダーへのやり取りに使用する構造体
struct VSOutput
{
    float4 pos : POSITION;      //システム用頂点座標
    float3 rot : ROT;           //回転情報
    float4 color : COLOR;       //色
    float scale : SCALE;        //スケール
};

struct GSOutput
{
    float4 svpos : SV_POSITION; //システム用頂点座標
    float4 worldpos : POSITION; //ワールド座標
    float4 color : COLOR;       //色
    float3 normal : NORMAL;     //法線ベクトル
    float2 uv : TEXCOORD;       //uv値
};
