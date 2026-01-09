struct Input {
	float3 pos : POSITION0;
};

struct Output {
	float4 pos : SV_POSITION;
};

Output main(Input input) {
	Output output = (Output)0;

    output.pos = float4(input.pos, 1);

	return output;
}