/* See LICENSE file for copyright and license details. */
#include <stdint.h>
#include <wchar.h>

extern wchar_t buf[1024];

#define LEN(x) (sizeof (x) / sizeof *(x))

extern wchar_t *argv0;

void warn(const wchar_t *, ...);
void die(const wchar_t *, ...);

int esnprintf(wchar_t *str, size_t size, const wchar_t *fmt, ...);
const wchar_t *bprintf(const wchar_t *fmt, ...);
const wchar_t *fmt_human(uintmax_t num, int base);
int pscanf(const wchar_t *path, const wchar_t *fmt, ...);
