#pragma once

namespace winc
{
	struct tournament_data;
	struct pool;

	void write_pool_to_file(state &state_data, pool &pl, size_t pool_index);
	void write_pool_results_to_file(state &state_data);
	void write_tournament_results_to_file(state &state_data);
	bool write_tournament_data(tournament_data &data);
	bool read_tournament_data(tournament_data &data);
}