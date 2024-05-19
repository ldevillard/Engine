#pragma once

#include <maths/glm/glm.hpp>

#include "maths/Math.h"
#include "utils/serializer/json/json.hpp"

namespace Serializer
{
	nlohmann::ordered_json Serialize(glm::vec3 vec, Math::Vec3Format format = Math::Vec3Format::XYZ);
}