/* See LICENSE file for copyright and license details. */
#include <stdio.h>
#include <sys/statvfs.h>

#include "../util.h"

const wchar_t **
disk_free(const wchar_t **path)
{
	struct statvfs fs;

	if (statvfs(path, &fs) < 0) {
		warn(L"statvfs '%s':", path);
		return NULL;
	}

	return fmt_human(fs.f_frsize * fs.f_bavail, 1024);
}

const wchar_t **
disk_perc(const wchar_t **path)
{
	struct statvfs fs;

	if (statvfs(path, &fs) < 0) {
		warn(L"statvfs '%s':", path);
		return NULL;
	}

	return bprintf(L"%d", (int)(100 *
	               (1.0f - ((float)fs.f_bavail / (float)fs.f_blocks))));
}

const wchar_t **
disk_total(const wchar_t **path)
{
	struct statvfs fs;

	if (statvfs(path, &fs) < 0) {
		warn(L"statvfs '%s':", path);
		return NULL;
	}

	return fmt_human(fs.f_frsize * fs.f_blocks, 1024);
}

const wchar_t **
disk_used(const wchar_t **path)
{
	struct statvfs fs;

	if (statvfs(path, &fs) < 0) {
		warn(L"statvfs '%s':", path);
		return NULL;
	}

	return fmt_human(fs.f_frsize * (fs.f_blocks - fs.f_bfree), 1024);
}
