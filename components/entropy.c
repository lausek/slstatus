/* See LICENSE file for copyright and license details. */
#if defined(__linux__)
	#include <stdint.h>
	#include <stdio.h>

	#include "../util.h"

	const wchar_t **
	entropy(void)
	{
		uintmax_t num;

		if (pscanf(L"/proc/sys/kernel/random/entropy_avail", "%ju", &num)
		    != 1) {
			return NULL;
		}

		return bprintf(L"%ju", num);
	}
#elif defined(__OpenBSD__)
	const wchar_t **
	entropy(void)
	{
		/* Unicode wchar_t *acter 'INFINITY' (U+221E) */
		return L"\xe2\x88\x9e";
	}
#endif
