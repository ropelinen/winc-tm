#include "precompiled.h"

#include "run_tournament_menu.h"

#include "3rd_party/imgui/imgui.h"
#include "core/file_io.h"
#include "data/state.h"
#include "data/tournament_data.h"

namespace winc
{
	namespace
	{
		enum tournament_state
		{
			setup,
			pools,
			elims,
			finals,
		};

		tournament_state get_current_state(tournament_data &data)
		{
			for (size_t i = 0; i < data.elimination_pools.size(); ++i)
			{
				if (data.elimination_pools[i].bouts.empty())
					continue;

				return elims;
				break;
			}

			for (size_t i = 0; i < data.pools.size(); ++i)
			{
				if (data.pools[i].bouts.empty())
					continue;

				return pools;
				break;
			}
			
			return setup;			
		}
	}

	void sort_pool_result(std::vector<fencer_results> &results)
	{
		if (results.empty())
			return;
		
		std::list<fencer_results> temp_results;
		temp_results.push_back(results[0]);

		for (size_t result_index = 1; result_index < results.size(); ++result_index)
		{
			bool inserted = false;
			fencer_results &result = results[result_index];
			for (std::list<fencer_results>::iterator it = temp_results.begin(); it != temp_results.end(); ++it)
			{
				/* Matchpoint index */
				if (get_fencer_matchpoint_index(result) > get_fencer_matchpoint_index(*it))
				{
					temp_results.insert(it, result);
					inserted = true;
					break;
				}
				
				if (get_fencer_matchpoint_index(result) != get_fencer_matchpoint_index(*it))
					continue;

				/* Victory index */
				if (get_fencer_victory_index(result) > get_fencer_victory_index(*it))
				{
					temp_results.insert(it, result);
					inserted = true;
					break;
				}

				if (get_fencer_victory_index(result) != get_fencer_victory_index(*it))
					continue;

				/* Q3 ration */
				if (get_fencer_q3_ratio(result) > get_fencer_q3_ratio(*it))
				{
					temp_results.insert(it, result);
					inserted = true;
					break;
				}

				if (get_fencer_q3_ratio(result) != get_fencer_q3_ratio(*it))
					continue;

				/* Clean ratio */
				if (get_fencer_clean_ratio(result) > get_fencer_clean_ratio(*it))
				{
					temp_results.insert(it, result);
					inserted = true;
					break;
				}
			}

			if (!inserted)
				temp_results.push_back(result);
		}

		assert(temp_results.size() == results.size());

		results.clear();
		results.reserve(temp_results.size());

		for (std::list<fencer_results>::iterator it = temp_results.begin(); it != temp_results.end(); ++it)
			results.push_back(*it);
	}

	void handle_create_bouts_for_pools(tournament_data &data)
	{
		uint16_t bout_id = 0;
		const uint16_t invalid_fencer_id = 0xFFFF;
		for (size_t pool_index = 0; pool_index < data.pools.size(); ++pool_index)
		{
			pool &pl = data.pools[pool_index];
			pl.bouts.clear();
			bool uneven_fencer_count = pl.fencers.size() % 2 != 0;
			size_t fencer_count = pl.fencers.size();
			if (uneven_fencer_count)
				++fencer_count;

			uint16_t *fencers = new uint16_t[fencer_count];

			if (uneven_fencer_count)
				fencers[fencer_count - 1] = invalid_fencer_id;

			for (size_t fencer_index = 0; fencer_index < pl.fencers.size(); ++fencer_index)
				fencers[fencer_index] = pl.fencers[fencer_index];

			for (size_t round = 0; round < fencer_count - 1; ++round)
			{
				for (size_t bout_index = 0; bout_index < fencer_count / 2; ++bout_index)
				{
					uint16_t fencer_a = fencers[bout_index];
					uint16_t fencer_b = fencers[fencer_count - 1 - bout_index];
					if (fencer_a == invalid_fencer_id || fencer_b == invalid_fencer_id)
						continue;

					bout bt;
					bt.id = bout_id;
					if (fencer_a < fencer_b)
					{
						bt.blue_fencer = fencer_a;
						bt.red_fencer = fencer_b;
					}
					else
					{
						bt.blue_fencer = fencer_b;
						bt.red_fencer = fencer_a;
					}

					pl.bouts.push_back(bt);
					++bout_id;
				}

				/* Advance the fencers */
				uint16_t last_fencer_id = fencers[fencer_count - 1];
				for (size_t move_index = fencer_count - 1; move_index > 1; --move_index)
					fencers[move_index] = fencers[move_index - 1];
				
				fencers[1] = last_fencer_id;
			}

			delete[] fencers;
		}

		write_tournament_data(data);
	}

