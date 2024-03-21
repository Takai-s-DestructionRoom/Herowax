// ���s�����^
struct DirectionalLight
{
    uint active;
    float3 lightVec;
    float3 lightColor;
};

//�_�����^
struct PointLight
{
    uint active;
    float3 pos;
    float3 color;
    float3 atten;
};

//�X�|�b�g���C�g�^
struct SpotLight
{
    uint active;
    float3 pos;
    float3 dir;
    float3 color;
    float3 atten;
    float2 factorAngleCos;
};

// �������
static const int DIRECTIONAL_LIGHT_NUM = 8;
static const int POINT_LIGHT_NUM = 8;
static const int SPOT_LIGHT_NUM = 8;
cbuffer ConstBufferLighting : register(b3)
{
    float3 ambientColor;
    DirectionalLight directionalLights[DIRECTIONAL_LIGHT_NUM];
    PointLight pointLights[POINT_LIGHT_NUM];
    SpotLight spotLights[SPOT_LIGHT_NUM];
};