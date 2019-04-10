#pragma once

#include "core/include_std_containers.h"
#include "data/bout.h"
#include "data/fencer_results.h"

namespace winc
{
	struct pool
	{
		std::vector<uint16_t> fencers;
		std::vector<bout> bouts;
	};

	static void calculate_pool_results(const pool &pl, std::vector<fencer_results> &results)
	{
		for (size_t bout_index = 0; bout_index < pl.bouts.size(); ++bout_index)
		{
			const bout &bt = pl.bouts[bout_index];
			fencer_results *blue = nullptr;
			fencer_results *red = nullptr;

			{
				size_t blue_index = 0xFFFF;
				size_t red_index = 0xFFFF;
				for (size_t result_index = 0; result_index < results.size(); ++result_index)
				{
					fencer_results &result = results[result_index];
					if (result.id == bt.blue_fencer)
						blue_index = result_index;
					else if (result.id == bt.red_fencer)
						red_index = result_index;

					if (red_index != 0xFFFF && blue_index != 0xFFFF)
						break;
				}

				if (blue_index == 0xFFFF)
				{
					fencer_results result;
					result.id = bt.blue_fencer;
					results.push_back(result);
					blue_index = results.size() - 1;
				}

				if (red_index == 0xFFFF)
				{
					fencer_results result;
					result.id = bt.red_fencer;
					results.push_back(result);
					red_index = results.size() - 1;
				}

				blue = &results[blue_index];
				red = &results[red_index];
			}

			/* Actual calculation */
			bout_result bout_result_blue;
			bout_result bout_result_red;
			get_bout_results(bt, bout_result_blue, bout_result_red);
			assert(bout_result_blue != invalid_bout_result && bout_result_red != invalid_bout_result && "Received invalid bout result");
			if (bout_result_blue == win)
				++blue->win_count;
			else if (bout_result_blue == draw)
				++blue->draw_count;
			else if (bout_result_blue == loss)
				++blue->loss_count;

			if (bout_result_red == win)
				++red->win_count;
			else if (bout_result_red == draw)
				++red->draw_count;
			else if (bout_result_red == loss)
				++red->loss_count;

			blue->exchange_count += (uint16_t)bt.exchanges.size();
			red->exchange_count += (uint16_t)bt.exchanges.size();
			for (size_t i = 0; i < bt.exchanges.size(); ++i)
			{
				const exchange &exc = bt.exchanges[i];
				/* Double hits do not receive Quality */
				if (exc.dbl != no_double)
					continue;

				uint16_t quality_blue = get_exchange_quality_blue(exc);
				uint16_t quality_red = get_exchange_quality_red(exc);
				
				if (quality_blue == 3)
					++blue->quality3_count;
				if (quality_red == 3)
					++red->quality3_count;

				if (quality_blue != 0 && quality_red == 0)
					++blue->clean_exchange_count;
				if (quality_red != 0 && quality_blue == 0)
					++red->clean_exchange_count;
			}
		}
	}
}