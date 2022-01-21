﻿// Copyright 2021 ShenMian
// License(Apache-2.0)

#pragma once

#include "../VKInstance.h"
#include <string_view>
#include <vector>

class InstanceBuilder final
{
public:
	InstanceBuilder();

	VKInstance build();

	/**
	 * @brief 设置引擎名称. (可选)
	 *
	 * @param name 引擎名称.
	 */
	InstanceBuilder& setEngineName(std::string_view name);

	/**
	 * @brief 设置引擎版本. (可选)
	 *
	 * @param name 引擎版本.
	 */
	InstanceBuilder& setEngineVersion(uint32_t major, uint32_t minor, uint32_t patch);

	/**
	 * @brief 设置应用程序名称. (可选)
	 *
	 * @param name 应用程序名称.
	 */
	InstanceBuilder& setAppName(std::string_view name);

	/**
	 * @brief 设置应用程序版本. (可选)
	 *
	 * @param name 应用程序版本.
	 */
	InstanceBuilder& setAppVersion(uint32_t major, uint32_t minor, uint32_t patch);

	/**
	 * @brief 启用 Layer.
	 *
	 * @param name Layer 名称.
	 */
	InstanceBuilder& enableLayer(std::string_view name);

	/**
	 * @brief 启用拓展.
	 *
	 * @param name 拓展名称.
	 */
	InstanceBuilder& enableExtension(std::string_view name);

private:
	bool isLayerAvailable(std::string_view name);
	bool isExtensionAvailable(std::string_view name);

	VkApplicationInfo appInfo = {};
	VkInstanceCreateInfo instanceInfo = {};

	std::vector<const char*> enabledLayers;
	std::vector<const char*> enabledExtensions;

	std::vector<VkLayerProperties> availableLayers;
	std::vector<VkExtensionProperties> availableExtensions;
};
