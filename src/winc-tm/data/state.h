#pragma once

#include "data/create_tournament_data.h"
#include "data/menu_state.h"

namespace winc
{
	struct state
	{
		uint16_t resolution_x = 0;
		uint16_t resolution_y = 0;
		menu_state menu_state = invalid;
		create_tournament_data *new_tournament_data = nullptr;
		bool exit_requested = false;
	};
}