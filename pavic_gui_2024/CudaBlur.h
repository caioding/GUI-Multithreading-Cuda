#pragma once

// Declaração da função wrapper que será chamada a partir do C++/CLI
#ifdef __cplusplus
extern "C" {
#endif

	void apply_cuda_blur(unsigned char* input_image, unsigned char* output_image, int width, int height, int channels, int blur_radius);

#ifdef __cplusplus
}
#endif