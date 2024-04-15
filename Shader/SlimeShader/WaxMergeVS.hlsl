#include "WaxMerge.hlsli"

VSOUT main(float4 pos : SV_POSITION, float2 uv : TEXCOORD)
{
    VSOUT output; // ピクセルシェーダーに渡す値
    //output.wpos = pos;
    output.svpos = pos;
    output.uv = uv;
    return output;
}