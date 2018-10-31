/* See LICENSE file for copyright and license details. */
#include <stdio.h>
#include <stdlib.h>

#include "../util.h"

const wchar_t **
load_avg(void)
{
	double avgs[3];

	if (getloadavg(avgs, 3) < 0) {
		warn(L"getloadavg: Failed to obtain load average");
		return NULL;
	}

	return bprintf(L"%.2f %.2f %.2f", avgs[0], avgs[1], avgs[2]);
}
