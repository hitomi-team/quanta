#include "common.hlsl"

struct VS_INPUT {
	VK_LOCATION(0) float4 position : POSITION;
	VK_LOCATION(1) float4 normal   : NORMAL;
	VK_LOCATION(2) float2 texcoord : TEXCOORD0;
};

struct PS_INPUT {
	VK_LOCATION(0) float4 position : SV_POSITION;
	VK_LOCATION(1) float2 texcoord : TEXCOORD0;
};

VK_BINDING(0, 0) cbuffer GlobalData : register(b0) {
	float time;
	float4 mvp;
};

PS_INPUT main(VS_INPUT input)
{
	PS_INPUT outps;

	outps.position = input.position;
	outps.texcoord = input.texcoord;

	return outps;
}
