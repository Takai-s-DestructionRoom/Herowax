#include "WaxShader3.hlsli"

VSOUT main(float4 pos : POSITION)
{
    VSOUT output; // �s�N�Z���V�F�[�_�[�ɓn���l
    //output.wpos = pos;
    output.pos = pos;
    output.radius = pos.w;
    return output;
}