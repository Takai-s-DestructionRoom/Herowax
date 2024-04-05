#include "SlimeShader.hlsli"

Texture2D<float4> mainTex : register(t0); //0番スロットに設定されたテクスチャ
SamplerState smp : register(s0); //0番スロットに設定されたサンプラー

//球の距離関数
float distanceFuncSphere(float4 sphere, float3 pos)
{
    return length(sphere.xyz - pos) - sphere.w;
}

//smooth min関数(間を埋めるやつ)
float smoothMin(float x1, float x2, float k)
{
    return -log(exp(-k * x1) + exp(-k * x2)) / k;
}

//いずれかの球との最短距離を返す
float getDistance(float3 pos)
{
    float dist = 100000; //ありえへん値
    for (int i = 0; i < sphereNum; i++)
    {
        dist = smoothMin(dist, distanceFuncSphere(Spheres[i], pos), smoothValue);
        //条件を満たすだけ小さくなったら抜ける
        //参考にしたやつにない部分なので、描画が荒くなってたらこいつが原因っぽいけど、
        //これがないとループ回数増えまくって激重になる
        if (abs(dist) < clipValue)
        {
            break;
        }
    }
    
    return dist;
}

float3 getNormal(const float3 pos)
{
    float d = 0.0001;
    return normalize(float3(
                getDistance(pos + float3(d, 0.0, 0.0)) - getDistance(pos + float3(-d, 0.0, 0.0)),
                getDistance(pos + float3(0.0, d, 0.0)) - getDistance(pos + float3(0.0, -d, 0.0)),
                getDistance(pos + float3(0.0, 0.0, d)) - getDistance(pos + float3(0.0, 0.0, -d))
           ));
}

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
    float4 texcolor = float4(mainTex.Sample(smp, input.uv));
    texcolor = texcolor * m_color;
	
    //レイの開始点をオブジェクトからにすることで
    //どれだけ離れてもraymarchを回す回数が一定になるため、描画されるようになる
    float3 pos = input.wpos.xyz;
    //視線から伸ばすベクトル
    float3 rayDir = normalize(input.wpos.xyz - cameraPos);

    for (int i = 0; i < rayMatchNum; i++)
    {
        float dist = getDistance(pos);
        pos += dist * rayDir;
        
        if (abs(dist) < clipValue)
        {
            //リムライトをつける
            float3 norm = getNormal(pos);
            float3 baseColor = float3(0.8f, 0.6f, 0.35f);

            const float rimPower = 2; //リムライトの強さ(光ってる部分の大きさ)
            const float rimRate = pow(1 - abs(dot(norm, rayDir)), rimPower); //輪郭っぽい部分の算出  
            const float3 rimColor = float3(1.5f, 1.5f, 1.5f); //リムライトの色

            float3 color = clamp(lerp(baseColor, rimColor, rimRate), 0, 1); //色
            
            PS_OUT output;
            output.color = float4(color, 1);
            output.depth = getDepth(pos);
            
            return output;
        }
    }
	
    PS_OUT output;
    output.color = float4(0,0,0,0);
    output.depth = 0;
    
    return output;
}