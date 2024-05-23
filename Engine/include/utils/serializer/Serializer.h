#pragma once

#include <string>

namespace Serializer
{
	void SaveSceneToFile(const std::string& path, const std::string& filename);
	void LoadSceneFromFile(const std::string& path, const std::string& filename);
};