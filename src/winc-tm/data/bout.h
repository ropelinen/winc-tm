#pragma once

#include "core/include_std_containers.h"
#include "data/exchange.h"

namespace winc
{
	struct bout
	{
		uint16_t id;
		uint16_t blue_fencer;
		uint16_t red_fencer;
		std::vector<exchange> exchanges;
	};

	static void get_blue_fencer_score(bout &bt, uint16_t &q1, uint16_t &q2, uint16_t &q3)
	{
		q1 = 0;
		q2 = 0;
		q3 = 0;
		for (size_t i = 0; i < bt.exchanges.size(); ++i)
		{
			exchange &exc = bt.exchanges[i];
			/* Double hits do not receive Quality */
			if (exc.dbl != no_double)
				continue;

			switch (get_exchange_quality_blue(exc))
			{
			case 0:
				/* nop */
				break;

			case 1:
				++q1;
				break;

			case 2:
				++q2;
				break;

			case 3:
				++q3;
				break;

			default:
				assert(0 && "Invalid hit quality");
				break;
			}
		}
	}

	static void get_red_fencer_score(bout &bt, uint16_t &q1, uint16_t &q2, uint16_t &q3)
	{
		q1 = 0;
		q2 = 0;
		q3 = 0;
		for (size_t i = 0; i < bt.exchanges.size(); ++i)
		{
			exchange &exc = bt.exchanges[i];
			/* Double hits do not receive Quality */
			if (exc.dbl != no_double)
				continue;

			switch (get_exchange_quality_red(exc))
			{
			case 0:
				/* nop */
				break;

			case 1:
				++q1;
				break;

			case 2:
				++q2;
				break;

			case 3:
				++q3;
				break;

			default:
				assert(0 && "Invalid hit quality");
				break;
			}
		}
	}

	static uint16_t get_suicidal_double_count(bout &bt)
	{
		uint16_t sd = 0;
		for (size_t i = 0; i < bt.exchanges.size(); ++i)
		{
			if (bt.exchanges[i].dbl == suicidal_double)
				++sd;
		}

		return sd;
	}

	static uint16_t get_blue_warning_count(bout &bt)
	{
		uint16_t warning_count = 0;
		for (size_t i = 0; i < bt.exchanges.size(); ++i)
		{
			if (bt.exchanges[i].warning_blue)
				++warning_count;
		}

		return warning_count;
	}

	static uint16_t get_red_warning_count(bout &bt)
	{
		uint16_t warning_count = 0;
		for (size_t i = 0; i < bt.exchanges.size(); ++i)
		{
			if (bt.exchanges[i].warning_red)
				++warning_count;
		}

		return warning_count;
	}
}