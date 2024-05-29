#include "SlimeWaxLight.hlsli"

Texture2D<float4> mainTex : register(t0); //0番スロットに設定されたテクスチャ
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

struct PS_OUT
{
    float4 color : SV_TARGET;
    float depth : SV_DEPTH;
};

PS_OUT main(VSOutput input)
{
    //float4 texcolor = float4(mainTex.Sample(smp, input.uv));
    //texcolor = texcolor * m_color;
	
    //レイの開始点をオブジェクトからにすることで
    //どれだけ離れてもraymarchを回す回数が一定になるため、描画されるようになる
    float3 pos = input.wpos.xyz;
    //視線から伸ばすベクトル
    float3 rayDir = normalize(input.wpos.xyz - cameraPos);
	
    //リムライトをつける
    float3 norm = input.normal;
    float4 baseColor = waxColor;

    //リムライトの強さ(光ってる部分の大きさ)
    const float rimRate = pow(1 - abs(dot(norm, rayDir)), rimPower); //輪郭っぽい部分の算出  
            
    float4 color = clamp(lerp(baseColor, rimColor, rimRate), 0, 1); //色
            
    PS_OUT output;
    output.color = color;
    output.depth = getDepth(pos);
            
    return output;
}