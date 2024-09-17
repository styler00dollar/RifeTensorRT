#pragma once
#include <torch/torch.h>
#include <torch/cuda.h>
#include <NvInfer.h>
#include <c10/cuda/CUDAStream.h>
#include <iostream>
#include <string>
#include <chrono>
#include <queue>
#include <thread>
extern "C" {
#include <libavformat/avformat.h>
}
class FFmpegWriter;

class RifeTensorRT {
public:
    RifeTensorRT(std::string interpolateMethod, int interpolateFactor,
        int width, int height, bool half, bool ensemble, bool benchmark, FFmpegWriter& writer);
    void handleModel();
    void processFrame(at::Tensor& frame) const;
    void avframe_nv12_to_rgb_npp(AVFrame* gpu_frame);
    void avframe_rgb_to_nv12_npp(at::Tensor rgb_tensor);
    void RifeTensorRT::run(at::Tensor input);
    // Allocate tensors for Y, U, and V planes on GPU
    int getInterpolateFactor() const { return interpolateFactor; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    cudaStream_t getStream() const { return stream; }
    cudaStream_t getWriteStream() const { return writestream; }

    FFmpegWriter& writer;
    AVBufferRef* hw_frames_ctx;
    AVBufferRef* hw_device_ctx;
    ~RifeTensorRT();
    bool benchmarkMode;

    std::string interpolateMethod;
    int interpolateFactor;
    int width;
    int height;
    bool half;
    bool ensemble;
    bool firstRun;
    bool useI0AsSource;
    torch::Device device;
    cudaStream_t stream, writestream;
    // Tensors
    std::vector<at::Tensor> timestep_tensors;
    torch::Tensor I0, I1, dummyInput, dummyOutput,
        rgb_tensor, y_plane, u_plane, v_plane, uv_flat,
        uv_plane, u_flat, v_flat, nv12_tensor, intermediate_tensor;
    AVFrame* interpolatedFrame, preAllocatedInputFrame;
    // TensorRT engine and context
    nvinfer1::ICudaEngine* engine;
    nvinfer1::IExecutionContext* context;
    std::string enginePath;
    std::vector<void*> bindings;
    torch::ScalarType dType;
};


