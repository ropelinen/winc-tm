#pragma once

namespace winc
{
	enum hit_quality
	{
		no_hit = 0,
		no_quality,
		wound,
		incapacitate,
	};

	enum double_hit
	{
		no_double = 0,
		accidenta_double,
		suicidal_double,
	};

	struct exchange
	{
		hit_quality hit_blue = no_hit;
		hit_quality hit_red = no_hit;
		double_hit dbl = no_double;
		static const uint16_t warning_max_len = 128;
		bool warning_red = false;
		char warning_red_reason[warning_max_len];
		bool warning_blue = false;
		char warning_blue_reason[warning_max_len];
	};
}