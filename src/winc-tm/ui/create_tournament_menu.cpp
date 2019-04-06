#include "precompiled.h"

#include "create_tournament_menu.h"

#include "3rd_party/imgui/imgui.h"
#include "data/state.h"

namespace winc
{
namespace
{
	bool validate_tournament_data(state &state_data)
	{
		create_tournament_data &tournament_data = *state_data.new_tournament_data;
		if (tournament_data.tournament_name[0] == '\0')
		{
			char msg[] = "Invalid tournament name";
			memcpy(tournament_data.error_message, msg, strlen(msg) + 1);
			return false;
		}

		if (tournament_data.max_pool_count == 0)
		{
			char msg[] = "Max pool count zero";
			memcpy(tournament_data.error_message, msg, strlen(msg) + 1);
			return false;
		}


		if (tournament_data.max_fencers_in_pool == 0)
		{
			char msg[] = "Max fencers per pool is zero";
			memcpy(tournament_data.error_message, msg, strlen(msg) + 1);
			return false;
		}

		if (tournament_data.elimination_pool_count == 0)
		{
			char msg[] = "Elimination pool count zero";
			memcpy(tournament_data.error_message, msg, strlen(msg) + 1);
			return false;
		}

		if (tournament_data.elimination_pool_count > tournament_data.max_pool_count)
		{
			char msg[] = "Elimination pool count is greater than maximum pool count";
			memcpy(tournament_data.error_message, msg, strlen(msg) + 1);
			return false;
		}

		if (tournament_data.final_pool_fencer_count < 3)
		{
			char msg[] = "Final pool fencer count is less than 3";
			memcpy(tournament_data.error_message, msg, strlen(msg) + 1);
			return false;
		}

		if (tournament_data.fencers.size() < tournament_data.final_pool_fencer_count)
		{
			char msg[] = "Fencer count is less than final pool fencer count";
			memcpy(tournament_data.error_message, msg, strlen(msg) + 1);
			return false;
		}

		if (tournament_data.fencers.size() > (size_t)(tournament_data.max_pool_count * tournament_data.max_fencers_in_pool))
		{
			char msg[] = "Fencer count is larger than total maximum of pool slots (max pools * max fencers per pool)";
			memcpy(tournament_data.error_message, msg, strlen(msg) + 1);
			return false;
		}

		return true;
	}

	void initialize_pools(state &state_data)
	{
		/* Sort fencers by ranking */
		std::list<fencer> sorted_fencers;
		std::list<fencer>::iterator it = state_data.new_tournament_data->fencers.begin();
		sorted_fencers.push_front(*it);
		++it;
		for (; it != state_data.new_tournament_data->fencers.end(); ++it)
		{
			if (it->hema_rating == 0)
			{
				sorted_fencers.push_back(*it);
				continue;
			}

			bool inserted = false;
			for (std::list<fencer>::iterator sorted_it = sorted_fencers.begin(); sorted_it != sorted_fencers.end(); ++sorted_it)
			{				
				if (it->hema_rating < sorted_it->hema_rating || sorted_it->hema_rating == 0)
				{
					sorted_fencers.insert(sorted_it, *it);
					inserted = true;
					break;
				}			
			}

			if (!inserted)
			{
				sorted_fencers.push_back(*it);
				continue;
			}
		}
		
		/* Set id pased on internal rank (what we get from prev sort) */
		uint8_t id = 0;
		for (it = sorted_fencers.begin(); it != sorted_fencers.end(); ++it, ++id)
			it->id = id;

		state_data.new_tournament_data->fencers.swap(sorted_fencers);

		/* Calculate pool count */
		size_t pool_count = state_data.new_tournament_data->fencers.size() / state_data.new_tournament_data->max_fencers_in_pool;
		if (state_data.new_tournament_data->fencers.size() / state_data.new_tournament_data->max_fencers_in_pool != 0)
			++pool_count;

		state_data.new_tournament_data->pools.resize(pool_count);

		/* Split people to pools in order (first goes to pool 1, second goes to pool 2, ..., pool count + 1 goes to pool 1, etc */
		uint8_t i = 0;
		for (it = state_data.new_tournament_data->fencers.begin(); it != state_data.new_tournament_data->fencers.end(); ++it, ++i)
			state_data.new_tournament_data->pools[i % pool_count].push_back(it->id);
	}

	void handle_return_to_main_menu_pressed(state &state_data)
	{
		state_data.menu_state = main_menu;
		if (state_data.new_tournament_data)
			delete state_data.new_tournament_data;
	}

	void handle_next_pressed(state &state_data)
	{
		if (validate_tournament_data(state_data))
		{
			initialize_pools(state_data);
			state_data.menu_state = create_pools;
		}
		else
			ImGui::OpenPopup("error_popup");
	}

