﻿// Copyright 2021 ShenMian
// License(Apache-2.0)

#pragma once

#include <memory>
#include <string>
#include <unordered_map>

/** @addtogroup shader
 *  @{
 */

 /**
  * @brief 阶段着色器.
  */
class Shader
{
public:
	enum class Stage
	{
		Vertex,
		Fragment,
		Pixel = Fragment,
		Geometry,
		Compute
	};

	/**
	 * @brief 创建着色器.
	 *
	 * @param name  着色器名称.
	 * @param stage 着色器阶段.
	 */
	static std::shared_ptr<Shader> create(const std::string& name, Stage stage);

	/**
	 * @brief 构造函数.
	 *
	 * @param name  着色器名称.
	 * @param stage 着色器阶段.
	 */
	Shader(const std::string& name, Stage stage);

	/**
	 * @brief 获取名称.
	 */
	const std::string& getName() const;

	/**
	 * @brief 获取阶段.
	 */
	Stage getStage() const;

protected:
	virtual ~Shader() = default;

	std::string name;
	Stage       stage;

	static std::unordered_map<Shader::Stage, const char*> extension;
};

/** @}*/
