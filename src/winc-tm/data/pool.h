#pragma once

#include "data/bout.h"

#pragma warning(push)
#pragma warning(disable : 4365)
#pragma warning(disable : 4626)
#include <list>
#include <vector>
#pragma warning(pop) 

namespace winc
{
	struct pool
	{
		std::vector<uint16_t> fencers;
		std::vector<bout> bouts;
	};
}