	void handle_error_popup(state & state_data)
	{
		if (ImGui::BeginPopup("error_popup"))
		{
			ImGui::Text(state_data.new_tournament_data->error_message);
			ImGui::EndPopup();
		}
	}

	void show_invalid_torunament_data_error(state & state_data)
	{
		ImVec2 window_size(550.0f, 300.0f);
		ImGui::SetNextWindowPos(ImVec2(state_data.resolution_x / 2.0f, state_data.resolution_y / 2.0f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
		ImGui::SetNextWindowSize(window_size, ImGuiCond_Always);

		static const ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		ImGui::Begin("Create Tournament", nullptr, window_flags);

		ImGui::TextWrapped("Something went horribly wrong and we failed to create data for the new tournament. Please return to main menu and try again.");

		if (ImGui::Button("Return to Main Menu"))
			handle_return_to_main_menu_pressed(state_data);

		ImGui::End();
	}

	void show_create_tournament_menu(state & state_data)
	{
		ImVec2 window_size(state_data.resolution_x * 0.9f, state_data.resolution_y * 0.85f);
		ImGui::SetNextWindowSize(window_size, ImGuiCond_Always);
		ImGui::SetNextWindowPos(ImVec2(state_data.resolution_x / 2.0f, state_data.resolution_y / 2.0f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));

		static const ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		ImGui::Begin("Create Tournament", nullptr, window_flags);

		ImGui::InputTextWithHint("##tournament_name", "Tournament Name", state_data.new_tournament_data->tournament_name, state_data.new_tournament_data->max_tournament_name_len);

		if (ImGui::CollapsingHeader("Tournament Structure"))
		{
			ImGui::Text("Maximum number of fencers in pool");
			ImGui::SameLine();
			int max_fencers_in_pool = state_data.new_tournament_data->max_fencers_in_pool;
			if (ImGui::InputInt("##max_fencers_in_pool", &max_fencers_in_pool))
				state_data.new_tournament_data->max_fencers_in_pool = (uint16_t)max_fencers_in_pool;

			ImGui::Text("Number of fencers in final pool");
			ImGui::SameLine();
			int final_pool_fencer_count = state_data.new_tournament_data->final_pool_fencer_count;
			if (ImGui::InputInt("##final_pool_fencer_count", &final_pool_fencer_count))
				state_data.new_tournament_data->final_pool_fencer_count = (uint16_t)final_pool_fencer_count;

			ImGui::Text("Maximum number of pools");
			ImGui::SameLine();
			int pool_count = state_data.new_tournament_data->max_pool_count;
			if (ImGui::InputInt("##pool_count", &pool_count))
				state_data.new_tournament_data->max_pool_count = (uint16_t)pool_count;

			ImGui::Text("Number of elimination pools");
			ImGui::SameLine();
			int elim_pool_count = state_data.new_tournament_data->elimination_pool_count;
			if (ImGui::InputInt("##elim_pool_count", &elim_pool_count))
				state_data.new_tournament_data->elimination_pool_count = (uint16_t)elim_pool_count;
		}

		if (ImGui::CollapsingHeader("Fencers"))
		{
			std::list<fencer> &fencers = state_data.new_tournament_data->fencers;
			int num_fencers = (int)fencers.size();
			ImGui::Text("Number of fencers");
			ImGui::SameLine();
			if (ImGui::InputInt("##number_of_fencers", &num_fencers))
				fencers.resize((size_t)num_fencers);

			size_t fencer_index = 0;
			for (std::list<fencer>::iterator it = fencers.begin(); it != fencers.end(); ++it)
			{
				if (ImGui::TreeNode((void *)(intptr_t)fencer_index, it->name))
				{
					ImGui::InputTextWithHint("##name", "Name", it->name, fencer::max_string_len);
					ImGui::InputTextWithHint("##club", "Club", it->club, fencer::max_string_len);
					ImGui::Text("HEMA Ratings Score (leave 0 for no score)");
					ImGui::SameLine();
					int rating = it->hema_rating;
					if (ImGui::InputInt("##rating", &rating))
						it->hema_rating = (uint16_t)rating;

					ImGui::TreePop();
				}

				++fencer_index;
			}
		}

		if (ImGui::Button("Next"))
			handle_next_pressed(state_data);

		handle_error_popup(state_data);

		ImGui::End();
	}
}

	void handle_create_tournament_menu(state &state_data)
	{
		if (!state_data.new_tournament_data)
		{
			show_invalid_torunament_data_error(state_data);
			return;
		}

		show_create_tournament_menu(state_data);
	}
}