#include "gpu_image/cuda_error.hpp"
#include <sstream>

namespace gpu_image {

namespace {
std::string buildCudaErrorMessage(cudaError_t error, const char* file,
                                  int line) {
  std::ostringstream oss;
  oss << "CUDA Error: " << cudaGetErrorString(error) << " (code "
      << static_cast<int>(error) << ")" << " at " << file << ":" << line;
  return oss.str();
}
} // anonymous namespace

CudaException::CudaException(cudaError_t error, const char* file, int line)
    : std::runtime_error(buildCudaErrorMessage(error, file, line)),
      error_(error) {}

} // namespace gpu_image
