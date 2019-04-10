#include "precompiled.h"

#include "menu_handler.h"

#include "data/state.h"
#include "ui/create_pools_menu.h"
#include "ui/create_tournament_menu.h"
#include "ui/main_menu.h"
#include "ui/modify_bout_menu.h"
#include "ui/pool_results_menu.h"
#include "ui/run_tournament_menu.h"

namespace winc
{
	void handle_menus(state& state_data)
	{
		switch (state_data.menu_state)
		{
		case invalid:
			assert(0 && "We should never end up in an invalid menu state");
			break;

		case main_menu:
			handle_main_menu(state_data);
			break;

		case load_tournament:
			break;

		case create_tournament:
			handle_create_tournament_menu(state_data);
			break;

		case create_pools:
			handle_create_pools_menu(state_data);
			break;

		case run_tournament:
			handle_run_tournament_menu(state_data);
			break;

		case modify_bout:
			handle_modify_bout_menu(state_data);
			break;

		case pool_results:
			handle_pool_results_menu(state_data);
			break;
		
		}
	}
}