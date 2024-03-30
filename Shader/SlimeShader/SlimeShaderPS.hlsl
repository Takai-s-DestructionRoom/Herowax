#include "SlimeShader.hlsli"

Texture2D<float4> mainTex : register(t0); //0番スロットに設定されたテクスチャ
SamplerState smp : register(s0); //0番スロットに設定されたサンプラー

static const int MAX_SPHERE_COUNT = 256;
static float4 Spheres[MAX_SPHERE_COUNT];
static int sphereCount = 256;

//球の距離関数
float sphereDistanceFunction(float4 sphere, float3 pos)
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
    for (int i = 0; i < sphereCount; i++)
    {
       //dist = min(dist, sphereDistanceFunction(Spheres[i], pos));
       dist = smoothMin(dist, sphereDistanceFunction(Spheres[i], pos), 7.5f);
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

float rand(float2 seed)
{
    return frac(sin(dot(seed.xy, float2(12.9898, 78.233))) * 43758.5453);
}

float4 main(VSOutput input) : SV_TARGET
{
    //float4 texcolor = float4(mainTex.Sample(smp, input.uv));
    //texcolor = texcolor * m_color;
	
    float3 pos = input.wpos.xyz;
    //視線から伸ばすベクトル
    float3 rayDir = normalize(pos.xyz - cameraPos);
    
    for (int x = 0; x < sphereCount; x++)
    {
        float3 center = float3(rand(x), rand(x + 1), rand(x + 2) * 8 - 4);
        float radius = rand(x + 3) * 0.25f;
        Spheres[x] = float4(center, radius);
    }
    
    for (int i = 0; i < 30; i++)
    {
        float dist = getDistance(pos);
        if (dist < 0.01)
        {
            float3 baseColor = (0.8f, 0.6f, 0.35f);
            float3 norm = getNormal(pos);
            
            float rimPower = 2;
            float rimRate = pow(1 - abs(dot(norm, rayDir)), rimPower);
            float3 rimColor = float3(1.5f, 1.5f, 1.5f);
           
            //float4 color = 

            return (clamp(lerp(baseColor, rimColor, rimRate), 0, 1), 1);

        }
        
        pos += dist * rayDir;
    }
	
    return float4(0, 0, 0, 0);
    //return texcolor;
}