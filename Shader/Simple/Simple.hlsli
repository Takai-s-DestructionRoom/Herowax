// 色
cbuffer ConstBufferDataColor : register(b0)
{
	float4 color;
}

// ビュー&射影変換行列
cbuffer ConstBufferDataViewProjection : register(b1)
{
	matrix matViewProjection;
};

// 頂点シェーダーの出力構造体
struct VSOutput
{
	float4 svpos : SV_POSITION;// システム用頂点座標
};