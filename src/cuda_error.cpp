#include "gpu_image/cuda_error.hpp"
#include <sstream>

namespace gpu_image {

CudaException::CudaException(cudaError_t error, const char* file, int line)
    : std::runtime_error(""), error_(error) {
    std::ostringstream oss;
    oss << "CUDA Error: " << cudaGetErrorString(error)
        << " (code " << static_cast<int>(error) << ")"
        << " at " << file << ":" << line;
    static_cast<std::runtime_error&>(*this) = std::runtime_error(oss.str());
}

} // namespace gpu_image
