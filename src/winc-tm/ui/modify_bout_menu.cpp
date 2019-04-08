#include "precompiled.h"

#include "run_tournament_menu.h"

#include "3rd_party/imgui/imgui.h"
#include "data/state.h"
#include "data/tournament_data.h"

namespace winc
{
	namespace
	{
		void handle_main_window(state &state_data)
		{
			if (ImGui::Button("Back"))
			{
				state_data.menu_state = run_tournament;
			}
		}
	}


	void handle_modify_bout_menu(state &state_data)
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

		handle_main_window(state_data);

		ImGui::End();

		char pool_window_title[] = "Running bout information";

		ImVec2 matches_window_size(state_data.resolution_x * 0.9f, state_data.resolution_y * 0.15f);
		ImGui::SetNextWindowSize(matches_window_size, ImGuiCond_Always);
		ImGui::SetNextWindowPos(ImVec2(state_data.resolution_x / 2.0f, state_data.resolution_y * 0.925f), ImGuiCond_Always, ImVec2(0.5f, 1.0f));

		ImGui::Begin(pool_window_title, nullptr, window_flags);

		/* We want identical next bout etc view here compared to run tournament menu */

		ImGui::End();
	}
}