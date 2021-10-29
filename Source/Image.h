﻿// Copyright 2021 SMS
// License(Apache-2.0)

#pragma once

#include <Math/Vector2.hpp>
#include <cstdint>
#include <filesystem>
#include <vector>

class Image
{
public:
  /**
   * @brief 构造函数, 从文件载入图像.
   * 
   * @param path 图像文件地址.
   */
  Image(const std::filesystem::path& path);

  /**
   * @brief 从文件载入图像.
   *
   * @param path 图像文件地址.
   */
  void loadFromFile(const std::filesystem::path& path);

  /**
   * @brief 导出图像到文件.
   *
   * @param path 图像文件地址.
   */
  void saveToFile(const std::filesystem::path& path) const;

  /**
   * @brief 获取图像数据.
   */
  uint8_t* getData();

  /**
   * @brief 获取图像数据.
   */
  const uint8_t* getData() const;

  /**
   * @brief 获取图像分辨率.
   */
  Vector2i getSize() const;

  /**
   * @brief 获取通道数.
   */
  int getChannelCount() const;

private:
	std::vector<uint8_t> data;
	Vector2i             size;
	int                  channels = 0;
};