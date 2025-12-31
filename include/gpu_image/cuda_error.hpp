#pragma once

#include <cuda_runtime.h>
#include <stdexcept>
#include <string>
#include <variant>

namespace gpu_image {

// CUDA 错误异常类
class CudaException : public std::runtime_error {
public:
    CudaException(cudaError_t error, const char* file, int line);
    cudaError_t errorCode() const { return error_; }
    
private:
    cudaError_t error_;
};

// 错误检查宏
#define CUDA_CHECK(call) do { \
    cudaError_t error = call; \
    if (error != cudaSuccess) { \
        throw gpu_image::CudaException(error, __FILE__, __LINE__); \
    } \
} while(0)

// 错误码枚举
enum class ErrorCode {
    Success = 0,
    OutOfMemory,
    InvalidMemoryAccess,
    InvalidImageSize,
    InvalidKernelSize,
    InvalidChannelCount,
    NullPointer,
    CudaDriverError,
    CudaKernelLaunchError,
    CudaSyncError,
    FileNotFound,
    FileReadError,
    FileWriteError,
    UnsupportedFormat
};

// Result 类型用于不抛异常的 API
template<typename T>
class Result {
public:
    static Result<T> ok(T value) {
        Result<T> r;
        r.data_ = std::move(value);
        return r;
    }
    
    static Result<T> error(std::string message) {
        Result<T> r;
        r.data_ = std::move(message);
        return r;
    }
    
    bool isOk() const { return std::holds_alternative<T>(data_); }
    bool isError() const { return std::holds_alternative<std::string>(data_); }
    
    T& value() { return std::get<T>(data_); }
    const T& value() const { return std::get<T>(data_); }
    
    const std::string& errorMessage() const { return std::get<std::string>(data_); }
    
private:
    std::variant<T, std::string> data_;
};

// void 特化
template<>
class Result<void> {
public:
    static Result<void> ok() {
        Result<void> r;
        r.success_ = true;
        return r;
    }
    
    static Result<void> error(std::string message) {
        Result<void> r;
        r.success_ = false;
        r.errorMsg_ = std::move(message);
        return r;
    }
    
    bool isOk() const { return success_; }
    bool isError() const { return !success_; }
    const std::string& errorMessage() const { return errorMsg_; }
    
private:
    bool success_ = false;
    std::string errorMsg_;
};

} // namespace gpu_image
