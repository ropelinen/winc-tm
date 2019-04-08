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
		void handle_main_window(state &state_data)
		{
			fencer *blue_fencer = nullptr;
			fencer *red_fencer = nullptr;
			bout *bt = nullptr;

			tournament_data &tournament = *state_data.tournament_data;
			for (size_t pool_index = 0; pool_index < tournament.pools.size(); ++pool_index)
			{
				for (size_t bout_index = 0; bout_index < tournament.pools[pool_index].bouts.size(); ++bout_index)
				{
					if (tournament.pools[pool_index].bouts[bout_index].id == state_data.bout_to_modify)
					{
						bt = &tournament.pools[pool_index].bouts[bout_index];
						break;
					}
				}

				if (bt)
					break;
			}

			for (size_t i = 0; i < tournament.fencers.size(); ++i)
			{
				if (tournament.fencers[i].id == bt->blue_fencer)
					blue_fencer = &tournament.fencers[i];
				else if (tournament.fencers[i].id == bt->red_fencer)
					red_fencer = &tournament.fencers[i];

				if (blue_fencer && red_fencer)
					break;
			}

			if (!blue_fencer || !red_fencer || !bt)
				return;

			ImGui::TextColored(ImVec4(0.04f, 0.1f, 0.9f, 1.0f), blue_fencer->name);
			ImGui::SameLine();
			ImGui::Text(" - ");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(0.8f, 0.1f, 0.04f, 1.0f), red_fencer->name);

			uint16_t exchange_to_remove = 0xFFFF;
			const char *items_hit_quality[] = { "No Hit", "No Quality", "Wound", "Incapacitate" };
			const char *items_double[] = { "No Double", "Accidental", "Suicidal" };
			for (size_t exchange_index = 0; exchange_index < bt->exchanges.size(); ++exchange_index)
			{
				exchange &exc = bt->exchanges[exchange_index];

				uint16_t quality_blue = get_exchange_quality_blue(exc);
				uint16_t quality_red = get_exchange_quality_red(exc);
				if (ImGui::TreeNode((void *)(intptr_t)exchange_index, "Blue; Q%u, Red: Q%u, Double: %s, Warnings: %s/%s", quality_blue, quality_red, items_double[exc.dbl], exc.warning_blue ? "Blue" : "-", exc.warning_red ? "Red" : "-"))
				{
					/* Blue hit */
					{
						const char *item_current_blue = items_hit_quality[exc.hit_blue];
						ImGui::Text("Hit quality blue");
						ImGui::SameLine();
						if (ImGui::BeginCombo("##hit_quality_blue", item_current_blue, 0))
						{
							for (int n = 0; n < IM_ARRAYSIZE(items_hit_quality); n++)
							{
								bool is_selected = (item_current_blue == items_hit_quality[n]);
								if (ImGui::Selectable(items_hit_quality[n], is_selected))
								{
									item_current_blue = items_hit_quality[n];
									exc.hit_blue = (hit_quality)n;
								}
								if (is_selected)
									ImGui::SetItemDefaultFocus();
							}
							ImGui::EndCombo();
						}
					}

					/* Red hit */
					{
						const char *item_current_red = items_hit_quality[exc.hit_red];
						ImGui::Text("Hit quality red");
						ImGui::SameLine();
						if (ImGui::BeginCombo("##hit_quality_red", item_current_red, 0))
						{
							for (int n = 0; n < IM_ARRAYSIZE(items_hit_quality); n++)
							{
								bool is_selected = (item_current_red == items_hit_quality[n]);
								if (ImGui::Selectable(items_hit_quality[n], is_selected))
								{
									item_current_red = items_hit_quality[n];
									exc.hit_red = (hit_quality)n;
								}
								if (is_selected)
									ImGui::SetItemDefaultFocus();
							}
							ImGui::EndCombo();
						}
					}

					/* Double */
					{
						const char *item_current_double = items_double[exc.dbl];
						ImGui::Text("Double");
						ImGui::SameLine();
						if (ImGui::BeginCombo("##double", item_current_double, 0))
						{
							for (int n = 0; n < IM_ARRAYSIZE(items_double); n++)
							{
								bool is_selected = (item_current_double == items_double[n]);
								if (ImGui::Selectable(items_double[n], is_selected))
								{
									item_current_double = items_double[n];
									exc.dbl = (double_hit)n;
								}
								if (is_selected)
									ImGui::SetItemDefaultFocus();
							}
							ImGui::EndCombo();
						}
					}

					/* Warning blue */
					{
						ImGui::Checkbox("Warning blue", &exc.warning_blue);
						ImGui::SameLine();
						ImGui::InputTextWithHint("##warning_reason_blue", "Warning Reason", exc.warning_blue_reason, exc.warning_max_len);
					}

					/* Warning red */
					{
						ImGui::Checkbox("Warning red", &exc.warning_red);
						ImGui::SameLine();
						ImGui::InputTextWithHint("##warning_reason_red", "Warning Reason", exc.warning_red_reason, exc.warning_max_len);
					}

					if (ImGui::Button("Remove"))
					{
						exchange_to_remove = (uint16_t)exchange_index;
					}

					ImGui::TreePop();
				}				
			}

			if (ImGui::Button("Add Exchange"))
			{
				exchange ex;
				ex.warning_blue_reason[0] = '\0';
				ex.warning_red_reason[0] = '\0';
				bt->exchanges.push_back(ex);
			}

			if (ImGui::Button("Back"))
			{
				write_tournament_data(tournament);
				state_data.menu_state = run_tournament;
			}

			if (exchange_to_remove != 0xFFFF)
			{
				bt->exchanges.erase(bt->exchanges.begin() + exchange_to_remove);
				exchange_to_remove = 0xFFFF;
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