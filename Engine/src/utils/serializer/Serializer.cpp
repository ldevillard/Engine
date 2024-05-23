#include "utils/serializer/Serializer.h"

#include "system/entity/EntityManager.h"
#include "utils/serializer/json/json.hpp"

// debug
#include <iostream>

namespace Serializer
{
	void SaveSceneToFile(const std::string& path, const std::string& filename)
	{
		nlohmann::ordered_json json;
		
		json[filename]["Scene"] = EntityManager::Get().Serialize();
	
        std::ofstream outputFile(path);

        if (outputFile.is_open()) 
        {
            outputFile << json.dump(4) << std::endl;
            outputFile.close();

            std::cout << "File saved successfully: " << path << std::endl;
        }
        else 
        {
            std::cerr << "Error: Unable to open file for writing: " << path << std::endl;
        }
	}
}

void Serializer::LoadSceneFromFile(const std::string& path, const std::string& filename)
{
    std::ifstream inputFile(path);

    if (inputFile.is_open())
    {
        nlohmann::ordered_json json;
        inputFile >> json;
        inputFile.close();

        EntityManager::Get().Deserialize(json[filename]["Scene"]);

        std::cout << "File loaded successfully: " << path << std::endl;
    }
    else
    {
        std::cerr << "Error: Unable to open file for reading: " << path << std::endl;
    }
}

