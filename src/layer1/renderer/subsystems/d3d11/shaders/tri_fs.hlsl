struct PS_INPUT {
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD;
};

cbuffer GlobalData : register(b0) {
	float time;
};

Texture2D tex;
SamplerState samp;

float4 main(PS_INPUT input) : SV_Target
{
	return tex.Sample(samp, input.texcoord) * float4(sin(abs(time)), 1, cos(abs(time)), 1);
}
