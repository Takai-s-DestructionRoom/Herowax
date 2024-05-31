#include "EasyWaxShader.hlsli"

struct PS_OUT
{
    float4 color : SV_TARGET0;
    //float4 normal : SV_TARGET1;
};

PS_OUT main(VSOUT input)
{
    float3 pos = input.wpos.xyz;
            
    PS_OUT output;
    output.color = bWax.color;
    //output.normal = float4(input.normal.xyz, 1);
    
    return output;
}