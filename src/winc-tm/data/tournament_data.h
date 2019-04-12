#pragma once

#include "core/include_std_containers.h"
#include "data/fencer.h"
#include "data/pool.h"

namespace winc
{
	struct tournament_data
	{
		static const uint8_t max_tournament_name_len = 128;
		char tournament_name[max_tournament_name_len];
		uint16_t elimination_pool_count;
		uint16_t max_fencers_in_pool;
		uint16_t fencers_in_finals;

		std::vector<fencer> fencers;
		std::vector<pool> pools;
		std::vector<pool> elimination_pools;
		/* This is a vactor simply so that I don't have to change some gui code (yes, I'm in a hurry) */
		std::vector<pool> final_pool;
	};
}