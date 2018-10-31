/* See LICENSE file for copyright and license details. */
#include <pwd.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#include "../util.h"

const wchar_t **
gid(void)
{
	return bprintf(L"%d", getgid());
}

const wchar_t **
username(void)
{
	struct passwd *pw;

	if (!(pw = getpwuid(geteuid()))) {
		warn(L"getpwuid '%d':", geteuid());
		return NULL;
	}

	return bprintf(L"%s", pw->pw_name);
}

const wchar_t **
uid(void)
{
	return bprintf(L"%d", geteuid());
}
