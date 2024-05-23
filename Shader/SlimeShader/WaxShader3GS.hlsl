#include "WaxShader3.hlsli"

static const float4 offsets[4] =
{
    float4(-1, -1, 0, 0), //ç∂â∫
    float4(-1, +1, 0, 0), //ç∂è„
    float4(+1, -1, 0, 0), //âEâ∫
    float4(+1, +1, 0, 0), //âEè„
};

[maxvertexcount(6)]
void main(
	point VSOUT input[1],
	inout TriangleStream< GSOUT > output
)
{
    for (int i = 0; i < 4; i++)
    {
        GSOUT element;
        element.radius = input[0].radius;
        element.wpos = input[0].pos;
        
        float4 offset = float4(offsets[i].x * input[0].radius, offsets[i].y * input[0].radius, 0, 0);

        matrix bill = bCamera.matInvView;
        bill._14 = 0.0f;
        bill._24 = 0.0f;
        bill._34 = 0.0f;
        
        offset = mul(bill, offset);
        element.pos = float4(input[0].pos + offset.xyz, 1);
        element.pos = mul(bCamera.matViewProjection, element.pos);
        output.Append(element);
    }
}