#include "common.hlsl"

struct VS_INPUT {
	VK_LOCATION(0) float2 position : POSITION;
	VK_LOCATION(1) float2 texcoord : TEXCOORD0;
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

	outps.position = float4(input.position, 0.0f, 1.0f);
	outps.texcoord = input.texcoord;

	return outps;
}
