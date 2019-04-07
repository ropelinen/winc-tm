#include "precompiled.h"

#include "main_menu.h"

#include "3rd_party/imgui/imgui.h"
#include "core/file_io.h"
#include "data/create_tournament_data.h"
#include "data/state.h"
#include "data/tournament_data.h"

namespace winc
{
	void initialize_new_tournament_data(state& state_data)
	{
		state_data.new_tournament_data = new create_tournament_data;
		state_data.new_tournament_data->tournament_name[0] = '\0';
		state_data.new_tournament_data->error_message[0] = '\0';
		state_data.new_tournament_data->max_pool_count = 0;
		state_data.new_tournament_data->max_fencers_in_pool = 0;
		state_data.new_tournament_data->elimination_pool_count = 0;
		state_data.new_tournament_data->final_pool_fencer_count = 0;
	}

	void handle_create_new_tournament_pressed(state& state_data)
	{
		initialize_new_tournament_data(state_data);
		state_data.menu_state = create_tournament;
	}

	void handle_load_tournament_pressed(state& state_data)
	{
		state_data.tournament_data = new tournament_data;
		if (read_tournament_data(*state_data.tournament_data))
			state_data.menu_state = run_tournament;
		else
		{
			delete state_data.tournament_data;
			state_data.tournament_data = nullptr;
		}
	}

	void handle_quit_pressed(state& state_data)
	{
		state_data.exit_requested = true;
	}

	void handle_main_menu(state& state_data)
	{
		/* I don't want to play with an ini file so lets do everything manually */
		ImVec2 window_size(550.0f, 300.0f);
		ImGui::SetNextWindowPos(ImVec2(state_data.resolution_x / 2.0f, state_data.resolution_y / 2.0f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
		ImGui::SetNextWindowSize(window_size, ImGuiCond_Always);

		static const ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		ImGui::Begin("Main menu window", nullptr, window_flags);

		if (ImGui::Button("Create New Tournament"))
			handle_create_new_tournament_pressed(state_data);
		
		if (ImGui::Button("Load Tournament"))
			handle_load_tournament_pressed(state_data);

		if (ImGui::Button("Quit"))
			handle_quit_pressed(state_data);

		ImGui::End();
	}
}