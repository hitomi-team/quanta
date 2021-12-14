#include "common.hlsl"

struct PS_INPUT {
	VK_LOCATION(0) float4 position : SV_POSITION;
	VK_LOCATION(1) float2 texcoord : TEXCOORD;
};

VK_BINDING(0, 0) Texture2D gTexture : register(t0);
VK_BINDING(0, 0) SamplerState gSampler : register(s0);

float4 main(PS_INPUT input) : SV_Target
{
	return gTexture.Sample(gSampler, input.texcoord);
}
