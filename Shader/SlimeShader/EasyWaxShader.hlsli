// ワールド変換行列
struct TransformData
{
    matrix matWorld;
};
ConstantBuffer<TransformData> bTransform : register(b0);

// ビュー&射影変換行列
struct ViewProjectionData
{
    matrix matViewProjection;
    float3 cameraPos;
};
ConstantBuffer<ViewProjectionData> bCamera : register(b1);

// ロウパラメータ
struct WaxData
{
    float4 color;
    float distance;
};
ConstantBuffer<WaxData> bWax : register(b2);

// 頂点シェーダーの出力構造体
struct VSOUT
{
    float3 wpos : WORLD_POSITION; //ワールド頂点座標
    float4 svpos : SV_POSITION; // システム用頂点座標
    float3 normal : NORMAL; //法線ベクトル
    float2 uv : TEXCOORD; // uv値
};