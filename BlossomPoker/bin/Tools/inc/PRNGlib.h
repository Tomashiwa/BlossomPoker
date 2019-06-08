#pragma once
namespace PRNGlib
{
	namespace xoroshiro128
	{
		#include <stdint.h>

		extern uint64_t s[2];

		static inline uint64_t rotl(const uint64_t x, int k);

		uint64_t next(void);
		void jump(void);
		void long_jump(void);
	}
}

