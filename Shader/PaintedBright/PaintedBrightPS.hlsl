#include "../Include/Material.hlsli"
#include "../Include/Transform.hlsli"
#include "../Include/Camera.hlsli"
#include "../Include/Lighting.hlsli"
#include "../Include/VSStruct.hlsli"

Texture2D<float4> tex : register(t0); //0番スロットに設定されたテクスチャ
Texture2D<float4> paintTex : register(t1); //1番スロットに設定されたテクスチャ
SamplerState smp : register(s0); //0番スロットに設定されたサンプラー

struct PaintData
{
    float3 mAmbient;
    float3 mDiffuse;
    float3 mSpecular;
    float4 mColor;
    float dissolveVal;
    float slide;
};

ConstantBuffer<PaintData> paintData : register(b10);

cbuffer ConstBufferAddColor : register(b11)
{
    float4 addColor;
};

//ディザ抜きの目の細かさ(本来は外部テクスチャから読み込んだ方がいい)
static const int pattern[16] =
{
    0, 8, 2, 10,
    12, 4, 14, 6,
    3, 11, 1, 9,
    15, 7, 13, 5
};

float3 magnitude(float3 v)
{
    return (v.x * v.x + v.y * v.y + v.z * v.z);
}

// シェーダー疑似乱数(UVを入力し0.0～1.0を返す/texture size周期でループする処理を追加)
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

float4 main(VSOutputBasic input) : SV_TARGET
{
    //----------ディザ抜きの諸々の処理-------------//
    int ditherUV_x = (int) fmod(input.svpos.x, 4.0f); //パターンの大きさで割ったときの余りを求める
    int ditherUV_y = (int) fmod(input.svpos.y, 4.0f); //今回のパターンサイズは4x4なので4で除算
    float dither = pattern[ditherUV_x + ditherUV_y * 4]; //求めた余りからパターン値を取得
    
    float camlength = magnitude(cameraPos - input.wpos.xyz);
    
    float clamp_dither = saturate(dither / 4);
    float clamp_length = saturate(camlength / 50);
    
    clip(clamp_length - clamp_dither); //閾値が0以下なら描画しない
    
    //----------ディザ抜きの諸々の処理終わり----------//
    
    float4 texcolor = float4(tex.Sample(smp, input.uv));
    texcolor = texcolor * m_color;
    
    //ペイント部分のマテリアル扱い
    float3 m_pAmbient = paintData.mAmbient;
    float3 m_pDiffuse = paintData.mDiffuse;
    float3 m_pSpecular = paintData.mSpecular;
    float4 paintcolor = paintData.mColor;
    float paintRatio = 0;
    
    float3 dist = normalNoise(input.uv + paintData.slide * 0.02f, float2(16.f, 16.f)); // perlinノイズで算出した法線を得る
    dist = dist * 2 - 1; // 範囲を0.0～1.0から-1.0～1.0へ変換
    dist *= 0.1f;

    float dissolveTexVal = paintTex.Sample(smp, input.uv + dist.xy).r;
    if (dissolveTexVal <= paintData.dissolveVal)
    {
        paintRatio = paintcolor.a;
        paintcolor.a = 1;
        texcolor = texcolor * (1 - paintRatio) + paintcolor * paintRatio;
    }
	
	//光沢度
    const float shininess = 4.0f;
	
	//視点へのベクトル
    float3 eyedir = normalize(cameraPos - input.wpos.xyz);
	
	//環境反射光
    float3 ambient = m_ambient * (1 - paintRatio) + m_pAmbient * paintRatio;
	
	//シェーディング結果の色
    float4 shadecolor = float4(ambientColor * ambient, 1);
	
    //平行光源
    for (int i = 0; i < DIRECTIONAL_LIGHT_NUM; i++)
    {
        if (directionalLights[i].active)
        {
            //光源へのベクトルと法線の内積
            float dotNormal = dot(-directionalLights[i].lightVec, input.normal);
		    //反射光ベクトル
            float3 reflect = normalize(directionalLights[i].lightVec + 2.0f * dotNormal * input.normal);
		    //拡散反射光
            float3 diffuse = saturate(dotNormal) * (m_diffuse * (1 - paintRatio) + m_pDiffuse * paintRatio);
		    //鏡面反射光
            float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * (m_specular * (1 - paintRatio) + m_pSpecular * paintRatio);
		
            shadecolor.rgb += (diffuse + specular) * directionalLights[i].lightColor;
        }
    }
    
    //点光源
    for (i = 0; i < POINT_LIGHT_NUM; i++)
    {
        if (pointLights[i].active)
        {
            float3 lightVec = pointLights[i].pos - input.wpos;
            float d = length(lightVec);
            lightVec = normalize(lightVec);
            
            //距離減衰係数
            float atten = 1.0f / (pointLights[i].atten.x + pointLights[i].atten.y * d + pointLights[i].atten.z * d * d);
            
            float dotNormal = dot(lightVec, input.normal);
            //反射光ベクトル
            float3 reflect = normalize(-lightVec + 2.0f * dotNormal * input.normal);
            //拡散反射光
            float3 diffuse = saturate(dotNormal) * m_diffuse;
            //鏡面反射光
            float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * m_specular;
            
            shadecolor.rgb += atten * (diffuse + specular) * pointLights[i].color;
        }
    }

    //スポットライト
    for (i = 0; i < SPOT_LIGHT_NUM; i++)
    {
        if (spotLights[i].active)
        {
            float3 lightVec = spotLights[i].pos - input.wpos;
            float d = length(lightVec);
            lightVec = normalize(lightVec);
            
            //距離減衰係数
            float atten = saturate(1.0f / (spotLights[i].atten.x + spotLights[i].atten.y * d + spotLights[i].atten.z * d * d));
            
            //角度減衰
            float cos = dot(lightVec, -spotLights[i].dir);
            
            //減衰開始角度から、減衰終了角度にかけて減衰
            float angleatten = smoothstep(spotLights[i].factorAngleCos.y, spotLights[i].factorAngleCos.x, cos);
            
            //角度減衰を乗算
            atten *= angleatten;
            
            float dotNormal = dot(lightVec, input.normal);
            //反射光ベクトル
            float3 reflect = normalize(-lightVec + 2.0f * dotNormal * input.normal);
            //拡散反射光
            float3 diffuse = saturate(dotNormal) * m_diffuse;
            //鏡面反射光
            float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * m_specular;
            
            shadecolor.rgb += atten * (diffuse + specular) * spotLights[i].color;
        }
    }
    
    return texcolor * shadecolor + addColor;
}