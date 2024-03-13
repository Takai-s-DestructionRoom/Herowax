#include "Particle3D.hlsli"

//四角形の頂点数
static const uint kVnum = 24;

//センターからのオフセット
static const float4 kOffset_array[kVnum] =
{
    //--------------- 手前 ---------------//
    float4(-0.5f, -0.5f, -0.5f, 0), //左下
    float4(-0.5f, +0.5f, -0.5f, 0), //左上
    float4(+0.5f, -0.5f, -0.5f, 0), //右下
    float4(+0.5f, +0.5f, -0.5f, 0), //右上
    
    //---------------  奥  ---------------//
    float4(-0.5f, -0.5f, +0.5f, 0), //左下
    float4(-0.5f, +0.5f, +0.5f, 0), //左上
    float4(+0.5f, -0.5f, +0.5f, 0), //右下
    float4(+0.5f, +0.5f, +0.5f, 0), //右上
    
    //---------------  左  ---------------//
    float4(-0.5f, -0.5f, -0.5f, 0), //左下
    float4(-0.5f, -0.5f, +0.5f, 0), //左上
    float4(-0.5f, +0.5f, -0.5f, 0), //右下
    float4(-0.5f, +0.5f, +0.5f, 0), //右上
    
    //---------------  右  ---------------//
    float4(+0.5f, -0.5f, -0.5f, 0), //左下
    float4(+0.5f, -0.5f, +0.5f, 0), //左上
    float4(+0.5f, +0.5f, -0.5f, 0), //右下
    float4(+0.5f, +0.5f, +0.5f, 0), //右上
    
    //---------------  上  ---------------//
    float4(-0.5f, -0.5f, -0.5f, 0), //左下
    float4(+0.5f, -0.5f, -0.5f, 0), //左上
    float4(-0.5f, -0.5f, +0.5f, 0), //右下
    float4(+0.5f, -0.5f, +0.5f, 0), //右上
    
    //---------------  下  ---------------//
    float4(-0.5f, +0.5f, -0.5f, 0), //左下
    float4(+0.5f, +0.5f, -0.5f, 0), //左上
    float4(-0.5f, +0.5f, +0.5f, 0), //右下
    float4(+0.5f, +0.5f, +0.5f, 0), //右上
};

//左上が0,0　右下が1,1
static const float2 kUV_array[kVnum] =
{
    //--------------- 手前 ---------------//
    float2(0.0f, 1.0f), //左下
    float2(0.0f, 0.0f), //左上
    float2(1.0f, 1.0f), //右下
    float2(1.0f, 0.0f), //右上
    //---------------  奥  ---------------//
    float2(0.0f, 1.0f), //左下
    float2(0.0f, 0.0f), //左上
    float2(1.0f, 1.0f), //右下
    float2(1.0f, 0.0f), //右上
    //---------------  左  ---------------//
    float2(0.0f, 1.0f), //左下
    float2(0.0f, 0.0f), //左上
    float2(1.0f, 1.0f), //右下
    float2(1.0f, 0.0f), //右上
    //---------------  右  ---------------//
    float2(0.0f, 1.0f), //左下
    float2(0.0f, 0.0f), //左上
    float2(1.0f, 1.0f), //右下
    float2(1.0f, 0.0f), //右上
    //---------------  上  ---------------//
    float2(0.0f, 1.0f), //左下
    float2(0.0f, 0.0f), //左上
    float2(1.0f, 1.0f), //右下
    float2(1.0f, 0.0f), //右上
    //---------------  下  ---------------//
    float2(0.0f, 1.0f), //左下
    float2(0.0f, 0.0f), //左上
    float2(1.0f, 1.0f), //右下
    float2(1.0f, 0.0f), //右上
};

static const float3 kNormal_array[kVnum] =
{
    //--------------- 手前 ---------------//
    float3(0.0f, 0.0f, -1.0f), //左下
    float3(0.0f, 0.0f, -1.0f), //左上
    float3(0.0f, 0.0f, -1.0f), //右下
    float3(0.0f, 0.0f, -1.0f), //右上
    //---------------  奥  ---------------//
    float3(0.0f, 0.0f, +1.0f), //左下
    float3(0.0f, 0.0f, +1.0f), //左上
    float3(0.0f, 0.0f, +1.0f), //右下
    float3(0.0f, 0.0f, +1.0f), //右上
    //---------------  左  ---------------//
    float3(-1.0f, 0.0f, 0.0f), //左下
    float3(-1.0f, 0.0f, 0.0f), //左上
    float3(-1.0f, 0.0f, 0.0f), //右下
    float3(-1.0f, 0.0f, 0.0f), //右上
    //---------------  右  ---------------//
    float3(+1.0f, 0.0f, 0.0f), //左下
    float3(+1.0f, 0.0f, 0.0f), //左上
    float3(+1.0f, 0.0f, 0.0f), //右下
    float3(+1.0f, 0.0f, 0.0f), //右上
    //---------------  上  ---------------//
    float3(0.0f, -1.0f, 0.0f), //左下
    float3(0.0f, -1.0f, 0.0f), //左上
    float3(0.0f, -1.0f, 0.0f), //右下
    float3(0.0f, -1.0f, 0.0f), //右上
    //---------------  下  ---------------//
    float3(0.0f, +1.0f, 0.0f), //左下
    float3(0.0f, +1.0f, 0.0f), //左上
    float3(0.0f, +1.0f, 0.0f), //右下
    float3(0.0f, +1.0f, 0.0f), //右上
};

[maxvertexcount(kVnum)]
void main(
	point VSOutput input[1],
	inout TriangleStream<GSOutput> output
)
{
    GSOutput element; //出力用頂点データ
   
    for (uint i = 0; i < kVnum; i++)
    {
        //中心からのオフセットをスケーリング
        float4 offset = kOffset_array[i] * input[0].scale;
        
        //Z軸回転行列
        float sinZ = sin(input[0].rot.z);
        float cosZ = cos(input[0].rot.z);

        float4x4 matZ = float4x4(
        cosZ, sinZ, 0, 0,
        -sinZ, cosZ, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1);
        
        //X軸回転行列
        float sinX = sin(input[0].rot.x);
        float cosX = cos(input[0].rot.x);

        float4x4 matX = float4x4(
        1, 0, 0, 0,
        0, cosX, sinX, 0,
        0, -sinX, cosX, 0,
        0, 0, 0, 1);
        
        //Y軸回転行列
        float sinY = sin(input[0].rot.y);
        float cosY = cos(input[0].rot.y);

        float4x4 matY = float4x4(
        cosY, 0, sinY, 0,
        0, 1, 0, 0,
        -sinY, 0, cosY, 0,
        0, 0, 0, 1);
        
        offset = mul(matZ, offset);
        offset = mul(matX, offset);
        offset = mul(matY, offset);
        
        float4x4 rotMat = world;
        rotMat = mul(matZ, rotMat);
        rotMat = mul(matX, rotMat);
        rotMat = mul(matY, rotMat);
        
        //システム用頂点座標
        //オフセット分ずらす(ワールド座標)
        float4 svpos = input[0].pos + offset;
        
        //ビュー、射影変換
        svpos = mul(viewproj, svpos);
        float4 wpos = mul(world, input[0].pos + offset);
        element.svpos = svpos;
        element.worldpos = wpos;
        element.color = input[0].color;
        float4 wnormal = normalize(mul(rotMat, float4(kNormal_array[i], 0)));
        element.normal = wnormal.xyz;
        element.uv = kUV_array[i];
        
        output.Append(element);
    }
}