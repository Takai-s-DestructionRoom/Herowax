#include "../Include/Material.hlsli"
#include "../Include/Transform.hlsli"
#include "../Include/Camera.hlsli"
#include "../Include/Lighting.hlsli"
#include "../Include/VSStruct.hlsli"

cbuffer ConstBufferTiling : register(b4)
{
    float2 tiling;
};