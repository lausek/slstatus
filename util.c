/* See LICENSE file for copyright and license details. */
#include <errno.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <wchar.h>

#include "util.h"

wchar_t *argv0;

static void
verr(const wchar_t *fmt, va_list ap)
{
	if (argv0 && wcsncmp(fmt, L"usage", sizeof("usage") - 1)) {
		fwprintf(stderr, L"%s: ", argv0);
	}

	vfwprintf(stderr, fmt, ap);

	if (fmt[0] && fmt[wcslen(fmt) - 1] == ':') {
		fputc(' ', stderr);
		perror(NULL);
	} else {
		fputc('\n', stderr);
	}
}

void
warn(const wchar_t *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	verr(fmt, ap);
	va_end(ap);
}

void
die(const wchar_t *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	verr(fmt, ap);
	va_end(ap);

	exit(1);
}

static int
evsnprintf(wchar_t *str, size_t size, const wchar_t *fmt, va_list ap)
{
	int ret;

	ret = vswprintf(str, size, fmt, ap);

	if (ret < 0) {
		warn(L"vsnprintf:");
		return -1;
	} else if ((size_t)ret >= size) {
		warn(L"vsnprintf: Output truncated");
		return -1;
	}

	return ret;
}

int
esnprintf(wchar_t *str, size_t size, const wchar_t *fmt, ...)
{
	va_list ap;
	int ret;

	va_start(ap, fmt);
	ret = evsnprintf(str, size, fmt, ap);
	va_end(ap);

	return ret;
}

const wchar_t *
bprintf(const wchar_t *fmt, ...)
{
	va_list ap;
	int ret;

	va_start(ap, fmt);
	ret = evsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);

	return (ret < 0) ? NULL : buf;
}

const wchar_t *
fmt_human(uintmax_t num, int base)
{
	double scaled;
	size_t i, prefixlen;
	const wchar_t **prefix;
	const wchar_t *prefix_1000[] = { L"", L"k", L"M", L"G", L"T", L"P", L"E", L"Z",
	                              L"Y" };
	const wchar_t *prefix_1024[] = { L"", L"Ki", L"Mi", L"Gi", L"Ti", L"Pi", L"Ei",
	                              L"Zi", L"Yi" };

	switch (base) {
	case 1000:
		prefix = prefix_1000;
		prefixlen = LEN(prefix_1000);
		break;
	case 1024:
		prefix = prefix_1024;
		prefixlen = LEN(prefix_1024);
		break;
	default:
		warn(L"fmt_human: Invalid base");
		return NULL;
	}

	scaled = num;
	for (i = 0; i < prefixlen && scaled >= base; i++) {
		scaled /= base;
	}

	return bprintf(L"%.1f%s", scaled, prefix[i]);
}

int
pscanf(const wchar_t *path, const wchar_t *fmt, ...)
{
	FILE *fp;
	va_list ap;
	int n;
    char mbyte_path[PATH_MAX]; 

    wcsrtombs(mbyte_path, &path, PATH_MAX, NULL);

	if (!(fp = fopen(mbyte_path, "r"))) {
		warn(L"fopen '%s':", mbyte_path);
		return -1;
	}
	va_start(ap, fmt);
	n = fwscanf(fp, fmt, ap);
	va_end(ap);
	fclose(fp);

	return (n == EOF) ? -1 : n;
}
