#include "../inc/PRNGLib.h"

namespace PRNGlib
{
	namespace xoroshiro128
	{
		uint64_t s[2];

		static inline uint64_t rotl(const uint64_t x, int k)
		{
			return (x << k) | (x >> (64 - k));
		}

		uint64_t next(void)
		{
			const uint64_t s0 = s[0];
			uint64_t s1 = s[1];
			const uint64_t result = s0 + s1;

			s1 ^= s0;
			s[0] = rotl(s0, 24) ^ s1 ^ (s1 << 16); // a, b
			s[1] = rotl(s1, 37); // c

			return result;
		}

		/* This is the jump function for the generator. It is equivalent
		to 2^64 calls to next(); it can be used to generate 2^64
		non-overlapping subsequences for parallel computations. */

		void jump(void)
		{
			static const uint64_t JUMP[] = { 0xdf900294d8f554a5, 0x170865df4b3201fc };

			uint64_t s0 = 0;
			uint64_t s1 = 0;
			for (int i = 0; i < sizeof JUMP / sizeof *JUMP; i++)
				for (int b = 0; b < 64; b++) {
					if (JUMP[i] & UINT64_C(1) << b) {
						s0 ^= s[0];
						s1 ^= s[1];
					}
					next();
				}

			s[0] = s0;
			s[1] = s1;
		}


		/* This is the long-jump function for the generator. It is equivalent to
		2^96 calls to next(); it can be used to generate 2^32 starting points,
		from each of which jump() will generate 2^32 non-overlapping
		subsequences for parallel distributed computations. */

		void long_jump(void)
		{
			static const uint64_t LONG_JUMP[] = { 0xd2a98b26625eee7b, 0xdddf9b1090aa7ac1 };

			uint64_t s0 = 0;
			uint64_t s1 = 0;
			for (int i = 0; i < sizeof LONG_JUMP / sizeof *LONG_JUMP; i++)
				for (int b = 0; b < 64; b++) {
					if (LONG_JUMP[i] & UINT64_C(1) << b) {
						s0 ^= s[0];
						s1 ^= s[1];
					}
					next();
				}

			s[0] = s0;
			s[1] = s1;
		}
	}
}

