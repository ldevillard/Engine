#include "utils/serializer/Serializer.h"

#include "system/entity/EntityManager.h"
#include "utils/serializer/json/json.hpp"

// debug
#include <iostream>

#pragma region Public Methods

void Serializer::SaveSceneToFile(const std::string& path, const std::string& filename)
{
	path;

	nlohmann::ordered_json json;
	
	json[filename]["Scene"] = EntityManager::Get().Serialize();

	std::cout << json.dump(4) << std::endl;
}

//void Serializer::LoadSceneFromFile(const std::string& path)
//{
//	path;
//}

#pragma endregion