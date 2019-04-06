#include "precompiled.h"

#include "create_pools_menu.h"

#include "3rd_party/imgui/imgui.h"
#include "data/state.h"

namespace winc
{
namespace
{
	void handle_back_pressed(state &state_data)
	{
		state_data.menu_state = create_tournament;
	}

	void handle_next_pressed(state &state_data)
	{
		state_data.menu_state = create_pools;
	}
}
	
	void handle_create_pools_menu(state &state_data)
	{
		ImVec2 window_size(state_data.resolution_x * 0.9f, state_data.resolution_y * 0.85f);
		ImGui::SetNextWindowSize(window_size, ImGuiCond_Always);
		ImGui::SetNextWindowPos(ImVec2(state_data.resolution_x / 2.0f, state_data.resolution_y / 2.0f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));

		static const ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		ImGui::Begin("Create Pools", nullptr, window_flags);

		std::list<fencer> &fencers = state_data.new_tournament_data->fencers;
		for (size_t pool_index = 0; pool_index < state_data.new_tournament_data->pools.size(); ++pool_index)
		{
			/* This is ugly but I'd say we are safe with 9999 pools */
			char pool_num_str[4];
			sprintf(pool_num_str, "%u", (uint32_t)pool_index + 1);
			char pool_name[10] = "Pool ";
			strcat(pool_name, pool_num_str);
			if (ImGui::CollapsingHeader(pool_name))
			{
				std::list<uint16_t> &pool = state_data.new_tournament_data->pools[pool_index];
				size_t pool_member_index = 0;
				for (std::list<uint16_t>::iterator pool_member_it = pool.begin(); pool_member_it != pool.end(); ++pool_member_it)
				{
					for (std::list<fencer>::iterator fencer_it = fencers.begin(); fencer_it != fencers.end(); ++fencer_it, ++pool_member_index)
					{
						if (*pool_member_it != fencer_it->id)
							continue;

						if (ImGui::TreeNode((void *)(intptr_t)pool_member_index, fencer_it->name))
						{

						}
					}
				}
			}
		}

		if (ImGui::Button("Back"))
			handle_back_pressed(state_data);

		ImGui::SameLine();

		if (ImGui::Button("Next"))
			handle_next_pressed(state_data);

		ImGui::End();
	}
}