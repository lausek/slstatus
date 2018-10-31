/* See LICENSE file for copyright and license details. */
#include <sys/utsname.h>
#include <stdio.h>

#include "../util.h"

const wchar_t **
kernel_release(void)
{
	struct utsname udata;

	if (uname(&udata) < 0) {
		warn(L"uname:");
		return NULL;
	}

	return bprintf(L"%s", udata.release);
}
