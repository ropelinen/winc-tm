#pragma once

namespace winc
{
	struct fencer_results
	{
		uint16_t id = 0;
		uint16_t exchange_count = 0;
		uint16_t win_count = 0;
		uint16_t draw_count = 0;
		uint16_t loss_count = 0;
		uint16_t quality3_count = 0;
		uint16_t clean_exchange_count = 0;
	};

	static uint16_t get_fencer_bout_count(const fencer_results &results)
	{
		return uint16_t(results.win_count + results.draw_count + results.loss_count);
	}

	static uint16_t get_fencer_matchpoints(const fencer_results &results)
	{
		return uint16_t((results.win_count * 9) + (results.draw_count * 6) + (results.loss_count * 3));
	}

	static float get_fencer_matchpoint_index(const fencer_results &results)
	{
		return float(get_fencer_matchpoints(results)) / float(get_fencer_bout_count(results));
	}

	static float get_fencer_victory_index(const fencer_results &results)
	{
		return float(results.win_count) / float(get_fencer_bout_count(results));
	}

	static float get_fencer_q3_ratio(const fencer_results &results)
	{
		return float(results.quality3_count) / float(results.exchange_count);
	}

	static float get_fencer_clean_ratio(const fencer_results &results)
	{
		return float(results.clean_exchange_count) / float(results.exchange_count);
	}
}