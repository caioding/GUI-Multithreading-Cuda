#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <cstdio>

// Kernel CUDA para o filtro de desfoque
__global__ void blurKernel(const unsigned char* in, unsigned char* out, int width, int height, int channels, int radius) {
    int col = blockIdx.x * blockDim.x + threadIdx.x;
    int row = blockIdx.y * blockDim.y + threadIdx.y;

    if (col < width && row < height) {
        float r_acc = 0.0f, g_acc = 0.0f, b_acc = 0.0f;
        int pixel_count = 0;

        for (int y = -radius; y <= radius; ++y) {
            for (int x = -radius; x <= radius; ++x) {
                int current_row = row + y;
                int current_col = col + x;

                if (current_row >= 0 && current_row < height && current_col >= 0 && current_col < width) {
                    int idx = (current_row * width + current_col) * channels;
                    r_acc += in[idx + 0];
                    g_acc += in[idx + 1];
                    b_acc += in[idx + 2];
                    pixel_count++;
                }
            }
        }

        int out_idx = (row * width + col) * channels;
        out[out_idx + 0] = static_cast<unsigned char>(r_acc / pixel_count);
        out[out_idx + 1] = static_cast<unsigned char>(g_acc / pixel_count);
        out[out_idx + 2] = static_cast<unsigned char>(b_acc / pixel_count);

        if (channels == 4) {
            out[out_idx + 3] = in[out_idx + 3];
        }
    }
}

// Função wrapper C-style para ser chamada a partir do C++/CLI
extern "C" void apply_cuda_blur(unsigned char* h_img_in, unsigned char* h_img_out, int width, int height, int channels, int blur_radius) {
    size_t img_size = width * height * channels * sizeof(unsigned char);
    unsigned char* d_img_in = nullptr, * d_img_out = nullptr;

    cudaMalloc(&d_img_in, img_size);
    cudaMalloc(&d_img_out, img_size);

    cudaMemcpy(d_img_in, h_img_in, img_size, cudaMemcpyHostToDevice);

    dim3 blockDim(16, 16);
    dim3 gridDim((width + blockDim.x - 1) / blockDim.x, (height + blockDim.y - 1) / blockDim.y);

    // Medição do tempo do kernel
    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);

    cudaEventRecord(start);
    blurKernel << <gridDim, blockDim >> > (d_img_in, d_img_out, width, height, channels, blur_radius);
    cudaEventRecord(stop);

    cudaDeviceSynchronize();

    float milliseconds = 0;
    cudaEventElapsedTime(&milliseconds, start, stop);
    printf("Tempo do kernel CUDA: %.3f ms\n", milliseconds);

    cudaEventDestroy(start);
    cudaEventDestroy(stop);

    cudaMemcpy(h_img_out, d_img_out, img_size, cudaMemcpyDeviceToHost);

    cudaFree(d_img_in);
    cudaFree(d_img_out);
}