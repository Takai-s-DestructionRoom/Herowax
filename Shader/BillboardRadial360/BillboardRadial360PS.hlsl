#include "../Include/Material.hlsli"
#include "../Include/Transform.hlsli"
#include "../Include/Camera.hlsli"
#include "../Include/VSStruct.hlsli"
#include "BillboardRadial360.hlsli"

Texture2D<float4> tex : register(t0); //0番スロットに設定されたテクスチャ
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

float4 main(VSOutput input) : SV_Target
{ 
    //値が入っている場合のみ処理
    if (radian > 0.f)
    {
        //uvの位置をずらす
        float uvX = (input.uv.x - 0.5f);
        float uvY = (input.uv.y - 0.5f) * -1.0f;
    
        //ピクセルの角度を出す
        float blue = -1 * uvY;
        float red = -1 * uvX;
        //比較したいピクセルの角度
        float thisAngle = atan2(red, blue);
    
        //全て正の値の方が扱いやすいので180°を加算
        thisAngle += 3.141592f;
    
        //定数バッファで渡している値より小さいなら描画キャンセル
        if (thisAngle < radian)
        {
            discard;
        }
    }
    
    float4 texcolor = float4(tex.Sample(smp, input.uv));
    texcolor = texcolor * m_color;

    return texcolor;
}