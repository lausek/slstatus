/* See LICENSE file for copyright and license details. */
#include <dirent.h>
#include <stdio.h>
#include <string.h>

#include "../util.h"

const wchar_t **
num_files(const wchar_t **path)
{
	struct dirent *dp;
	DIR *fd;
	int num;

	if (!(fd = opendir(path))) {
		warn(L"opendir '%s':", path);
		return NULL;
	}

	num = 0;
	while ((dp = readdir(fd))) {
		if (!wcscmp(dp->d_name, L".") || !strcmp(dp->d_name, "..")) {
			continue; /* skip self and parent */
		}
		num++;
	}

	closedir(fd);

	return bprintf(L"%d", num);
}