	void handle_main_window_setup_state(tournament_data &data)
	{
		std::vector<fencer> &fencers = data.fencers;
		for (size_t pool_index = 0; pool_index < data.pools.size(); ++pool_index)
		{
			/* This is ugly but I'd say we are safe with 9999 pools */
			char pool_num_str[4];
			sprintf(pool_num_str, "%u", (uint32_t)pool_index + 1);
			char pool_name[10] = "Pool ";
			strcat(pool_name, pool_num_str);
			if (ImGui::CollapsingHeader(pool_name))
			{
				std::vector<uint16_t> &pool = data.pools[pool_index].fencers;
				for (size_t pool_member_index = 0; pool_member_index < pool.size(); ++pool_member_index)
				{
					for (size_t fencer_index = 0; fencer_index < fencers.size(); ++fencer_index)
					{
						if (pool[pool_member_index] != fencers[fencer_index].id)
							continue;

						if (ImGui::TreeNode((void *)(intptr_t)pool_member_index, "%s, %s (%u)", fencers[fencer_index].name, fencers[fencer_index].club, fencers[fencer_index].hema_rating))
						{
							ImGui::Text("Club: %s", fencers[fencer_index].club);
							ImGui::Text("Rating: %u", fencers[fencer_index].hema_rating);

							ImGui::TreePop();
						}
					}
				}
			}
		}

		if (ImGui::Button("Create bouts"))
			handle_create_bouts_for_pools(data);
	}

