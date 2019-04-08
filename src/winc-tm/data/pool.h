#pragma once

#include "core/include_std_containers.h"
#include "data/bout.h"

namespace winc
{
	struct pool
	{
		std::vector<uint16_t> fencers;
		std::vector<bout> bouts;
	};
}