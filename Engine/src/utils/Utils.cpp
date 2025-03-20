#include "utils/Utils.h"

#include <algorithm>

namespace Utils
{
	std::string& GetSingleSlashPath(std::string& path)
	{
		std::replace(path.begin(), path.end(), '\\', '/');
		return path;
	}
}
