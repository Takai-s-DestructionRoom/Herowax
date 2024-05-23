#include "WaxShader2.hlsli"

Texture2D<float4> distanceTex : register(t0);
Texture2D<float4> oldDistanceTex : register(t1);
SamplerState smp : register(s0);

struct PS_OUT
{
    float4 potential : SV_TARGET0;
    float4 bridge : SV_TARGET1;
    float depth : SV_DEPTH;
};

float getDepth(float3 pos)
{
    float4 vpPos = mul(bCamera.matViewProjection, float4(pos, 1));
    return vpPos.z / vpPos.w;
}

float getPotential(float d, float radius)
{
    if (0 <= d && d <= radius)
        return (1 - d / radius) * (1 - d / radius);
    else
        return 0;
}

PS_OUT main(GSOUT input)
{
    PS_OUT output;
    
    // スクリーン座標からワールド座標系に変換してレイを飛ばす
    // 絶対もっといい書き方ある、カス
    float4 posA = float4(input.pos.xy, 0, 1);
    float4 posB = float4(input.pos.xy, 1, 1);
    
    posA = mul(bCamera.matInvViewport, posA);
    posA = mul(bCamera.matInvProj, posA);
    posA /= posA.w;
    posA = mul(bCamera.matInvView, posA);
    
    posB = mul(bCamera.matInvViewport, posB);
    posB = mul(bCamera.matInvProj, posB);
    posB /= posB.w;
    posB = mul(bCamera.matInvView, posB);
    
    float3 rayDir = normalize(posB.xyz - posA.xyz);

    float distance = distanceTex.Sample(smp, input.pos.xy / float2(1280, 720)).r;
    float3 calcPos = bCamera.cameraPos + rayDir * distance;
    
    float potential = getPotential(length(calcPos - input.wpos), input.radius);
    output.potential = float4(potential, 0, 0, 1);
    output.depth = 0;
    output.bridge = oldDistanceTex.Load(int3(input.pos.x, input.pos.y, 0));
    return output;
}