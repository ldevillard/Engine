#pragma once

#include "utils/serializer/SerializerUtils.h"

namespace Serializer
{
	nlohmann::ordered_json Serialize(glm::vec3 vec, Math::Vec3Format format)
	{
        nlohmann::ordered_json json;

        switch (format)
        {
            case Math::Vec3Format::XYZ:
                json = 
                {
                    {"x", vec.x},
                    {"y", vec.y},
                    {"z", vec.z}
                };
                break;
            case Math::Vec3Format::RGB:
                json = 
                {
                    {"r", vec.x},
                    {"g", vec.y},
                    {"b", vec.z}
                };
                break;
            default:
                throw std::invalid_argument("Unknown format");
        }

        return json;
	}
}