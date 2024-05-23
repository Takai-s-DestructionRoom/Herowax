#include "WaxShader3.hlsli"

VSOUT main(float4 pos : POSITION)
{
    VSOUT output; // ピクセルシェーダーに渡す値
    //output.wpos = pos;
    output.pos = pos;
    output.radius = pos.w;
    return output;
}