#include "ParticleBillboard.hlsli"

Texture2D<float4> tex : register(t0);  // 0番スロットに設定されたテクスチャ
SamplerState smp : register(s0);      // 0番スロットに設定されたサンプラー

float4 main(GSOutput input) : SV_TARGET
{
    // テクスチャマッピング
    float4 texcolor = tex.Sample(smp, input.uv);
    
    //--- 炎用の2値化処理 ---//
    ////ある程度下限を引き上げないと黒めのとこ結構入る
    //float threshold = clamp(input.timer,0.2,1.0);

    ////閾値よりも暗かったら描画しない
    //if (texcolor.r < threshold || texcolor.g < threshold || texcolor.b < threshold)
    //{
    //    texcolor.a = 0.0;
    //}
    
    // シェーディング色で描画
    float4 color = texcolor * input.color;
    
    return color;
}