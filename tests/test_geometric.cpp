#include <gtest/gtest.h>
#include "gpu_image/geometric.hpp"
#include "gpu_image/gpu_image.hpp"

using namespace gpu_image;

class GeometricTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 创建 8x8 测试图像
        testImage = ImageUtils::createGpuImage(8, 8, 1);
        std::vector<unsigned char> data(64);
        for (int i = 0; i < 64; ++i) {
            data[i] = static_cast<unsigned char>(i * 4);
        }
        cudaMemcpy(testImage.buffer.data(), data.data(), 64, cudaMemcpyHostToDevice);
    }
    
    GpuImage testImage;
};

TEST_F(GeometricTest, Rotate90_Once) {
    GpuImage output;
    Geometric::rotate90(testImage, output, 1);
    cudaDeviceSynchronize();
    
    EXPECT_EQ(output.width, 8);
    EXPECT_EQ(output.height, 8);
    EXPECT_TRUE(output.isValid());
}

TEST_F(GeometricTest, Rotate90_Twice) {
    GpuImage output;
    Geometric::rotate90(testImage, output, 2);
    cudaDeviceSynchronize();
    
    EXPECT_EQ(output.width, 8);
    EXPECT_EQ(output.height, 8);
}

TEST_F(GeometricTest, FlipHorizontal) {
    GpuImage output;
    Geometric::flip(testImage, output, FlipDirection::Horizontal);
    cudaDeviceSynchronize();
    
    EXPECT_EQ(output.width, testImage.width);
    EXPECT_EQ(output.height, testImage.height);
    
    // 验证翻转
    std::vector<unsigned char> result(64);
    cudaMemcpy(result.data(), output.buffer.data(), 64, cudaMemcpyDeviceToHost);
    
    std::vector<unsigned char> original(64);
    cudaMemcpy(original.data(), testImage.buffer.data(), 64, cudaMemcpyDeviceToHost);
    
    // 第一行第一个像素应该等于原图第一行最后一个像素
    EXPECT_EQ(result[0], original[7]);
}

TEST_F(GeometricTest, FlipVertical) {
    GpuImage output;
    Geometric::flip(testImage, output, FlipDirection::Vertical);
    cudaDeviceSynchronize();
    
    EXPECT_EQ(output.width, testImage.width);
    EXPECT_EQ(output.height, testImage.height);
}

TEST_F(GeometricTest, Crop) {
    GpuImage output;
    Geometric::crop(testImage, output, 2, 2, 4, 4);
    cudaDeviceSynchronize();
    
    EXPECT_EQ(output.width, 4);
    EXPECT_EQ(output.height, 4);
}

TEST_F(GeometricTest, Pad) {
    GpuImage output;
    Geometric::pad(testImage, output, 2, 2, 2, 2, 128);
    cudaDeviceSynchronize();
    
    EXPECT_EQ(output.width, 12);
    EXPECT_EQ(output.height, 12);
    
    // 验证填充值
    std::vector<unsigned char> result(144);
    cudaMemcpy(result.data(), output.buffer.data(), 144, cudaMemcpyDeviceToHost);
    EXPECT_EQ(result[0], 128);  // 左上角应该是填充值
}

TEST_F(GeometricTest, Rotate) {
    GpuImage output;
    Geometric::rotate(testImage, output, 45.0f);
    cudaDeviceSynchronize();
    
    EXPECT_TRUE(output.isValid());
    // 45度旋转后尺寸会变大
    EXPECT_GT(output.width, testImage.width);
}

TEST_F(GeometricTest, AffineTransform_Identity) {
    GpuImage output;
    float matrix[6] = {1, 0, 0, 0, 1, 0};  // 单位矩阵
    Geometric::affineTransform(testImage, output, matrix, 8, 8);
    cudaDeviceSynchronize();
    
    EXPECT_EQ(output.width, 8);
    EXPECT_EQ(output.height, 8);
}

TEST_F(GeometricTest, AffineTransform_Scale) {
    GpuImage output;
    float matrix[6] = {2, 0, 0, 0, 2, 0};  // 2x 缩放
    Geometric::affineTransform(testImage, output, matrix, 16, 16);
    cudaDeviceSynchronize();
    
    EXPECT_EQ(output.width, 16);
    EXPECT_EQ(output.height, 16);
}

TEST_F(GeometricTest, InvalidInput) {
    GpuImage invalid;
    GpuImage output;
    
    EXPECT_THROW(Geometric::rotate(invalid, output, 45.0f), std::invalid_argument);
    EXPECT_THROW(Geometric::flip(invalid, output, FlipDirection::Horizontal), std::invalid_argument);
    EXPECT_THROW(Geometric::crop(invalid, output, 0, 0, 4, 4), std::invalid_argument);
}

TEST_F(GeometricTest, InvalidCropDimensions) {
    GpuImage output;
    EXPECT_THROW(Geometric::crop(testImage, output, 0, 0, 0, 0), std::invalid_argument);
    EXPECT_THROW(Geometric::crop(testImage, output, 0, 0, -1, 4), std::invalid_argument);
}

TEST_F(GeometricTest, InvalidPadding) {
    GpuImage output;
    EXPECT_THROW(Geometric::pad(testImage, output, -1, 0, 0, 0), std::invalid_argument);
}

TEST_F(GeometricTest, NullMatrix) {
    GpuImage output;
    EXPECT_THROW(Geometric::affineTransform(testImage, output, nullptr, 8, 8), std::invalid_argument);
}
