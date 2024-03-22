#include "SimpleLine.hlsli"

[maxvertexcount(6)]
void main(
	line VSOutput input[2], 
	inout TriangleStream< GSOutput > output
)
{
    float rad = atan2(input[1].pos.y - input[0].pos.y, input[1].pos.x - input[0].pos.x);
    float offsetRad = 90.0f * (3.141592f / 180.0f);
    
	for (uint i = 0; i < 2; i++)
	{
        float offsetLength = input[i].thickness / 2.0f;
		
		{
            GSOutput element;
            float2 offset = offsetLength * float2(cos(rad + offsetRad), sin(rad + offsetRad));
            element.pos = input[i].pos + float4(offset.x, offset.y, 0.0f, 0.0f);
            element.pos = mul(matViewProjection, element.pos);
            element.color = input[i].color;
            output.Append(element);
        }
		{
            GSOutput element;
            float2 offset = offsetLength * float2(cos(rad - offsetRad), sin(rad - offsetRad));
            element.pos = input[i].pos + float4(offset.x, offset.y, 0.0f, 0.0f);
            element.pos = mul(matViewProjection, element.pos);
            element.color = input[i].color;
            output.Append(element);
        }
        {
            GSOutput element;
            float2 offset = offsetLength * float2(cos(rad - (offsetRad + offsetRad * 2 * (i % 2))), sin(rad - (offsetRad + offsetRad * 2 * (i % 2))));
            element.pos = input[(i + 1) % 2].pos + float4(offset.x, offset.y, 0.0f, 0.0f);
            element.pos = mul(matViewProjection, element.pos);
            element.color = input[(i + 1) % 2].color;
            output.Append(element);
        }
        
        output.RestartStrip();

    }
}