#include "SimpleInstance.hlsli"

float4 main(VSOutput input) : SV_TARGET
{
    return input.color;
}