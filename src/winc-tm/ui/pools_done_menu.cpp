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
		void create_elimination_pools(state &state_data)
		{
			uint16_t advancing_fencer_count = uint16_t(state_data.tournament_data->elimination_pool_count * state_data.tournament_data->max_fencers_in_pool);
			uint16_t pool_count = state_data.tournament_data->elimination_pool_count;

			state_data.tournament_data->elimination_pools.clear();
			state_data.tournament_data->elimination_pools.resize(pool_count);

			for (size_t fencer_index = 0; fencer_index < advancing_fencer_count; ++fencer_index)
				state_data.tournament_data->elimination_pools[fencer_index % pool_count].fencers.push_back(state_data.pool_results[fencer_index].id);
		}

		void handle_create_bouts_for_pools(tournament_data & data)
		{
			uint16_t bout_id = 0;
			const uint16_t invalid_fencer_id = 0xFFFF;
			for (size_t pool_index = 0; pool_index < data.elimination_pools.size(); ++pool_index)
			{
				pool &pl = data.elimination_pools[pool_index];
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
						bt.id = 1000u + bout_id;
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
	}

	void handle_total_pool_results(state &state_data)
	{
		uint16_t advancing_fencer_count = uint16_t(state_data.tournament_data->elimination_pool_count * state_data.tournament_data->max_fencers_in_pool);

		ImGui::Text("Results");
		ImGui::Columns(6, "mycolumns");
		ImGui::Separator();
		ImGui::Text("# Name");
		ImGui::NextColumn();
		ImGui::Text("Club");
		ImGui::NextColumn();
		ImGui::Text("Match point index");
		ImGui::NextColumn();
		ImGui::Text("Victory index");
		ImGui::NextColumn();
		ImGui::Text("Q3 ratio");
		ImGui::NextColumn();
		ImGui::Text("Clean ratio");
		ImGui::NextColumn();
		/* We could add extra info if there is space etc */
		ImGui::Separator();

		for (size_t result_index = 0; result_index < state_data.pool_results.size(); ++result_index)
		{
			fencer *fenc = nullptr;
			fencer_results &result = state_data.pool_results[result_index];
			for (size_t fencer_index = 0; fencer_index < state_data.tournament_data->fencers.size(); ++fencer_index)
			{
				if (result.id != state_data.tournament_data->fencers[fencer_index].id)
					continue;

				fenc = &state_data.tournament_data->fencers[fencer_index];
				break;
			}

			if (!fenc)
				continue;

			ImGui::Text("%u %s", result_index + 1, fenc->name);
			ImGui::NextColumn();
			ImGui::Text("%s", fenc->club);
			ImGui::NextColumn();
			ImGui::Text("%f", get_fencer_matchpoint_index(result));
			ImGui::NextColumn();
			ImGui::Text("%f", get_fencer_victory_index(result));
			ImGui::NextColumn();
			ImGui::Text("%f", get_fencer_q3_ratio(result));
			ImGui::NextColumn();
			ImGui::Text("%f", get_fencer_clean_ratio(result));
			ImGui::NextColumn();

			if (result_index == (uint16_t)(advancing_fencer_count - 1))
			{
				ImGui::Separator();
				ImGui::Separator();
			}
		}

		ImGui::Columns(1);
		ImGui::Separator();

		if (ImGui::Button("Back"))
			state_data.menu_state = run_tournament;

		ImGui::SameLine();

		if (ImGui::Button("Create elimination pools"))
		{
			create_elimination_pools(state_data);
			handle_create_bouts_for_pools(*state_data.tournament_data);
			write_tournament_data(*state_data.tournament_data);
			state_data.menu_state = run_tournament;
		}
	}

	void handle_pools_done_menu(state &state_data)
	{
		if (!state_data.tournament_data)
			return;

		tournament_data &tournament = *state_data.tournament_data;

		ImVec2 main_window_size(state_data.resolution_x * 0.9f, state_data.resolution_y * 0.75f);
		ImGui::SetNextWindowSize(main_window_size, ImGuiCond_Always);
		ImGui::SetNextWindowPos(ImVec2(state_data.resolution_x / 2.0f, state_data.resolution_y * 0.025f), ImGuiCond_Always, ImVec2(0.5f, 0.0f));

		static const ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		/* We could actually have 'Tournament Name - State' as the window name */
		ImGui::Begin(tournament.tournament_name, nullptr, window_flags);

		handle_total_pool_results(state_data);

		ImGui::End();

		char pool_window_title[] = "No bouts available";

		ImVec2 matches_window_size(state_data.resolution_x * 0.9f, state_data.resolution_y * 0.15f);
		ImGui::SetNextWindowSize(matches_window_size, ImGuiCond_Always);
		ImGui::SetNextWindowPos(ImVec2(state_data.resolution_x / 2.0f, state_data.resolution_y * 0.925f), ImGuiCond_Always, ImVec2(0.5f, 1.0f));

		ImGui::Begin(pool_window_title, nullptr, window_flags);


		ImGui::End();
	}
}