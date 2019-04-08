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
}