#include "Particle2D.hlsli"

//四角形の頂点数
static const uint kVnum = 4;

//センターからのオフセット
static const float4 kOffset_array[kVnum] =
{
    float4(-0.5f, +0.5f, 0, 0), //左下
    float4(-0.5f, -0.5f, 0, 0), //左上
    float4(+0.5f, +0.5f, 0, 0), //右下
    float4(+0.5f, -0.5f, 0, 0), //右上
};

//左上が0,0　右下が1,1
static const float2 kUV_array[kVnum] =
{
    float2(0.0f, 1.0f), //左下
    float2(0.0f, 0.0f), //左上
    float2(1.0f, 1.0f), //右下
    float2(1.0f, 0.0f), //右上
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
        
        offset = mul(matZ, offset);
        
        //システム用頂点座標
        //オフセット分ずらす(ワールド座標)
        float4 svpos = mul(matViewProjection, input[0].pos + offset);
        
        //ビュー、射影変換
        element.svpos = svpos;
        element.color = input[0].color;
        element.uv = kUV_array[i];
        
        output.Append(element);
    }
}