	void handle_main_window_pools_state(state &state_data)
	{
		tournament_data &data = *state_data.tournament_data;
		std::vector<fencer> &fencers = data.fencers;
		for (size_t pool_index = 0; pool_index < data.pools.size(); ++pool_index)
		{
			/* This is ugly but I'd say we are safe with 9999 pools */
			char pool_num_str[4];
			sprintf(pool_num_str, "%u", (uint32_t)pool_index + 1);
			char pool_name[10] = "Pool ";
			strcat(pool_name, pool_num_str);
			if (ImGui::CollapsingHeader(pool_name))
			{
				pool &pl = data.pools[pool_index];
				bool all_matches_fought = true;
				for (size_t bout_index = 0; bout_index < pl.bouts.size(); ++bout_index)
				{
					bout &bt = pl.bouts[bout_index];
					fencer *blue = nullptr;
					fencer *red = nullptr;

					if (bt.exchanges.empty())
						all_matches_fought = false;

					for (size_t fencer_index = 0; fencer_index < fencers.size(); ++fencer_index)
					{
						uint16_t fencer_id = fencers[fencer_index].id;
						if (fencer_id == bt.blue_fencer)
							blue = &fencers[fencer_index];
						else if (fencer_id == bt.red_fencer)
							red = &fencers[fencer_index];

						if (blue && red)
							break;
					}

					if (!blue || !red)
						continue;

					if (ImGui::TreeNode((void *)(intptr_t)bt.id, "%s (%s) - %s (%s)", blue->name, blue->club, red->name, red->club))
					{	
						if (bt.exchanges.empty())
						{
							if (ImGui::Button("Input Bout"))
							{
								state_data.bout_to_modify = bt.id;
								state_data.menu_state = modify_bout;
							}
						}
						else
						{
							uint16_t blue_q1, blue_q2, blue_q3;
							uint16_t red_q1, red_q2, red_q3;
							get_blue_fencer_score(bt, blue_q1, blue_q2, blue_q3);
							get_red_fencer_score(bt, red_q1, red_q2, red_q3);

							if (get_suicidal_double_count(bt) >= 3)
								ImGui::Text("Double loss, three suicidal doubles");
							
							uint16_t warning_count = get_blue_warning_count(bt);
							if (warning_count >= 4)
								ImGui::Text("Blue: Warning loss");
							else if (warning_count >= 2)
								ImGui::Text("Blue: Penaly to highest quality");

							warning_count = get_red_warning_count(bt);
							if (warning_count >= 4)
								ImGui::Text("Red: Warning loss");
							else if (warning_count >= 2)
								ImGui::Text("Red: Penaly to highest quality");

							ImGui::Text("This score takes doubles in count, but doesn't take penalties to count");
							ImGui::Text("Blue: Q1(%u) Q2(%u) Q3(%u) - Red: Q1(%u) Q2(%u) Q3(%u)", blue_q1, blue_q2, blue_q3, red_q1, red_q2, red_q3);

							if (ImGui::Button("Modify Bout"))
							{
								state_data.bout_to_modify = bt.id;
								state_data.menu_state = modify_bout;
							}
						}
						ImGui::TreePop();
					}
				}

				if (all_matches_fought)
				{
					if (ImGui::Button("Show pool results"))
					{
						state_data.pool_results.clear();
						calculate_pool_results(pl, state_data.pool_results);
						sort_pool_result(state_data.pool_results);
						state_data.menu_state = pool_results;
					}
				}
			}			
		}

		/* Check for pools done condition */
		bool all_pools_done = true;
		for (size_t pool_index = 0; pool_index < data.pools.size(); ++pool_index)
		{
			pool &pl = data.pools[pool_index];
			for (size_t bout_index = 0; bout_index < pl.bouts.size(); ++bout_index)
			{
				if (pl.bouts[bout_index].exchanges.empty())
				{
					all_pools_done = false;
					break;
				}
			}

			if (!all_pools_done)
				break;
		}

		if (all_pools_done)
		{
			if (ImGui::Button("Finish pools"))
			{
				state_data.pool_results.clear();
				for (size_t pool_index = 0; pool_index < data.pools.size(); ++pool_index)
				{
					pool &pl = data.pools[pool_index];
					calculate_pool_results(pl, state_data.pool_results);
				}
				sort_pool_result(state_data.pool_results);
				state_data.menu_state = pools_done;
			}
		}
	}

	void handle_run_tournament_menu(state &state_data)
	{
		if (!state_data.tournament_data)
			return;

		tournament_data &tournament = *state_data.tournament_data;
		tournament_state current_state = get_current_state(tournament);

		ImVec2 main_window_size(state_data.resolution_x * 0.9f, state_data.resolution_y * 0.75f);
		ImGui::SetNextWindowSize(main_window_size, ImGuiCond_Always);
		ImGui::SetNextWindowPos(ImVec2(state_data.resolution_x / 2.0f, state_data.resolution_y * 0.025f), ImGuiCond_Always, ImVec2(0.5f, 0.0f));

		static const ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		/* We could actually have 'Tournament Name - State' as the window name */
		ImGui::Begin(tournament.tournament_name, nullptr, window_flags);

		switch (current_state)
		{
		case setup:
			handle_main_window_setup_state(tournament);
			break;

		case pools:
			handle_main_window_pools_state(state_data);
			break;

		case elims:
			break;

		case finals:
			break;
		}

		ImGui::End();

		char pool_window_title[] = "No bouts available";

		ImVec2 matches_window_size(state_data.resolution_x * 0.9f, state_data.resolution_y * 0.15f);
		ImGui::SetNextWindowSize(matches_window_size, ImGuiCond_Always);
		ImGui::SetNextWindowPos(ImVec2(state_data.resolution_x / 2.0f, state_data.resolution_y * 0.925f), ImGuiCond_Always, ImVec2(0.5f, 1.0f));

		ImGui::Begin(pool_window_title, nullptr, window_flags);

		switch (current_state)
		{
		case setup:
			/* Nothing to show at this point */
			break;

		case pools:
			break;

		case elims:
			break;

		case finals:
			break;
		}

		ImGui::End();
	}
}