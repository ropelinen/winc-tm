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

	enum bout_result
	{
		invalid_bout_result = 0,
		win,
		draw, 
		loss,
	};

	static void get_blue_fencer_score(const bout &bt, uint16_t &q1, uint16_t &q2, uint16_t &q3)
	{
		q1 = 0;
		q2 = 0;
		q3 = 0;
		for (size_t i = 0; i < bt.exchanges.size(); ++i)
		{
			const exchange &exc = bt.exchanges[i];
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

	static void get_red_fencer_score(const bout &bt, uint16_t &q1, uint16_t &q2, uint16_t &q3)
	{
		q1 = 0;
		q2 = 0;
		q3 = 0;
		for (size_t i = 0; i < bt.exchanges.size(); ++i)
		{
			const exchange &exc = bt.exchanges[i];
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

	static uint16_t get_suicidal_double_count(const bout &bt)
	{
		uint16_t sd = 0;
		for (size_t i = 0; i < bt.exchanges.size(); ++i)
		{
			if (bt.exchanges[i].dbl == suicidal_double)
				++sd;
		}

		return sd;
	}

	static uint16_t get_blue_warning_count(const bout &bt)
	{
		uint16_t warning_count = 0;
		for (size_t i = 0; i < bt.exchanges.size(); ++i)
		{
			if (bt.exchanges[i].warning_blue)
				++warning_count;
		}

		return warning_count;
	}

	static uint16_t get_red_warning_count(const bout &bt)
	{
		uint16_t warning_count = 0;
		for (size_t i = 0; i < bt.exchanges.size(); ++i)
		{
			if (bt.exchanges[i].warning_red)
				++warning_count;
		}

		return warning_count;
	}

	static void get_bout_results(const bout &bt, bout_result &result_blue, bout_result &result_red)
	{
		result_blue = invalid_bout_result;
		result_red = invalid_bout_result;

		if (get_suicidal_double_count(bt) >= 3)
		{
			result_blue = loss;
			result_red = loss;
			return;
		}

		uint16_t blue_warning_count = get_blue_warning_count(bt);
		uint16_t red_warning_count = get_red_warning_count(bt);
		if (blue_warning_count >= 4 || red_warning_count >= 4)
		{
			result_blue = blue_warning_count >= 4 ? loss : win;
			result_red = red_warning_count >= 4 ? loss : win;
			return;
		}

		uint16_t blue_q1, blue_q2, blue_q3;
		get_blue_fencer_score(bt, blue_q1, blue_q2, blue_q3);
		if (blue_warning_count >= 2)
		{
			if (blue_q3 > 0)
				--blue_q3;
			else if (blue_q2 > 0)
				--blue_q2;
			else if (blue_q1 > 0)
				--blue_q1;
		}

		uint16_t red_q1, red_q2, red_q3;
		get_red_fencer_score(bt, red_q1, red_q2, red_q3);
		if (red_warning_count >= 2)
		{
			if (red_q3 > 0)
				--red_q3;
			else if (red_q2 > 0)
				--red_q2;
			else if (red_q1 > 0)
				--red_q1;
		}

		if (blue_q3 > red_q3)
		{
			result_blue = win;
			result_red = loss;
			return;
		}
		else if (blue_q3 < red_q3)
		{
			result_blue = loss;
			result_red = win;
			return;
		}

		if (blue_q2 > red_q2)
		{
			result_blue = win;
			result_red = loss;
			return;
		}
		else if (blue_q2 < red_q2)
		{
			result_blue = loss;
			result_red = win;
			return;
		}

		if (blue_q1 > red_q1)
		{
			result_blue = win;
			result_red = loss;
			return;
		}
		else if (blue_q1 < red_q1)
		{
			result_blue = loss;
			result_red = win;
			return;
		}

		result_blue = draw;
		result_red = draw;
		return;
	}
}