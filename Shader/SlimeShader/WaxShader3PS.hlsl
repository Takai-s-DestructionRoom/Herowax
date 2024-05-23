#include "WaxShader3.hlsli"

Texture2D<float4> oldDistanceTex : register(t0);
Texture2D<float4> bridgeTex : register(t1);
Texture2D<float4> potentialTex : register(t2);
SamplerState smp : register(s0);

struct PS_OUT
{
    float4 distance : SV_TARGET;
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
    
    float4 oldDistanceData = oldDistanceTex.Sample(smp, input.pos.xy / float2(1280, 720));
    
    //既に表面位置を見つける材料が整っているならコピー(棄却したい)
    if (oldDistanceData.b < 0)
    {
        output.distance = oldDistanceData;
        output.depth = 0;
        return output;
    }
    
    float4 bridgeData = bridgeTex.Sample(smp, input.pos.xy / float2(1280, 720));
    
    float potential = sqrt(potentialTex.Sample(smp, input.pos.xy / float2(1280, 720)).r);
    bool flag = false;
    //ポテンシャルが初めて正になったら
    if (potential - 0.165625f >= 0)
    {
        //正になる直前の点(bridgeBuffに格納されてる)
        output.distance.r = bridgeData.r;
        //正になった点(oldDistanceBuffに格納されてる)
        output.distance.g = oldDistanceData.r;
        //正になってるよ！を他のシェーダーたちに伝える手段として-1に
        output.distance.b = -1;
        
        //下手な値にすると勝手に棄却される？分からんから今は1
        output.distance.a = 1;
        
        //最前面とする
        output.depth = 0;
        return output;
    }
    
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
    
    //ピクセルの中心へ向かうベクトル
    float3 rayDir = normalize(posB.xyz - posA.xyz);
    
    float samplingRate = 1.0f;
    
    float tNext = max(oldDistanceTex.Sample(smp, input.pos.xy / float2(1280, 720)).r, bridgeTex.Load(int3(input.pos.x, input.pos.y, 0)).r);
    tNext += samplingRate;
    float tB = dot(rayDir, input.wpos - bCamera.cameraPos);
    float tF = tB - input.radius;
    
    if (tB < tNext)
    {
        //output.distance = float4(oldDistanceData.r, 0, 0, 1);
        //output.depth = 0.99f;
        //return output;
        discard;
    }
    
    float nextDistance = tF < tNext ? tNext : tF;
    float3 calcPos = bCamera.cameraPos + rayDir * nextDistance;
    
    output.distance = float4(nextDistance, 0, 0, 1);
    output.depth = getDepth(calcPos);
    return output;
}