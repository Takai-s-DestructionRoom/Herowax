#include "GameObject.hlsli"

Texture2D<float4> tex : register(t0); //0番スロットに設定されたテクスチャ
SamplerState smp : register(s0); //0番スロットに設定されたサンプラー

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
	
	//光沢度
    const float shininess = 4.0f;
	
	//視点へのベクトル
    float3 eyedir = normalize(cameraPos - input.wpos.xyz);
	
	//環境反射光
    float3 ambient = m_ambient;
	
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
            float3 diffuse = saturate(dotNormal) * m_diffuse;
		    //鏡面反射光
            float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * m_specular;
		
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