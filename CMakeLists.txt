cmake_minimum_required(VERSION 3.18)
project(gpu_image_processing LANGUAGES CXX CUDA)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CUDA_STANDARD 17)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)

# CUDA 配置
find_package(CUDAToolkit REQUIRED)
set(CMAKE_CUDA_ARCHITECTURES 75 80 86)

# 输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 包含目录
include_directories(${CMAKE_SOURCE_DIR}/include)

# 主库
add_library(gpu_image_processing STATIC
    src/cuda_error.cpp
    src/device_buffer.cu
    src/memory_manager.cpp
    src/stream_manager.cu
    src/pixel_operator.cu
    src/convolution_engine.cu
    src/histogram_calculator.cu
    src/image_resizer.cu
    src/image_processor.cpp
    src/pipeline_processor.cu
    src/morphology.cu
    src/threshold.cu
    src/color_space.cu
    src/geometric.cu
)

target_link_libraries(gpu_image_processing CUDA::cudart)

# 测试配置
option(BUILD_TESTS "Build tests" ON)
if(BUILD_TESTS)
    enable_testing()
    
    # 下载 Google Test
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
    )
    FetchContent_MakeAvailable(googletest)
    
    # 下载 RapidCheck
    FetchContent_Declare(
        rapidcheck
        GIT_REPOSITORY https://github.com/emil-e/rapidcheck.git
        GIT_TAG master
    )
    FetchContent_MakeAvailable(rapidcheck)
    
    # 测试可执行文件
    add_executable(gpu_image_tests
        tests/test_main.cpp
        tests/test_device_buffer.cpp
        tests/test_pixel_operator.cpp
        tests/test_convolution.cpp
        tests/test_histogram.cpp
        tests/test_resizer.cpp
        tests/test_pipeline.cpp
        tests/test_morphology.cpp
        tests/test_threshold.cpp
        tests/test_color_space.cpp
        tests/test_geometric.cpp
    )
    
    target_link_libraries(gpu_image_tests
        gpu_image_processing
        GTest::gtest
        GTest::gtest_main
        rapidcheck
    )
    
    add_test(NAME gpu_image_tests COMMAND gpu_image_tests)
endif()

# 示例程序
option(BUILD_EXAMPLES "Build examples" ON)
if(BUILD_EXAMPLES)
    add_executable(basic_example examples/basic_example.cpp)
    target_link_libraries(basic_example gpu_image_processing)
    
    add_executable(pipeline_example examples/pipeline_example.cpp)
    target_link_libraries(pipeline_example gpu_image_processing)
endif()

# 基准测试
option(BUILD_BENCHMARKS "Build benchmarks" ON)
if(BUILD_BENCHMARKS)
    add_executable(benchmark benchmarks/benchmark_main.cpp)
    target_link_libraries(benchmark gpu_image_processing)
endif()
