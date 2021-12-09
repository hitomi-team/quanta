#include "common.hlsl"

struct PS_INPUT {
	VK_LOCATION(0) float4 position : SV_POSITION;
	VK_LOCATION(1) float2 texcoord : TEXCOORD;
};

float4 main(PS_INPUT input) : SV_Target
{
	return float4(0.1, 0.1, 0.1, 1.0);
}
