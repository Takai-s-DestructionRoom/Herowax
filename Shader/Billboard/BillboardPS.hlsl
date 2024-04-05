#include "../Include/Material.hlsli"
#include "../Include/Transform.hlsli"
#include "../Include/Camera.hlsli"
#include "../Include/VSStruct.hlsli"
#include "Billboard.hlsli"

Texture2D<float4> tex : register(t0); //0番スロットに設定されたテクスチャ
SamplerState smp : register(s0); //0番スロットに設定されたサンプラー

//深度の計算
float getDepth(float3 pos)
{
    float4 vpPos = mul(matViewProjection, float4(pos, 1.0f));

    //OpenGL環境だとこっちの式を使わないといけないらしい
    //return (vpPos.z / vpPos.w) * 0.5f + 0.5f;
    //DirectX環境ならこっち
    return (vpPos.z / vpPos.w);
}

float4 main(VSOutput input)
{ 
    float4 texcolor = float4(tex.Sample(smp, input.uv));
    texcolor = texcolor * m_color;

    return texcolor;
}