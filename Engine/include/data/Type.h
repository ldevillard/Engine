#pragma once

#include <map>
#include <string>

class Component;

namespace Type
{
    inline std::map<std::string, Component* (*)()> componentTypeMap;

#define REGISTER_COMPONENT_TYPE(ComponentType) \
    namespace { \
        Component* create_##ComponentType() { return new ComponentType(); } \
        bool registered_##ComponentType = []() { \
            Type::componentTypeMap[#ComponentType] = &create_##ComponentType; \
            return true; \
        }(); \
    }
}