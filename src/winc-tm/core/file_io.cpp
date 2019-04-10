#include "precompiled.h"

#include "3rd_party/tinyxml2/tinyxml2.h"
#include "data/tournament_data.h"

#include <Windows.h>

namespace winc
{
	const char file_path[] = "./files";
	const char attribute_value_name[] = "val";

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

			for (size_t i = 0; i < data.pools.size(); ++i)
			{
				pool &pl = data.pools[i];
				tinyxml2::XMLElement *pool_element = pools_doc.NewElement(pool_element_name);
				pool_element->SetAttribute(attribute_value_name, (int)i);
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

				memcpy(fenc.club, club_element->GetText(), strlen(club_element->GetText()) + 1);

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

			tinyxml2::XMLElement *pool_element = main_element->FirstChildElement(pool_element_name);
			if (!pool_element)
				return false;

			while (pool_element)
			{
				pool pl;

				/* Fencers */
				tinyxml2::XMLElement *fencer_element = pool_element->FirstChildElement(fencer_element_name);
				if (!fencer_element)
				{
					pool_element = pool_element->NextSiblingElement(pool_element_name);
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

						tinyxml2::XMLElement *exchange_warning_red_element = exchange_element->FirstChildElement(warning_red_element_name);
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

				data.pools.push_back(pl);
				pool_element = pool_element->NextSiblingElement(pool_element_name);
			}
		}
		
		return true;
	}
}