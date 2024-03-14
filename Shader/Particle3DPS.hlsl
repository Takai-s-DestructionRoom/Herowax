#include "Particle3D.hlsli"

Texture2D<float4> tex : register(t0);  // 0番スロットに設定されたテクスチャ
SamplerState smp : register(s0);      // 0番スロットに設定されたサンプラー

float4 main(GSOutput input) : SV_TARGET
{
    float m_ambient = 0.3f;
    float m_diffuse = 0.8f;
    float m_specular = 0.5f;
    
    // テクスチャマッピング
    float4 texcolor = tex.Sample(smp, input.uv);
    
	// 光沢度
    const float shininess = 4.0f;
	// 頂点から視点への方向ベクトル
    float3 eyedir = normalize(cameraPos - input.worldpos.xyz);
	
	// 環境反射光
    float3 ambient = m_ambient * ambientColor;
    
	// シェーディングによる色
    float4 shadecolor = float4(ambient, 1.0f);
	
    //平行光源
    for (uint i = 0; i < DIRECTIONAL_LIGHT_NUM; i++)
    {
        if (directionalLights[i].active)
        {
            // ライトに向かうベクトルと法線の内積
            float3 dotlightnormal = dot(directionalLights[i].lightVec, input.normal);
	        // 反射光ベクトル
            float3 reflect = normalize(-directionalLights[i].lightVec + 2 * dotlightnormal * input.normal);
            // 拡散反射光
            float3 diffuse = dotlightnormal * 0.3f;
	        // 鏡面反射光
            float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * 0.0f;
	        //ハーフランバート係数
            float3 cos = pow(dotlightnormal * 0.5f + 0.5f, 2.0f);
	        // 全て加算する
            shadecolor.rgb += cos * (diffuse + specular) * directionalLights[i].lightColor;
        }
    }
    
     //点光源
    for (uint j = 0; j < POINT_LIGHT_NUM; j++)
    {
        if (pointLights[j].active)
        {
            // ライトのベクトル
            float3 lightVec = pointLights[j].pos - input.worldpos.xyz;
            //ベクトルの長さ
            float d = length(lightVec);
            //正規化し、単位ベクトルにする
            lightVec = normalize(lightVec);
            //距離減衰係数
            float atten = 1.0f / (pointLights[j].atten.x + pointLights[j].atten.y * d +
            pointLights[j].atten.z * d * d);
            // ライトに向かうベクトルと法線の内積
            float3 dotlightnormal = dot(lightVec, input.normal);
	        // 反射光ベクトル
            float3 reflect = normalize(-lightVec + 2 * dotlightnormal * input.normal);
            // 拡散反射光
            float3 diffuse = dotlightnormal * m_diffuse;
	        // 鏡面反射光
            float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * m_specular;
	        //ハーフランバート係数
            float3 cos = pow(dotlightnormal * 0.5f + 0.5f, 2.0f);
	        // 全て加算する
            shadecolor.rgb += cos * atten * (diffuse + specular) * pointLights[j].color;
        }
    }
    
    //スポットライト
    for (uint k = 0; k < SPOT_LIGHT_NUM; k++)
    {
        if (spotLights[k].active)
        {
            // ライトのベクトル
            float3 lightVec = spotLights[k].pos - input.worldpos.xyz;
            //ベクトルの長さ
            float d = length(lightVec);
            //正規化し、単位ベクトルにする
            lightVec = normalize(lightVec);
            //距離減衰係数
            float atten = saturate(1.0f / (spotLights[k].atten.x + spotLights[k].atten.y * d +
            spotLights[k].atten.z * d * d));
            //角度減衰
            float cos = dot(lightVec, spotLights[k].dir);
            //減衰開始角度から減衰終了角度にかけて減衰
            //減衰開始角度の内側は1倍　減衰終了角度の外側は0倍の輝度
            float angleatten = smoothstep(spotLights[k].factorAngleCos.y, spotLights[k].factorAngleCos.x, cos);
            //角度減衰を乗算
            atten *= angleatten;
            // ライトに向かうベクトルと法線の内積
            float3 dotlightnormal = dot(lightVec, input.normal);
	        // 反射光ベクトル
            float3 reflect = normalize(-lightVec + 2 * dotlightnormal * input.normal);
            // 拡散反射光
            float3 diffuse = dotlightnormal * m_diffuse;
	        // 鏡面反射光
            float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * m_specular;
	        // 全て加算する
            shadecolor.rgb += atten * (diffuse + specular) * spotLights[k].color;
        }
    }
    
    // シェーディング色で描画
    float4 color = shadecolor * texcolor * input.color;
    
    return color;
}