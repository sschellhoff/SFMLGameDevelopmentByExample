#pragma once
#include <string>
#include "config.h"

namespace Utils{
	inline std::string GetResourceDirectory(){
		return RESOURCE_PATH;
	}
}