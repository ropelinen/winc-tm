#include "precompiled.h"

#include "run_tournament_menu.h"

#include "3rd_party/imgui/imgui.h"
#include "core/file_io.h"
#include "data/state.h"
#include "data/tournament_data.h"

namespace winc
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
			create_elimination_pools(state_data);
	}

	void handle_elimination_pools(state &state_data)
	{
		std::vector<fencer> &fencers = state_data.tournament_data->fencers;
		for (size_t pool_index = 0; pool_index < state_data.tournament_data->elimination_pools.size(); ++pool_index)
		{
			/* This is ugly but I'd say we are safe with 9999 pools */
			char pool_num_str[4];
			sprintf(pool_num_str, "%u", (uint32_t)pool_index + 1);
			char pool_name[10] = "Pool ";
			strcat(pool_name, pool_num_str);
			if (ImGui::CollapsingHeader(pool_name))
			{
				std::vector<uint16_t> &pool = state_data.tournament_data->elimination_pools[pool_index].fencers;
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
		{

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

		if (tournament.elimination_pools.empty())
			handle_total_pool_results(state_data);
		else
			handle_elimination_pools(state_data);

		ImGui::End();

		char pool_window_title[] = "No bouts available";

		ImVec2 matches_window_size(state_data.resolution_x * 0.9f, state_data.resolution_y * 0.15f);
		ImGui::SetNextWindowSize(matches_window_size, ImGuiCond_Always);
		ImGui::SetNextWindowPos(ImVec2(state_data.resolution_x / 2.0f, state_data.resolution_y * 0.925f), ImGuiCond_Always, ImVec2(0.5f, 1.0f));

		ImGui::Begin(pool_window_title, nullptr, window_flags);


		ImGui::End();
	}
}