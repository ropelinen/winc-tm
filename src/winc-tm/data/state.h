#pragma once

#include "data/menu_state.h"

namespace winc
{
	struct create_tournament_data;
	struct tournament_data;

	struct state
	{
		uint16_t resolution_x = 0;
		uint16_t resolution_y = 0;
		menu_state menu_state = invalid;
		create_tournament_data *new_tournament_data = nullptr;
		tournament_data *tournament_data = nullptr;
		bool exit_requested = false;
	};
}