#include "ParticleBillboard.hlsli"

Texture2D<float4> tex : register(t0);  // 0番スロットに設定されたテクスチャ
SamplerState smp : register(s0);      // 0番スロットに設定されたサンプラー

float4 main(GSOutput input) : SV_TARGET
{
    // テクスチャマッピング
    float4 texcolor = tex.Sample(smp, input.uv);
    
    float threshold = 0.3;

    if (texcolor.r < threshold || texcolor.g < threshold || texcolor.b < threshold)
    {
        texcolor.a = 0.0;
    }
    
    // シェーディング色で描画
    float4 color = texcolor * input.color;
    
    return color;
}