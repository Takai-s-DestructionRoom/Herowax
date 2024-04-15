// �}�e���A��
struct Material
{
    float4 m_color; // �F(RGBA)
    float3 m_ambient;
    float3 m_diffuse;
    float3 m_specular;
};
ConstantBuffer<Material> bMaterial : register(b0);

struct Camera
{
    float3 pos;
    float3 dir;
    float3 up;
    matrix matViewProj;
    matrix matInvView;
    matrix matInvProj;
    matrix matInvViewport;
};
ConstantBuffer<Camera> bCamera : register(b1);

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
struct LightGroup
{
    float3 ambientColor;
    DirectionalLight directionalLights[DIRECTIONAL_LIGHT_NUM];
    PointLight pointLights[POINT_LIGHT_NUM];
    SpotLight spotLights[SPOT_LIGHT_NUM];
};
ConstantBuffer<LightGroup> bLightGroup : register(b2);

// ���_�V�F�[�_�[�̏o�͍\����
struct VSOUT
{
    //float3 wpos : WORLD_POSITION; //���[���h���_���W
    float4 svpos : SV_POSITION; // �V�X�e���p���_���W
    float2 uv : TEXCOORD; // uv�l
};

static const int MAX_SPHERE_COUNT = 1024;
struct WaxData
{
    float4 spheres[MAX_SPHERE_COUNT];
    uint sphereNum;
    uint rayMarchNum;
    float hitThreshold;
    float smoothFactor;
};
ConstantBuffer<WaxData> bWax : register(b3);