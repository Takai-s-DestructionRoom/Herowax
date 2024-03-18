#include "BrightCut.hlsli"

Texture2D<float4> tex : register(t0); //0番スロットに設定されたテクスチャ
SamplerState smp : register(s0); //0番スロットに設定されたサンプラー

float4 main(OutputVS i) : SV_TARGET
{
    float4 color = tex.Sample(smp, i.uv);
    float4 t = dot(color.xyz, float3(0.2125f, 0.7154f, 0.0721f));
    float4 extract = smoothstep(0.6, 0.9, t);
    return color * extract;
}