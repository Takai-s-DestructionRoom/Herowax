#include "SlimeShader.hlsli"

Texture2D<float4> mainTex : register(t0); //0番スロットに設定されたテクスチャ
SamplerState smp : register(s0); //0番スロットに設定されたサンプラー

float3 trans(float3 p)
{
    return fmod(p, 4.0) - 2.0;
}

float distanceFunc(float4 sphere,float3 p)
{
    return length(trans(p)) - sphere.w;
}

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
    for (int i = 0; i < sphereNum; i++)
    {
        dist = smoothMin(dist, sphereDistanceFunction(Spheres[i], pos), smoothValue);
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

    return (vpPos.z / vpPos.w) * 0.5f + 0.5f;
}

float rand(float2 seed)
{
    return frac(sin(dot(seed.xy, float2(12.9898, 78.233))) * 43758.5453);
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
	
	//光沢度
    const float shininess = 4.0f;
	
	//視点へのベクトル
    float3 eyedir = normalize(cameraPos - input.wpos.xyz);
	
	//環境反射光
    float3 ambient = m_ambient;
	
	//シェーディング結果の色
    float4 shadecolor = float4(ambientColor * ambient, 1);

    float3 pos = input.wpos.xyz;
    //視線から伸ばすベクトル
    float3 rayDir = normalize(pos.xyz - cameraPos);
    
    //for (int x = 0; x < sphereNum; x++)
    //{
    //    float3 center = float3(rand(x), rand(x + 1), rand(x + 2) * 8 - 4);
    //    float radius = rand(x + 3) * 0.25f;
    //    Spheres[x] = float4(center, radius);
    //}
     
    for (int i = 0; i < rayMatchNum; i++)
    {
        float dist = getDistance(pos);
        if (dist < clipValue)
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
            //return float4(color, 1) * texcolor * shadecolor; //塗りつぶし
        }
        
        pos += dist * rayDir;
    }
	
    //return color; //塗りつぶし
    PS_OUT output;
    output.color = float4(0, 0, 0, 0);
    output.depth = 0;
    
    return output;
    //return texcolor * shadecolor;
}