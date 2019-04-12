#include "precompiled.h"

#include "run_tournament_menu.h"

#include "3rd_party/imgui/imgui.h"
#include "core/file_io.h"
#include "data/state.h"
#include "data/tournament_data.h"

namespace winc
{
	void handle_pool_results_menu(state & state_data)
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
		}

		ImGui::Columns(1);
		ImGui::Separator();

		if (ImGui::Button("Back"))
			state_data.menu_state = run_tournament;

		ImGui::SameLine();

		if (ImGui::Button("Write to file"))
			write_pool_results_to_file(state_data);

		ImGui::End();

		char pool_window_title[] = "No bouts available";

		ImVec2 matches_window_size(state_data.resolution_x * 0.9f, state_data.resolution_y * 0.15f);
		ImGui::SetNextWindowSize(matches_window_size, ImGuiCond_Always);
		ImGui::SetNextWindowPos(ImVec2(state_data.resolution_x / 2.0f, state_data.resolution_y * 0.925f), ImGuiCond_Always, ImVec2(0.5f, 1.0f));

		ImGui::Begin(pool_window_title, nullptr, window_flags);


		ImGui::End();
	}
}