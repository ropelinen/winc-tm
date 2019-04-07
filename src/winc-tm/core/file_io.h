#pragma once

namespace winc
{
	struct tournament_data;

	bool write_tournament_data(tournament_data &data);
	bool read_tournament_data(tournament_data &data);
}