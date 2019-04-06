#pragma once

#include "data/fencer.h"

#pragma warning(push)
#pragma warning(disable : 4365)
#pragma warning(disable : 4571)
#pragma warning(disable : 4625)
#pragma warning(disable : 4626)
#include <list>
#include <vector>
#pragma warning(pop) 


namespace winc
{
	struct create_tournament_data
	{
		uint16_t max_pool_count;
		uint16_t max_fencers_in_pool;
		uint16_t elimination_pool_count;
		uint16_t final_pool_fencer_count;
		static const uint8_t max_tournament_name_len = 128;
		char tournament_name[max_tournament_name_len];
		static const uint8_t max_error_msg_len = 255;
		char error_message[max_error_msg_len];
		std::list<fencer> fencers;
		std::vector<std::list<uint16_t>> pools;
	};
}