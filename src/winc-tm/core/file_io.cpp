#include "precompiled.h"

#include "3rd_party/tinyxml2/tinyxml2.h"
#include "data/pool.h"
#include "data/state.h"
#include "data/tournament_data.h"

#pragma warning(push)
#pragma warning(disable : 4774)
#include <fstream>
#include <iomanip>
#include <iostream>
#pragma warning(pop)
#include <Windows.h>

namespace winc
{
	const char file_path[] = "./files";
	const char attribute_value_name[] = "val";
	const char pool_fencers_file[] = "./files/pool_fencers.txt";
	const char pool_results_file[] = "./files/pool_results.txt";
	const char tournament_results_file[] = "./files/tournament_results.txt";

	/* tournament.xml */
	const char tournament_file[] = "./files/tournament.xml";
	const char tournament_element_name[] = "Tournament";
	const char name_element_name[] = "Name";
	const char elim_pool_count_element_name[] = "ElimPoolCount";
	const char max_fencers_in_pool_element_name[] = "MaxFencersInPool";
	const char fencers_in_finals_element_name[] = "FencersInFinals";

	/* fencers.xml */
	const char fencers_file[] = "./files/fencers.xml";
	const char fencers_element_name[] = "Fencers";
	const char fencer_element_name[] = "Fencer";
	const char fencer_id_element_name[] = "Id";
	const char hema_rating_element_name[] = "HemaRating";
	const char fencer_club_element_name[] = "Club";

	/* pools.xml */
	const char pools_file[] = "./files/pools.xml";
	const char pools_element_name[] = "Pools";
	const char pool_element_name[] = "Pool";
	const char elimination_pool_element_name[] = "ElimPool";
	const char final_pool_element_name[] = "FinalPool";
	const char bout_element_name[] = "Bout";
	const char bout_id_element_name[] = "Id";
	const char blue_fencer_element_name[] = "BlueFencer";
	const char red_fencer_element_name[] = "RedFencer";
	const char exchange_element_name[] = "Exchange";
	const char blue_hit_element_name[] = "BlueHit";
	const char red_hit_element_name[] = "RedHit";
	const char double_element_name[] = "Double";
	const char warning_blue_element_name[] = "WarningBlue";
	const char warning_red_element_name[] = "WarningRed";

	bool does_file_exist(LPCSTR file_name)
	{
		HANDLE hf = CreateFile(file_name, GENERIC_READ,	FILE_SHARE_READ | FILE_SHARE_WRITE,	NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,	NULL);

		if (INVALID_HANDLE_VALUE != hf)
		{
			CloseHandle(hf);
			return true;
		}
		else if (GetLastError() == ERROR_SHARING_VIOLATION)
		{
			// should we return 'exists but you can't access it' here?
			return true;
		}

		return false;
	}

	bool create_file(LPCSTR file_name)
	{
		HANDLE h = CreateFile(file_name, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,	0);
		if (h)
		{
			CloseHandle(h);
			return true;
		}
		
		return false;
	}

	/* Totally ugly and only works for trimming the names */
	void trim(char *str, size_t len)
	{
		for (size_t i = 0; i < len - 1; ++i)
		{
			if (str[i] == ' ' && str[i + 1] == ' ')
			{
				str[i] = '\0';
				return;
			}
		}
	}

	void write_pool_to_file(state &state_data, pool &pl, size_t pool_index)
	{
		CreateDirectory(file_path, NULL);

		if (!does_file_exist(pool_fencers_file))
			create_file(pool_fencers_file);

		std::ofstream file;
		file.open(pool_fencers_file);
		if (!file.is_open())
			return;

		file << "Pool ";
		file << pool_index;
		file << "\n";

		for (size_t pool_member_index = 0; pool_member_index < pl.fencers.size(); ++pool_member_index)
		{
			uint16_t pool_member = pl.fencers[pool_member_index];
			for (size_t fencer_index = 0; fencer_index < state_data.tournament_data->fencers.size(); ++fencer_index)
			{
				fencer &fenc = state_data.tournament_data->fencers[fencer_index];
				if (pool_member != fenc.id)
					continue;

				file << pool_member_index + 1;
				file << " ";
				file << fenc.name;
				file << " (";
				file << fenc.club;
				file << ")\n";
				break;
			}
		}

		file.close();
	}

