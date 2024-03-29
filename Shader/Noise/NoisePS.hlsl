#include "Noise.hlsli"

Texture2D<float4> tex : register(t0); //0番スロットに設定されたテクスチャ
SamplerState smp : register(s0); //0番スロットに設定されたサンプラー

// シェーダー疑似乱数(fragmentシェーダーでUVを入力し0.0～1.0を返す/texture size周期でループする処理を追加)
float rand(float2 uv, float2 size)
{
    uv = frac(uv / size);
    return frac(sin(dot(frac(uv / size), float2(12.9898, 78.233))) * 43758.5453) * 0.99999;
}

// ランダムな2次元ベクトル(parlin勾配ベクトル/0～1範囲で生成した乱数を-1～1の範囲(*2-1)に拡張)
float2 randVec2(float2 uv, float2 size)
{
    return normalize(float2(rand(uv, size), rand(uv + float2(127.1, 311.7), size)) * 2.0 + -1.0);
}

// バイリニア補間(4点を与えてその矩形内のxy両方向に線形補間)
float2 bilinear(float f0, float f1, float f2, float f3, float fx, float fy)
{
    return lerp(lerp(f0, f1, fx), lerp(f2, f3, fx), fy);
}

// fade関数(ease曲線をつくる。easing関数)
float fade(float t)
{
    return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
}

// 単位パーリンノイズ(ランダムに決定した格子点の勾配ベクトルと、格子点から内部点へのベクトルとの内積をバイリニア補完)
float perlinNoiseOne(float2 uv, float2 size)
{
    float2 p = floor(uv);
    float2 f = frac(uv);

    float d00 = dot(randVec2(p + float2(0, 0), size), f - float2(0, 0));
    float d10 = dot(randVec2(p + float2(1, 0), size), f - float2(1, 0));
    float d01 = dot(randVec2(p + float2(0, 1), size), f - float2(0, 1));
    float d11 = dot(randVec2(p + float2(1, 1), size), f - float2(1, 1));
    return bilinear(d00, d10, d01, d11, fade(f.x), fade(f.y)) + 0.5f;
}

// パーリンノイズ(オクターブの異なるノイズテクスチャを5枚合成)
float perlinNoise(float2 uv, float2 size)
{
    float f = 0;
    f += perlinNoiseOne(uv * 2, size) / 2;
    f += perlinNoiseOne(uv * 4, size) / 4;
    f += perlinNoiseOne(uv * 8, size) / 8;
    f += perlinNoiseOne(uv * 16, size) / 16;
    f += perlinNoiseOne(uv * 32, size) / 32;
    return f;
}


// 法線マップ用ノイズ(x:0.0 ～ 1.0, x:0.0 ～ 1.0, z:1.0)
float3 normalNoise(float2 uv, float2 size)
{
    float3 result = float3(perlinNoise(uv.xy, size),
                            perlinNoise(uv.xy + float2(1, 1), size),
                            1.0);
    return result;
}

float4 main(OutputVS i) : SV_TARGET
{
    //float2 uvMove = { i.uv.x, i.uv.y * time };
    
    
    float3 dist = normalNoise(i.uv, float2(16.f, 16.f)); // perlinノイズで算出した法線を得る
    dist = dist * 2.0 - 1.0; // 範囲を0.0～1.0から-1.0～1.0へ変換
    dist *= 0.1f; // 歪み強度を乗算(歪み強度をシェーダーパラメータとして調整可能にする)

    i.uv.xy += dist.xy; // 歪み量だけ、メインテクスチャのUVをずらす

    float4 texcolor = float4(tex.Sample(smp, i.uv));
    return texcolor;

}