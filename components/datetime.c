/* See LICENSE file for copyright and license details. */
#include <stdio.h>
#include <time.h>

#include "../util.h"

const wchar_t *
datetime(const wchar_t *fmt)
{
	time_t t;

	t = time(NULL);
	if (!wcsftime(buf, sizeof(buf), fmt, localtime(&t))) {
		warn(L"strftime: Result string exceeds buffer size");
		return NULL;
	}

	return buf;
}
