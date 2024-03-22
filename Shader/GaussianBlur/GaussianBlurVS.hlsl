#include "GaussianBlur.hlsli"

OutputVS main( float4 pos : POSITION, float2 uv : TEXCOORD )
{
    OutputVS output;
    output.pos = pos;
    output.uv = uv;
	return output;
}