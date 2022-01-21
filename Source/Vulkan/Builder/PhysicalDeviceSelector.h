﻿// Copyright 2021 ShenMian
// License(Apache-2.0)

#pragma once

#include "../VKInstance.h"
#include "../VKPhysicalDevice.h"
#include <vector>

class VKPhysicalDevice;

class PhysicalDeviceSelector final
{
public:
	PhysicalDeviceSelector(const VKInstance& instance, VkSurfaceKHR surface);

	VKPhysicalDevice select();

	PhysicalDeviceSelector& requireGraphicsQueue();

	PhysicalDeviceSelector& requireComputeQueue();

	PhysicalDeviceSelector& requireTransferQueue();

	PhysicalDeviceSelector& requirePresentQueue();

private:
	struct Info
	{
		bool requireGraphicsQueue = false;
		bool requireComputeQueue = false;
		bool requireTransferQueue = false;
		bool requirePresentQueue = false;
	};

	std::vector<VKPhysicalDevice> devices;
	Info info;
};
