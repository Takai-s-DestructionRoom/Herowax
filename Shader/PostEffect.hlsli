// マテリアル
cbuffer ConstBufferDataMaterial : register(b0)
{
    float4 m_color : packoffset(c0); // 色(RGBA)
    float3 m_ambient : packoffset(c1);
    float3 m_diffuse : packoffset(c2);
    float3 m_specular : packoffset(c3);
};

// 頂点シェーダーの出力構造体
struct VSOutput
{
    float4 svpos : SV_POSITION; // システム用頂点座標
    float2 uv : TEXCOORD; // uv値
};