#include "WaxMerge.hlsli"

VSOUT main(float4 pos : SV_POSITION, float2 uv : TEXCOORD)
{
    VSOUT output; // �s�N�Z���V�F�[�_�[�ɓn���l
    //output.wpos = pos;
    output.svpos = pos;
    output.uv = uv;
    return output;
}