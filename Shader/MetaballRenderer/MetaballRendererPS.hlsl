#include "MetaballRenderer.hlsli"

Texture2D<float4> tex : register(t0); //0番スロットに設定されたテクスチャ
SamplerState smp : register(s0); //0番スロットに設定されたサンプラー

float4 main(VSOutput input) : SV_TARGET
{
    float4 texcolor = float4(tex.Sample(smp, input.uv));
    
    clip(texcolor.a - cutoff_);
    
    texcolor = texcolor.a < stroke_ ? strokecolor_ : color_;
    
    return texcolor;
}