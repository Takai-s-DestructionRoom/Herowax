// ビュー&射影変換行列
cbuffer ConstBufferDataMetaball2D : register(b0)
{
    float4 color_;
    float4 strokecolor_;
    float cutoff_;
    float stroke_;
};

// 頂点シェーダーの出力構造体
struct VSOutput
{
    float4 svpos : SV_POSITION; // システム用頂点座標
    float2 uv : TEXCOORD; // uv値
};