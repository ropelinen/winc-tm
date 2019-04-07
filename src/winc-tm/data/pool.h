#pragma once

#include "data/bout.h"

#include <vector>

namespace winc
{
	struct pool
	{
		std::vector<uint16_t> fencers;
		std::vector<bout> bouts;
	};
}