// ビュー&射影変換行列
cbuffer ConstBufferDataViewProjection : register(b0)
{
	matrix matViewProjection;
};

// 頂点シェーダーの出力構造体
struct VSOutput
{
	float4 svpos : SV_POSITION; // 頂点座標
    float4 color : COLOR;
};