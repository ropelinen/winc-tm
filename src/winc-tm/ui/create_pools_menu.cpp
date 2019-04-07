#include "precompiled.h"

#include "create_pools_menu.h"

#include "3rd_party/imgui/imgui.h"
#include "core/file_io.h"
#include "data/create_tournament_data.h"
#include "data/state.h"
#include "data/tournament_data.h"

#include <algorithm>

namespace winc
{
namespace
{
	bool validate_pools(state &state_data)
	{
		create_tournament_data &tournament_data = *state_data.new_tournament_data;
		for (size_t pool_index = 0; pool_index < tournament_data.pools.size(); ++pool_index)
		{
			if (tournament_data.pools[pool_index].size() > tournament_data.max_fencers_in_pool)
			{
				char msg[] = "Too many fencers in a single pool";
				memcpy(tournament_data.error_message, msg, strlen(msg) + 1);
				return false;
			}
		}

		return true;
	}

	void initialize_tournament_data(state &state_data)
	{
		tournament_data *tournament = new tournament_data;
		state_data.tournament_data = tournament;
		create_tournament_data *creation_data = state_data.new_tournament_data;
		
		memcpy(tournament->tournament_name, creation_data->tournament_name, std::min(tournament_data::max_tournament_name_len, create_tournament_data::max_tournament_name_len));
		tournament->elimination_pool_count = creation_data->elimination_pool_count;
		tournament->fencers_in_finals = creation_data->final_pool_fencer_count;

		for (std::list<fencer>::iterator it = creation_data->fencers.begin(); it != creation_data->fencers.end(); ++it)
			tournament->fencers.push_back(*it);

		for (size_t pool_index = 0; pool_index < creation_data->pools.size(); ++pool_index)
		{
			std::list<uint16_t> &creation_pool = creation_data->pools[pool_index];
			pool pool;
			for (std::list<uint16_t>::iterator it = creation_pool.begin(); it != creation_pool.end(); ++it)
				pool.fencers.push_back(*it);

			tournament->pools.push_back(pool);
		}
	}

	void handle_back_pressed(state &state_data)
	{
		state_data.menu_state = create_tournament;
	}

	void handle_next_pressed(state &state_data)
	{
		if (!validate_pools(state_data))
		{
			ImGui::OpenPopup("error_popup");
			return;
		}

		initialize_tournament_data(state_data);
		delete state_data.new_tournament_data;
		state_data.new_tournament_data = nullptr;
		write_tournament_data(*state_data.tournament_data);
		state_data.menu_state = run_tournament;
	}

	void handle_error_popup(state &state_data)
	{
		if (ImGui::BeginPopup("error_popup"))
		{
			ImGui::Text(state_data.new_tournament_data->error_message);
			ImGui::EndPopup();
		}
	}

	void handle_move_to_new_pool(state &state_data, uint16_t fencer_id, uint16_t rating, size_t current_pool, size_t new_pool)
	{
		if (current_pool == new_pool)
			return;

		std::list<uint16_t> &old_pool = state_data.new_tournament_data->pools[current_pool];
		for (std::list<uint16_t>::iterator it = old_pool.begin(); it != old_pool.end(); ++it)
		{
			if (*it != fencer_id)
				continue;
			
			old_pool.erase(it);
			break;
		}

		std::list<uint16_t> &pool = state_data.new_tournament_data->pools[new_pool];

		if (rating == 0)
		{
			pool.push_back(fencer_id);
			return;
		}

		bool inserted = false;
		for (std::list<uint16_t>::iterator it = pool.begin(); it != pool.end(); ++it)
		{
			for (std::list<fencer>::iterator fencer_it = state_data.new_tournament_data->fencers.begin(); fencer_it != state_data.new_tournament_data->fencers.end(); ++fencer_it)
			{
				if (*it != fencer_it->id)
					continue;
				
				if (fencer_it->hema_rating > rating)
				{
					pool.insert(it, fencer_id);
					inserted = true;
				}
				break;
			}

			if (inserted)
				break;
		}

		if (!inserted)
		{
			pool.push_back(fencer_id);
			return;
		}
	}
}
	
	void handle_create_pools_menu(state &state_data)
	{
		ImVec2 window_size(state_data.resolution_x * 0.9f, state_data.resolution_y * 0.85f);
		ImGui::SetNextWindowSize(window_size, ImGuiCond_Always);
		ImGui::SetNextWindowPos(ImVec2(state_data.resolution_x / 2.0f, state_data.resolution_y / 2.0f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));

		static const ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		ImGui::Begin("Create Pools", nullptr, window_flags);

		/* This is quite ugly */
		bool move_fencer = false;
		uint16_t fencer_to_move = 0;
		uint16_t fencer_rating = 0;
		size_t fencer_current_pool = 0;
		size_t fencer_new_pool = 0;

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

						if (ImGui::TreeNode((void *)(intptr_t)pool_member_index, "%s, %s (%u)", fencer_it->name, fencer_it->club, fencer_it->hema_rating))
						{
							ImGui::Text("Club: %s", fencer_it->club);
							ImGui::Text("Rating: %u", fencer_it->hema_rating);

							if (ImGui::Button("Change pool"))
								ImGui::OpenPopup("change_pool_popup");

							if (ImGui::BeginPopup("change_pool_popup"))
							{
								ImGui::Text("Select new pool");

								for (size_t new_pool = 0; new_pool < state_data.new_tournament_data->pools.size(); ++new_pool)
								{
									char new_pool_num_str[4];
									sprintf(new_pool_num_str, "%u", (uint32_t)new_pool + 1);
									char new_pool_name[10] = "Pool ";
									strcat(new_pool_name, new_pool_num_str);

									if (ImGui::Button(new_pool_name))
									{
										if (new_pool != pool_index)
										{
											move_fencer = true;
											fencer_to_move = fencer_it->id;
											fencer_rating = fencer_it->hema_rating;
											fencer_current_pool = pool_index;
											fencer_new_pool = new_pool;

											ImGui::CloseCurrentPopup();
										}
									}
								}

								if (ImGui::Button("Cancel"))
									ImGui::CloseCurrentPopup();

								ImGui::EndPopup();
							}

							ImGui::TreePop();
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

		handle_error_popup(state_data);

		ImGui::End();

		if (move_fencer)
			handle_move_to_new_pool(state_data, fencer_to_move, fencer_rating, fencer_current_pool, fencer_new_pool);
	}
}