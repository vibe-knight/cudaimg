cmake_minimum_required(VERSION 3.18)
project(gpu_image_processing
    VERSION 2.0.0
    DESCRIPTION "CUDA-based high-performance image processing library"
    LANGUAGES CXX
)

include(CheckLanguage)
check_language(CUDA)
if(NOT CMAKE_CUDA_COMPILER)
    message(FATAL_ERROR "CUDA toolkit with nvcc is required to build mini-opencv. Set CUDAToolkit_ROOT or ensure nvcc is available on PATH.")
endif()
enable_language(CUDA)

# ── C++ / CUDA 标准 ──────────────────────────────────────────────
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CUDA_STANDARD 17)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ── CUDA 配置 ────────────────────────────────────────────────────
find_package(CUDAToolkit REQUIRED)

# 自动检测 GPU 架构（CMake 3.24+），否则使用常见架构
if(NOT DEFINED CMAKE_CUDA_ARCHITECTURES)
    if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.24")
        set(CMAKE_CUDA_ARCHITECTURES native)
    else()
        set(CMAKE_CUDA_ARCHITECTURES 75 80 86 89)
    endif()
endif()

# ── 输出目录 ─────────────────────────────────────────────────────
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# ── 主库 ─────────────────────────────────────────────────────────
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
    src/filters.cu
    src/image_io.cpp
)

add_library(gpu_image::gpu_image_processing ALIAS gpu_image_processing)

target_include_directories(gpu_image_processing
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(gpu_image_processing PUBLIC CUDA::cudart)

target_compile_options(gpu_image_processing
    PRIVATE
        $<$<COMPILE_LANGUAGE:CUDA>:--expt-relaxed-constexpr>
        $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CXX_COMPILER_ID:GNU,Clang>>:-Wall -Wextra -Wpedantic>
        $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CXX_COMPILER_ID:MSVC>>:/W4>
)

# ── 可选：stb 图像 I/O ──────────────────────────────────────────
option(GPU_IMAGE_ENABLE_IO "Enable image file I/O via stb" ON)
if(GPU_IMAGE_ENABLE_IO)
    include(FetchContent)
    FetchContent_Declare(
        stb
        GIT_REPOSITORY https://github.com/nothings/stb.git
        GIT_TAG master
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
    )
    FetchContent_MakeAvailable(stb)
    target_include_directories(gpu_image_processing PRIVATE ${stb_SOURCE_DIR})
    target_compile_definitions(gpu_image_processing PRIVATE GPU_IMAGE_HAS_STB)
endif()

# 版本宏注入
target_compile_definitions(gpu_image_processing PUBLIC
    GPU_IMAGE_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
    GPU_IMAGE_VERSION_MINOR=${PROJECT_VERSION_MINOR}
    GPU_IMAGE_VERSION_PATCH=${PROJECT_VERSION_PATCH}
)

# ── 测试配置 ─────────────────────────────────────────────────────
option(BUILD_TESTS "Build tests" ON)
if(BUILD_TESTS)
    enable_testing()
    
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    
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
        tests/test_filters.cpp
    )
    
    target_link_libraries(gpu_image_tests
        PRIVATE
            gpu_image_processing
            GTest::gtest
            GTest::gtest_main
    )
    
    include(GoogleTest)
    gtest_discover_tests(gpu_image_tests)
endif()

# ── 示例程序 ─────────────────────────────────────────────────────
option(BUILD_EXAMPLES "Build examples" ON)
if(BUILD_EXAMPLES)
    add_executable(basic_example examples/basic_example.cpp)
    target_link_libraries(basic_example PRIVATE gpu_image_processing)
    
    add_executable(pipeline_example examples/pipeline_example.cpp)
    target_link_libraries(pipeline_example PRIVATE gpu_image_processing)
endif()

# ── 基准测试 ─────────────────────────────────────────────────────
option(BUILD_BENCHMARKS "Build benchmarks" OFF)
if(BUILD_BENCHMARKS)
    FetchContent_Declare(
        googlebenchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG v1.8.3
    )
    set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googlebenchmark)

    add_executable(gpu_image_benchmark benchmarks/benchmark_main.cpp)
    target_link_libraries(gpu_image_benchmark
        PRIVATE gpu_image_processing benchmark::benchmark
    )
endif()

# ── Install ──────────────────────────────────────────────────────
include(GNUInstallDirs)

install(TARGETS gpu_image_processing
    EXPORT gpu_image_processing-targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(DIRECTORY include/gpu_image
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(EXPORT gpu_image_processing-targets
    FILE gpu_image_processing-config.cmake
    NAMESPACE gpu_image::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/gpu_image_processing
)
