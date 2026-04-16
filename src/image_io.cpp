#include "gpu_image/image_io.hpp"
#include <algorithm>
#include <cstring>
#include <fstream>
#include <stdexcept>

// stb_image 由 CMake FetchContent 提供
#ifdef GPU_IMAGE_HAS_STB

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

namespace gpu_image {

HostImage ImageIO::loadFromFile(const std::string& filepath) {
  int width, height, channels;
  unsigned char* data =
      stbi_load(filepath.c_str(), &width, &height, &channels, 0);

  if (data == nullptr) {
    throw std::runtime_error("Failed to load image: " + filepath + " (" +
                             stbi_failure_reason() + ")");
  }

  HostImage image;
  image.width = width;
  image.height = height;
  image.channels = channels;
  image.data.assign(data,
                    data + static_cast<size_t>(width) * height * channels);

  stbi_image_free(data);
  return image;
}

bool ImageIO::saveToFile(const HostImage& image, const std::string& filepath) {
  if (!image.isValid()) {
    return false;
  }

  // 从文件扩展名推断格式
  std::string ext = filepath.substr(filepath.find_last_of('.') + 1);
  std::transform(ext.begin(), ext.end(), ext.begin(),
                 [](unsigned char c) { return std::tolower(c); });

  int result = 0;
  if (ext == "png") {
    result = stbi_write_png(filepath.c_str(), image.width, image.height,
                            image.channels, image.data.data(),
                            image.width * image.channels);
  } else if (ext == "jpg" || ext == "jpeg") {
    result = stbi_write_jpg(filepath.c_str(), image.width, image.height,
                            image.channels, image.data.data(), 90);
  } else if (ext == "bmp") {
    result = stbi_write_bmp(filepath.c_str(), image.width, image.height,
                            image.channels, image.data.data());
  } else if (ext == "tga") {
    result = stbi_write_tga(filepath.c_str(), image.width, image.height,
                            image.channels, image.data.data());
  } else {
    return false;
  }

  return result != 0;
}

HostImage ImageIO::loadFromMemory(const unsigned char* data, size_t size) {
  if (data == nullptr || size == 0) {
    throw std::invalid_argument("Invalid memory buffer");
  }

  int width, height, channels;
  unsigned char* pixels = stbi_load_from_memory(data, static_cast<int>(size),
                                                &width, &height, &channels, 0);

  if (pixels == nullptr) {
    throw std::runtime_error(
        std::string("Failed to decode image from memory: ") +
        stbi_failure_reason());
  }

  HostImage image;
  image.width = width;
  image.height = height;
  image.channels = channels;
  image.data.assign(pixels,
                    pixels + static_cast<size_t>(width) * height * channels);

  stbi_image_free(pixels);
  return image;
}

// stb_image_write 回调，用于写入 std::vector
static void stbiWriteCallback(void* context, void* data, int size) {
  auto* vec = static_cast<std::vector<unsigned char>*>(context);
  auto* bytes = static_cast<unsigned char*>(data);
  vec->insert(vec->end(), bytes, bytes + size);
}

std::vector<unsigned char> ImageIO::encodeToMemory(const HostImage& image,
                                                   const std::string& format) {
  if (!image.isValid()) {
    throw std::invalid_argument("Invalid image");
  }

  std::vector<unsigned char> result;
  std::string fmt = format;
  std::transform(fmt.begin(), fmt.end(), fmt.begin(),
                 [](unsigned char c) { return std::tolower(c); });

  int ok = 0;
  if (fmt == "png") {
    ok = stbi_write_png_to_func(stbiWriteCallback, &result, image.width,
                                image.height, image.channels, image.data.data(),
                                image.width * image.channels);
  } else if (fmt == "jpg" || fmt == "jpeg") {
    ok = stbi_write_jpg_to_func(stbiWriteCallback, &result, image.width,
                                image.height, image.channels, image.data.data(),
                                90);
  } else if (fmt == "bmp") {
    ok =
        stbi_write_bmp_to_func(stbiWriteCallback, &result, image.width,
                               image.height, image.channels, image.data.data());
  } else if (fmt == "tga") {
    ok =
        stbi_write_tga_to_func(stbiWriteCallback, &result, image.width,
                               image.height, image.channels, image.data.data());
  } else {
    throw std::invalid_argument("Unsupported format: " + format);
  }

  if (!ok) {
    throw std::runtime_error("Failed to encode image to " + format);
  }

  return result;
}

std::vector<std::string> ImageIO::getSupportedFormats() {
  return {"png", "jpg", "jpeg", "bmp", "tga", "psd", "gif", "hdr", "pnm"};
}

bool ImageIO::isFormatSupported(const std::string& filepath) {
  if (filepath.empty())
    return false;

  auto dotPos = filepath.find_last_of('.');
  if (dotPos == std::string::npos)
    return false;

  std::string ext = filepath.substr(dotPos + 1);
  std::transform(ext.begin(), ext.end(), ext.begin(),
                 [](unsigned char c) { return std::tolower(c); });

  auto formats = getSupportedFormats();
  return std::find(formats.begin(), formats.end(), ext) != formats.end();
}

} // namespace gpu_image

#else // !GPU_IMAGE_HAS_STB

// 无 stb 时的空实现，所有函数抛出异常
namespace gpu_image {

HostImage ImageIO::loadFromFile(const std::string&) {
  throw std::runtime_error(
      "ImageIO requires stb_image. Build with -DGPU_IMAGE_ENABLE_IO=ON");
}

bool ImageIO::saveToFile(const HostImage&, const std::string&) {
  throw std::runtime_error(
      "ImageIO requires stb_image_write. Build with -DGPU_IMAGE_ENABLE_IO=ON");
}

HostImage ImageIO::loadFromMemory(const unsigned char*, size_t) {
  throw std::runtime_error(
      "ImageIO requires stb_image. Build with -DGPU_IMAGE_ENABLE_IO=ON");
}

std::vector<unsigned char> ImageIO::encodeToMemory(const HostImage&,
                                                   const std::string&) {
  throw std::runtime_error(
      "ImageIO requires stb_image_write. Build with -DGPU_IMAGE_ENABLE_IO=ON");
}

std::vector<std::string> ImageIO::getSupportedFormats() { return {}; }
bool ImageIO::isFormatSupported(const std::string&) { return false; }

} // namespace gpu_image

#endif // GPU_IMAGE_HAS_STB
