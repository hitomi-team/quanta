
struct VS_INPUT {
	float4 position : POSITION;
	float4 normal   : NORMAL;
	float2 texcoord : TEXCOORD0;
};

struct PS_INPUT {
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD0;
};

cbuffer GlobalData : register(b0) {
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
