#include "WaxShader.hlsli"

struct PS_OUT
{
    float4 color : SV_TARGET;
    float depth : SV_DEPTH;
};

float smoothMin(float a, float b, float k)
{
    float h = exp(-k * a) + exp(-k * b);
    if (h == 0)
    {
        return min(a, b);
    }
    return -log(h) / k;
}

float distanceFunc(float3 rPos, float3 sPos, float sphereSize)
{
    return length(sPos - rPos) - sphereSize;
}

// �����ꂩ�Ƃ̍ŒZ������Ԃ�
float getDistance(float3 pos)
{
    float distance = 114514;
    for (uint i = 0; i < bWax.sphereNum; i++)
    {
        distance = smoothMin(distance, distanceFunc(pos, bWax.spheres[i].xyz, bWax.spheres[i].w), bWax.smoothFactor);
        //if (abs(distance) <= bWax.hitThreshold)
        //{
        //    return distance;
        //}
    }
    return distance;
}

// �@�����v�Z����
float3 getNormal(float3 pos)
{
    float d = 0.0001f;
    return normalize(float3(
        getDistance(pos + float3(d, 0, 0)) - getDistance(pos + float3(-d, 0, 0)),
        getDistance(pos + float3(0, d, 0)) - getDistance(pos + float3(0, -d, 0)),
        getDistance(pos + float3(0, 0, d)) - getDistance(pos + float3(0, 0, -d))
    ));
}

// �[�x�l���v�Z����
float getDepth(float3 pos)
{
    float4 vpPos = mul(bCamera.matViewProj, float4(pos, 1));
    return vpPos.z / vpPos.w;
}

PS_OUT main(VSOUT input)
{
    PS_OUT output;
    
    float2 resolution = float2(1280, 720);
    
    float2 pixelpos = input.uv * resolution;
   
    // �X�N���[�����W���烏�[���h���W�n�ɕϊ����ă��C���΂�
    // ��΂����Ƃ�������������A�J�X
    float4 posA = float4(pixelpos.xy, 0, 1);
    float4 posB = float4(pixelpos.xy, 1, 1);
    
    posA = mul(bCamera.matInvViewport, posA);
    posA = mul(bCamera.matInvProj, posA);
    posA /= posA.w;
    posA = mul(bCamera.matInvView, posA);
    
    posB = mul(bCamera.matInvViewport, posB);
    posB = mul(bCamera.matInvProj, posB);
    posB /= posB.w;
    posB = mul(bCamera.matInvView, posB);
    
    float3 rayDir = normalize(posB.xyz - posA.xyz);
    float3 fPos = posA.xyz;

    float rLen = 0.0f;
    float3 rPos = fPos;
    for (uint i = 0; i < bWax.rayMarchNum; i++)
    {
        float distance = getDistance(rPos);
        
        if (abs(distance) <= bWax.hitThreshold)
        {
            //�������C�g������
            float3 norm = getNormal(rPos);
            float3 baseColor = float3(0.8f, 0.6f, 0.35f);

            const float rimPower = 2; //�������C�g�̋���(�����Ă镔���̑傫��)
            const float rimRate = pow(1 - abs(dot(norm, rayDir)), rimPower); //�֊s���ۂ������̎Z�o  
            const float3 rimColor = float3(1.5f, 1.5f, 1.5f); //�������C�g�̐F

            float3 color = clamp(lerp(baseColor, rimColor, rimRate), 0, 1); //�F
            //float3 color = baseColor;
            
            PS_OUT output;
            output.color = float4(color, 1);
            output.depth = getDepth(rPos);
            return output;
        }
        
        rLen += distance;
        rPos = fPos + rayDir * rLen;
    }
    
    output.color = float4(0, 0, 0, 0);
    output.depth = 1;
    return output;
}