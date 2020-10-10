#include "common.hlsl"

struct PS_INPUT {
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD;
};

cbuffer GlobalData : register(b0) {
	float time;
};

VK_BINDING(0, 0) Texture2D tex;
VK_BINDING(0, 0) SamplerState samp;

float4 main(PS_INPUT input) : SV_Target
{
	return tex.Sample(samp, input.texcoord) * float4(sin(abs(time * 0.01)), 1, cos(abs(time * 0.01)), 1);
}
