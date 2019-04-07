#pragma once

#include "data/fencer.h"
#include "data/pool.h"

#pragma warning(push)
#pragma warning(disable : 4365)
#pragma warning(disable : 4571)
#pragma warning(disable : 4626)
#include <vector>
#pragma warning(pop) 

namespace winc
{
	struct tournament_data
	{
		static const uint8_t max_tournament_name_len = 128;
		char tournament_name[max_tournament_name_len];
		uint16_t elimination_pool_count;
		uint16_t fencers_in_finals;

		std::vector<fencer> fencers;
		std::vector<pool> pools;
		std::vector<pool> elimination_pools;
		pool final_pool;
	};
}