	void write_pool_results_to_file(state &state_data)
	{
		CreateDirectory(file_path, NULL);

		if (!does_file_exist(pool_results_file))
			create_file(pool_results_file);

		std::ofstream file;
		file.open(pool_results_file);
		if (!file.is_open())
			return;

		file << "Pool Results\n";
		file << "# Name - Club - Match point index - Victory index - Q3 ratio - Clean ratio\n";
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

			file << result_index + 1;
			file << " ";
			file << fenc->name;

			file << " - ";
			file << fenc->club;

			file << " - ";
			file << std::fixed << std::setprecision(6) << get_fencer_matchpoint_index(result);

			file << " - ";
			file << std::fixed << std::setprecision(6) << get_fencer_victory_index(result);
			
			file << " - ";
			file << std::fixed << std::setprecision(6) << get_fencer_q3_ratio(result);

			file << " - ";
			file << std::fixed << std::setprecision(6) << get_fencer_clean_ratio(result);
			file << "\n";
		}
		file.close();
	}

	void write_single_pool_results_to_file(std::ofstream &file, tournament_data &tournament, pool &pl)
	{
		for (size_t bout_index = 0; bout_index < pl.bouts.size(); ++bout_index)
		{
			bout &bt = pl.bouts[bout_index];

			char *name_blue = nullptr;
			char *name_red = nullptr;
			for (size_t fencer_index = 0; fencer_index < tournament.fencers.size(); ++fencer_index)
			{
				fencer &fenc = tournament.fencers[fencer_index];
				if (fenc.id == bt.blue_fencer)
					name_blue = fenc.name;
				else if (fenc.id == bt.red_fencer)
					name_red = fenc.name;

				if (name_red && name_blue)
					break;
			}

			if (!name_red || !name_blue)
				continue;

			uint16_t blue_q1, blue_q2, blue_q3;
			uint16_t red_q1, red_q2, red_q3;
			get_blue_fencer_score(bt, blue_q1, blue_q2, blue_q3);
			get_red_fencer_score(bt, red_q1, red_q2, red_q3);

			bool suicidal_double_loss = get_suicidal_double_count(bt) >= 3;
			uint16_t warning_count_blue = get_blue_warning_count(bt);
			if (warning_count_blue >= 2)
			{
				if (blue_q3 > 0)
					--blue_q3;
				else if (blue_q2 > 0)
					--blue_q2;
				else if (blue_q1 > 0)
					--blue_q1;
			}

			uint16_t warning_count_red = get_red_warning_count(bt);
			if (warning_count_red >= 2)
			{
				if (red_q3 > 0)
					--red_q3;
				else if (red_q2 > 0)
					--red_q2;
				else if (red_q1 > 0)
					--red_q1;
			}

			const char *result_string[] = { "INVALID", "WIN", "DRAW", "LOSS" };
			bout_result bout_result_blue;
			bout_result bout_result_red;
			get_bout_results(bt, bout_result_blue, bout_result_red);

			if (suicidal_double_loss)
				file << "DOUBLE LOSS: ";

			file << "Q1(" << blue_q1 << ") Q2(" << blue_q2 << ") Q3(" << blue_q3 << ") " << result_string[bout_result_blue] << (warning_count_blue >= 4 ? "(warning loss) " : " ") << name_blue;
			file << " - ";
			file << name_red << " " << result_string[bout_result_red] << (warning_count_red >= 4 ? "(warning loss) " : " ") << " Q1(" << red_q1 << ") Q2(" << red_q2 << ") Q3(" << red_q3 << ")";
			file << "\n";
		}
	}

	void write_tournament_results_to_file(state &state_data)
	{
		if (!state_data.tournament_data)
			return;

		CreateDirectory(file_path, NULL);

		if (!does_file_exist(tournament_results_file))
			create_file(tournament_results_file);

		std::ofstream file;
		file.open(tournament_results_file);
		if (!file.is_open())
			return;

		tournament_data &tournament = *state_data.tournament_data;
		file << "TOURNAMENT RESULTS\n\n";

		file << "POOLS\n";
		for (size_t pool_index = 0; pool_index < tournament.pools.size(); ++pool_index)
		{
			file << "Pool ";
			file << pool_index + 1;
			file << "\n";

			pool &pl = tournament.pools[pool_index];
			write_single_pool_results_to_file(file, tournament, pl);

			file << "\n";
		}

		file << "ELIMINATION POOLS\n";
		for (size_t pool_index = 0; pool_index < tournament.elimination_pools.size(); ++pool_index)
		{
			file << "Elimination Pool ";
			file << pool_index + 1;
			file << "\n";

			pool &pl = tournament.elimination_pools[pool_index];
			write_single_pool_results_to_file(file, tournament, pl);

			file << "\n";
		}

		file << "FINALS\n";
		for (size_t pool_index = 0; pool_index < tournament.final_pool.size(); ++pool_index)
		{
			file << "Final Pool\n";

			pool &pl = tournament.final_pool[pool_index];
			write_single_pool_results_to_file(file, tournament, pl);

			file << "\n";
		}

		file.close();
	}

	void write_pool_data(pool &pl, tinyxml2::XMLDocument &pools_doc, tinyxml2::XMLNode *main_element, const char *element_name, size_t index)
	{
		tinyxml2::XMLElement *pool_element = pools_doc.NewElement(element_name);
		pool_element->SetAttribute(attribute_value_name, (int)index);
		main_element->InsertEndChild(pool_element);

		/* Fencers */
		for (size_t fencer_index = 0; fencer_index < pl.fencers.size(); ++fencer_index)
		{
			tinyxml2::XMLElement *fencer_element = pools_doc.NewElement(fencer_element_name);
			fencer_element->SetAttribute(attribute_value_name, pl.fencers[fencer_index]);
			pool_element->InsertEndChild(fencer_element);
		}

		/* Bouts */
		for (size_t bout_index = 0; bout_index < pl.bouts.size(); ++bout_index)
		{
			bout &bt = pl.bouts[bout_index];
			tinyxml2::XMLElement *bout_element = pools_doc.NewElement(bout_element_name);
			pool_element->InsertEndChild(bout_element);

			tinyxml2::XMLElement *bout_id_element = pools_doc.NewElement(bout_id_element_name);
			bout_id_element->SetAttribute(attribute_value_name, bt.id);
			bout_element->InsertEndChild(bout_id_element);

			tinyxml2::XMLElement *blue_fencer_element = pools_doc.NewElement(blue_fencer_element_name);
			blue_fencer_element->SetAttribute(attribute_value_name, bt.blue_fencer);
			bout_element->InsertEndChild(blue_fencer_element);

			tinyxml2::XMLElement *red_fencer_element = pools_doc.NewElement(red_fencer_element_name);
			red_fencer_element->SetAttribute(attribute_value_name, bt.red_fencer);
			bout_element->InsertEndChild(red_fencer_element);

			/* Exchanges */
			for (size_t exchange_index = 0; exchange_index < bt.exchanges.size(); ++exchange_index)
			{
				exchange &exc = bt.exchanges[exchange_index];
				tinyxml2::XMLElement *exchange_element = pools_doc.NewElement(exchange_element_name);
				bout_element->InsertEndChild(exchange_element);

				tinyxml2::XMLElement *exchange_blue_hit_element = pools_doc.NewElement(blue_hit_element_name);
				exchange_blue_hit_element->SetAttribute(attribute_value_name, exc.hit_blue);
				exchange_element->InsertEndChild(exchange_blue_hit_element);

				tinyxml2::XMLElement *exchange_red_hit_element = pools_doc.NewElement(red_hit_element_name);
				exchange_red_hit_element->SetAttribute(attribute_value_name, exc.hit_red);
				exchange_element->InsertEndChild(exchange_red_hit_element);

				tinyxml2::XMLElement *exchange_double_element = pools_doc.NewElement(double_element_name);
				exchange_double_element->SetAttribute(attribute_value_name, exc.dbl);
				exchange_element->InsertEndChild(exchange_double_element);

				tinyxml2::XMLElement *exchange_warning_blue_element = pools_doc.NewElement(warning_blue_element_name);
				exchange_warning_blue_element->SetAttribute(attribute_value_name, exc.warning_blue);
				exchange_warning_blue_element->InsertEndChild(pools_doc.NewText(exc.warning_blue_reason));
				exchange_element->InsertEndChild(exchange_warning_blue_element);

				tinyxml2::XMLElement *exchange_warning_red_element = pools_doc.NewElement(warning_red_element_name);
				exchange_warning_red_element->SetAttribute(attribute_value_name, exc.warning_red);
				exchange_warning_red_element->InsertEndChild(pools_doc.NewText(exc.warning_red_reason));
				exchange_element->InsertEndChild(exchange_warning_red_element);
			}
		}
	}

	bool read_pool_elements(std::vector<pool> &pools, tinyxml2::XMLNode *main_element, const char *element_name)
	{
		tinyxml2::XMLElement *pool_element = main_element->FirstChildElement(element_name);
		if (!pool_element)
			return false;

		while (pool_element)
		{
			pool pl;

			/* Fencers */
			tinyxml2::XMLElement *fencer_element = pool_element->FirstChildElement(fencer_element_name);
			if (!fencer_element)
			{
				pool_element = pool_element->NextSiblingElement(element_name);
				continue;
			}

			while (fencer_element)
			{
				pl.fencers.push_back((uint16_t)fencer_element->IntAttribute(attribute_value_name));
				fencer_element = fencer_element->NextSiblingElement(fencer_element_name);
			}

			/* Bouts */
			tinyxml2::XMLElement *bout_element = pool_element->FirstChildElement(bout_element_name);
			while (bout_element)
			{
				bout bt;
				tinyxml2::XMLElement *bout_id_element = bout_element->FirstChildElement(bout_id_element_name);
				if (!bout_id_element)
				{
					bout_element = bout_element->NextSiblingElement(bout_element_name);
					continue;
				}
				bt.id = (uint16_t)bout_id_element->IntAttribute(attribute_value_name);

				tinyxml2::XMLElement *blue_fencer_element = bout_element->FirstChildElement(blue_fencer_element_name);
				if (!blue_fencer_element)
				{
					bout_element = bout_element->NextSiblingElement(bout_element_name);
					continue;
				}
				bt.blue_fencer = (uint16_t)blue_fencer_element->IntAttribute(attribute_value_name);

				tinyxml2::XMLElement *red_fencer_element = bout_element->FirstChildElement(red_fencer_element_name);
				if (!red_fencer_element)
				{
					bout_element = bout_element->NextSiblingElement(bout_element_name);
					continue;
				}
				bt.red_fencer = (uint16_t)red_fencer_element->IntAttribute(attribute_value_name);

				/* Exchanges */
				tinyxml2::XMLElement *exchange_element = bout_element->FirstChildElement(exchange_element_name);
				while (exchange_element)
				{
					exchange exc;
					tinyxml2::XMLElement *exchange_blue_hit_element = exchange_element->FirstChildElement(blue_hit_element_name);
					if (!exchange_blue_hit_element)
					{
						exchange_element = exchange_element->NextSiblingElement(exchange_element_name);
						continue;
					}
					exc.hit_blue = (hit_quality)exchange_blue_hit_element->IntAttribute(attribute_value_name);

					tinyxml2::XMLElement *exchange_red_hit_element = exchange_element->FirstChildElement(red_hit_element_name);
					if (!exchange_red_hit_element)
					{
						exchange_element = exchange_element->NextSiblingElement(exchange_element_name);
						continue;
					}
					exc.hit_red = (hit_quality)exchange_red_hit_element->IntAttribute(attribute_value_name);

					tinyxml2::XMLElement *exchange_double_element = exchange_element->FirstChildElement(double_element_name);
					if (!exchange_double_element)
					{
						exchange_element = exchange_element->NextSiblingElement(exchange_element_name);
						continue;
					}
					exc.dbl = (double_hit)exchange_double_element->IntAttribute(attribute_value_name);

					tinyxml2::XMLElement *exchange_warning_blue_element = exchange_element->FirstChildElement(warning_blue_element_name);
					if (!exchange_warning_blue_element)
					{
						exchange_element = exchange_element->NextSiblingElement(exchange_element_name);
						continue;
					}
					exc.warning_blue = exchange_warning_blue_element->BoolAttribute(attribute_value_name);
					if (exchange_warning_blue_element->GetText())
						memcpy(exc.warning_blue_reason, exchange_warning_blue_element->GetText(), strlen(exchange_warning_blue_element->GetText()) + 1);
					else
						exc.warning_blue_reason[0] = '\0';

					tinyxml2::XMLElement * exchange_warning_red_element = exchange_element->FirstChildElement(warning_red_element_name);
					if (!exchange_warning_red_element)
					{
						exchange_element = exchange_element->NextSiblingElement(exchange_element_name);
						continue;
					}
					exc.warning_red = exchange_warning_red_element->BoolAttribute(attribute_value_name);
					if (exchange_warning_red_element->GetText())
						memcpy(exc.warning_red_reason, exchange_warning_red_element->GetText(), strlen(exchange_warning_red_element->GetText()) + 1);
					else
						exc.warning_red_reason[0] = '\0';

					bt.exchanges.push_back(exc);
					exchange_element = exchange_element->NextSiblingElement(exchange_element_name);
				}



				pl.bouts.push_back(bt);
				bout_element = bout_element->NextSiblingElement(bout_element_name);
			}

			pools.push_back(pl);
			pool_element = pool_element->NextSiblingElement(element_name);
		}

		return true;
	}

	bool write_tournament_data(tournament_data &data)
	{		
		CreateDirectory(file_path, NULL);

		/* Tournament document */
		{
			tinyxml2::XMLDocument tournament_doc;
			if (!does_file_exist(tournament_file))
				create_file(tournament_file);

			tinyxml2::XMLNode *main_element = tournament_doc.InsertEndChild(tournament_doc.NewElement(tournament_element_name));

			tinyxml2::XMLNode *name_element = main_element->InsertEndChild(tournament_doc.NewElement(name_element_name));
			name_element->InsertFirstChild(tournament_doc.NewText(data.tournament_name));

			tinyxml2::XMLElement *elim_pool_count_element = tournament_doc.NewElement(elim_pool_count_element_name);
			elim_pool_count_element->SetAttribute(attribute_value_name, data.elimination_pool_count);
			main_element->InsertEndChild(elim_pool_count_element);

			tinyxml2::XMLElement *max_fencers_in_pool_element = tournament_doc.NewElement(max_fencers_in_pool_element_name);
			max_fencers_in_pool_element->SetAttribute(attribute_value_name, data.max_fencers_in_pool);
			main_element->InsertEndChild(max_fencers_in_pool_element);

			tinyxml2::XMLElement *fencers_in_finals_element = tournament_doc.NewElement(fencers_in_finals_element_name);
			fencers_in_finals_element->SetAttribute(attribute_value_name, data.fencers_in_finals);
			main_element->InsertEndChild(fencers_in_finals_element);
		
			tinyxml2::XMLError ret = tournament_doc.SaveFile(tournament_file);
			if (ret != tinyxml2::XML_SUCCESS)
				return false;
		}

		/* Fencers document */
		{
			tinyxml2::XMLDocument fencers_doc;
			if (!does_file_exist(fencers_file))
				create_file(fencers_file);

			tinyxml2::XMLNode *main_element = fencers_doc.InsertEndChild(fencers_doc.NewElement(fencers_element_name));

			for (size_t i = 0; i < data.fencers.size(); ++i)
			{
				fencer &fenc = data.fencers[i];
				tinyxml2::XMLNode *fencer_element = main_element->InsertEndChild(fencers_doc.NewElement(fencer_element_name));
				fencer_element->InsertFirstChild(fencers_doc.NewText(fenc.name));

				tinyxml2::XMLElement *id_element = fencers_doc.NewElement(fencer_id_element_name);
				id_element->SetAttribute(attribute_value_name, fenc.id);
				fencer_element->InsertEndChild(id_element);

				tinyxml2::XMLElement *rating_element = fencers_doc.NewElement(hema_rating_element_name);
				rating_element->SetAttribute(attribute_value_name, fenc.hema_rating);
				fencer_element->InsertEndChild(rating_element);

				tinyxml2::XMLNode *club_element = fencer_element->InsertEndChild(fencers_doc.NewElement(fencer_club_element_name));
				club_element->InsertEndChild(fencers_doc.NewText(fenc.club));
			}

			tinyxml2::XMLError ret = fencers_doc.SaveFile(fencers_file);
			if (ret != tinyxml2::XML_SUCCESS)
				return false;
		}

		/* Pools document */
		{
			tinyxml2::XMLDocument pools_doc;
			if (!does_file_exist(pools_file))
				create_file(pools_file);

			tinyxml2::XMLNode *main_element = pools_doc.InsertEndChild(pools_doc.NewElement(pools_element_name));

			/* Normal pools */
			for (size_t i = 0; i < data.pools.size(); ++i)
				write_pool_data(data.pools[i], pools_doc, main_element, pool_element_name, i);

			/* Elimination pools */
			for (size_t i = 0; i < data.elimination_pools.size(); ++i)
				write_pool_data(data.elimination_pools[i], pools_doc, main_element, elimination_pool_element_name, i);

			/* Final pool */
			for (size_t i = 0; i < data.final_pool.size(); ++i)
				write_pool_data(data.final_pool[i], pools_doc, main_element, final_pool_element_name, i);

			tinyxml2::XMLError ret = pools_doc.SaveFile(pools_file);
			if (ret != tinyxml2::XML_SUCCESS)
				return false;
		}

		return true;
	}

	bool read_tournament_data(tournament_data &data)
	{
		/* Tournament document */
		{
			tinyxml2::XMLDocument tournament_doc;
			tinyxml2::XMLError ret = tournament_doc.LoadFile(tournament_file);
			if (ret != tinyxml2::XML_SUCCESS)
				return false;

			tinyxml2::XMLElement *main_element = tournament_doc.FirstChildElement(tournament_element_name);
			if (!main_element)
				return false;

			tinyxml2::XMLElement *name_element = main_element->FirstChildElement(name_element_name);
			if (!name_element)
				return false;

			memcpy(data.tournament_name, name_element->GetText(), strlen(name_element->GetText()) + 1);

			tinyxml2::XMLElement *elim_pool_count_element = main_element->FirstChildElement(elim_pool_count_element_name);
			if (!elim_pool_count_element)
				return false;

			data.elimination_pool_count = (uint16_t)elim_pool_count_element->IntAttribute(attribute_value_name);

			tinyxml2::XMLElement *max_fencers_in_pool_element = main_element->FirstChildElement(max_fencers_in_pool_element_name);
			if (!max_fencers_in_pool_element)
				return false;

			data.max_fencers_in_pool = (uint16_t)max_fencers_in_pool_element->IntAttribute(attribute_value_name);

			tinyxml2::XMLElement *fencers_in_finals_element = main_element->FirstChildElement(fencers_in_finals_element_name);
			if (!fencers_in_finals_element_name)
				return false;

			data.fencers_in_finals = (uint16_t)fencers_in_finals_element->IntAttribute(attribute_value_name);
		}

		/* Fencers document */
		{
			tinyxml2::XMLDocument fencers_doc;
			tinyxml2::XMLError ret = fencers_doc.LoadFile(fencers_file);
			if (ret != tinyxml2::XML_SUCCESS)
				return false;

			tinyxml2::XMLElement *main_element = fencers_doc.FirstChildElement(fencers_element_name);
			if (!main_element)
				return false;

			tinyxml2::XMLElement *fencer_element = main_element->FirstChildElement(fencer_element_name);
			if (!fencer_element)
				return false;

			while (fencer_element)
			{
				fencer fenc;
				memcpy(fenc.name, fencer_element->GetText(), strlen(fencer_element->GetText()) + 1);
				trim(fenc.name, fencer::max_string_len);

				tinyxml2::XMLElement *id_element = fencer_element->FirstChildElement(fencer_id_element_name);
				if (!id_element)
					return false;

				fenc.id = (uint16_t)id_element->IntAttribute(attribute_value_name);

				tinyxml2::XMLElement *rating_element = fencer_element->FirstChildElement(hema_rating_element_name);
				if (!rating_element)
					return false;

				fenc.hema_rating = (uint16_t)rating_element->IntAttribute(attribute_value_name);

				tinyxml2::XMLElement *club_element = fencer_element->FirstChildElement(fencer_club_element_name);
				if (!club_element)
					return false;

				if (club_element->GetText() && strlen(club_element->GetText()) > 0)
					memcpy(fenc.club, club_element->GetText(), strlen(club_element->GetText()) + 1);
				else
					fenc.club[0] = '\0';

				data.fencers.push_back(fenc);
				fencer_element = fencer_element->NextSiblingElement(fencer_element_name);
			}
		}

		/* Pools document */
		{
			tinyxml2::XMLDocument pools_doc;
			tinyxml2::XMLError ret = pools_doc.LoadFile(pools_file);
			if (ret != tinyxml2::XML_SUCCESS)
				return false;

			tinyxml2::XMLElement *main_element = pools_doc.FirstChildElement(pools_element_name);
			if (!main_element)
				return false;

			if (!read_pool_elements(data.pools, main_element, pool_element_name))
				return false;

			read_pool_elements(data.elimination_pools, main_element, elimination_pool_element_name);
			read_pool_elements(data.final_pool, main_element, final_pool_element_name);
		}
		
		return true;
	}
}