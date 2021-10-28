﻿// Copyright 2021 SMS
// License(Apache-2.0)

#pragma once

#include <Math/Vector2.hpp>
#include <filesystem>

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
   * @brief 析构函数.
   */
  virtual ~Image();

  /**
   * @brief 获取图像数据.
   */
  unsigned char* getData();

  /**
   * @brief 获取图像数据.
   */
  unsigned char* getData() const;

  /**
   * @brief 获取图像分辨率.
   */
  Vector2i getSize() const;

  /**
   * @brief 获取通道数.
   */
  int getChannelCount() const;

private:
	unsigned char* data;
	Vector2i       size;
	int            channels;
};