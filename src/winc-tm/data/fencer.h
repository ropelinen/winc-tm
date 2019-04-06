#pragma once

namespace winc
{
	struct fencer
	{
		uint16_t id;
		uint16_t hema_rating;
		/* Would be really nice to use actual strings but ImGui is easier to use with char arrays... */
		static const uint16_t max_string_len = 128;
		char name[max_string_len];
		char club[max_string_len];
	};
}