#include "common.hlsl"

struct PS_INPUT {
	VK_LOCATION(0) float4 position : SV_POSITION;
	VK_LOCATION(1) float2 texcoord : TEXCOORD;
};

VK_BINDING(0, 0) cbuffer GlobalData : register(b0) {
	float time;
};

VK_BINDING(1, 0) Texture2D tex : register(t0);
VK_BINDING(1, 0) SamplerState samp : register(s0);

float4 main(PS_INPUT input) : SV_Target
{
	return tex.Sample(samp, input.texcoord) * float4(sin(abs(time)), 1, cos(abs(time)), 1);
}
