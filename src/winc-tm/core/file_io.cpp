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

				for (size_t fencer_index = 0; fencer_index < pl.fencers.size(); ++fencer_index)
				{
					tinyxml2::XMLElement *fencer_element = pools_doc.NewElement(fencer_element_name);
					fencer_element->SetAttribute(attribute_value_name, pl.fencers[fencer_index]);
					pool_element->InsertEndChild(fencer_element);
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

				data.pools.push_back(pl);
				pool_element = pool_element->NextSiblingElement(pool_element_name);
			}
		}
		
		return true;
	}
}