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
		accidental_double,
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

	static uint16_t get_exchange_quality_blue(exchange &exc)
	{
		uint16_t quality = 0;
		if (exc.hit_blue == wound)
			quality = 1;
		else if (exc.hit_blue == incapacitate)
			quality = 2;

		if (quality != 0 && (exc.hit_red == no_hit || exc.hit_red == no_quality))
			++quality;

		return quality;
	}

	static uint16_t get_exchange_quality_red(exchange &exc)
	{
		uint16_t quality = 0;
		if (exc.hit_red == wound)
			quality = 1;
		else if (exc.hit_red == incapacitate)
			quality = 2;

		if (quality != 0 && (exc.hit_blue == no_hit || exc.hit_blue == no_quality))
			++quality;

		return quality;
	}
}