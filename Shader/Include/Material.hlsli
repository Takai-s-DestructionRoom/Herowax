// マテリアル
cbuffer ConstBufferMaterial : register(b0)
{
	float4 m_color : packoffset(c0); // 色(RGBA)
	float3 m_ambient : packoffset(c1);
	float3 m_diffuse : packoffset(c2);
	float3 m_specular : packoffset(c3);
};