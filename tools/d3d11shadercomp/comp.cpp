#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <d3dcompiler.h>

const char *shader_profiles[] = {
	"vs_5_0",
	"ps_5_0"
};

void print_usage()
{
	fprintf(stderr, "usage: d3d11shadercomp.exe [path to shader] [path to output] [shader type]\n\tvalid shader types:\n\t-vs  Vertex Shader\n\t-fs  Fragment Shader\n\n");
}

int main(int argc, char **argv)
{
	if (argc != 4) {
		print_usage();
		return -1;
	}

	const char *selected_shader_profile = NULL;

	if (!strcmp(argv[3], "-vs")) {
		selected_shader_profile = shader_profiles[0];
	} else if (!strcmp(argv[3], "-fs")) {
		selected_shader_profile = shader_profiles[1];
	} else {
		print_usage();
		return -1;
	}

	size_t filename_len = strlen(argv[1]);
	wchar_t *filename = (wchar_t *)malloc(filename_len + 1);
	mbstowcs(filename, argv[1], filename_len + 1);

	ID3DBlob *output = NULL;
	ID3DBlob *error_msg = NULL;
	HRESULT hr = D3DCompileFromFile(filename, NULL, NULL, "main", selected_shader_profile, D3DCOMPILE_OPTIMIZATION_LEVEL3 | D3DCOMPILE_ENABLE_STRICTNESS, 0, &output, &error_msg);
	
	if ((hr != S_OK) || !output) {
		if (error_msg) {
			fprintf(stderr, "Compilation Error: \n%s\n", (char *)error_msg->GetBufferPointer());
			error_msg->Release();
			error_msg = NULL;
			return -1;
		} else {
			fprintf(stderr, "Missing file.\n");
			print_usage();
			return -1;
		}
	}

	size_t bytecode_size = output->GetBufferSize();

	FILE *bytecode_file = fopen(argv[2], "wb");
	
	if (!bytecode_file) {
		fprintf(stderr, "Failed to open file for output: %s\n", argv[2]);
		return -1;
	}

	fwrite(output->GetBufferPointer(), bytecode_size, 1, bytecode_file);
	fclose(bytecode_file);

	output->Release();

	return 0;
}
