#pragma once

#include <string>

class Serializer
{
public:
	static void SaveSceneToFile(const std::string& path, const std::string& filename);
	//static void LoadSceneFromFile(const std::string& path);
};