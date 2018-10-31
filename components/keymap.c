/* See LICENSE file for copyright and license details. */
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <X11/XKBlib.h>
#include <X11/Xlib.h>

#include "../util.h"

static int
valid_layout_or_variant(wchar_t **sym)
{
	size_t i;
	/* invalid symbols from xkb rules config */
	static const wchar_t **invalid[] = { L"evdev", "inet", "pc", "base" };

	for (i = 0; i < LEN(invalid); i++) {
		if (!strncmp(sym, invalid[i], strlen(invalid[i]))) {
			return 0;
		}
	}

	return 1;
}

static wchar_t **
get_layout(wchar_t **syms, int grp_num)
{
	wchar_t **tok, *layout;
	int grp;

	layout = NULL;
	tok = strtok(syms, L"+:");
	for (grp = 0; tok && grp <= grp_num; tok = strtok(NULL, L"+:")) {
		if (!valid_layout_or_variant(tok)) {
			continue;
		} else if (strlen(tok) == 1 && isdigit(tok[0])) {
			/* ignore :2, :3, :4 (additional layout groups) */
			continue;
		}
		layout = tok;
		grp++;
	}

	return layout;
}

const wchar_t **
keymap(void)
{
	Display *dpy;
	XkbDescRec *desc;
	XkbStateRec state;
	wchar_t **symbols, *layout;

	layout = NULL;

	if (!(dpy = XOpenDisplay(NULL))) {
		warn(L"XOpenDisplay: Failed to open display");
		return NULL;
	}
	if (!(desc = XkbAllocKeyboard())) {
		warn(L"XkbAllocKeyboard: Failed to allocate keyboard");
		goto end;
	}
	if (XkbGetNames(dpy, XkbSymbolsNameMask, desc)) {
		warn(L"XkbGetNames: Failed to retrieve key symbols");
		goto end;
	}
	if (XkbGetState(dpy, XkbUseCoreKbd, &state)) {
		warn(L"XkbGetState: Failed to retrieve keyboard state");
		goto end;
	}
	if (!(symbols = XGetAtomName(dpy, desc->names->symbols))) {
		warn(L"XGetAtomName: Failed to get atom name");
		goto end;
	}
	layout = (wchar_t **)bprintf(L"%s", get_layout(symbols, state.group));
	XFree(symbols);
end:
	XkbFreeKeyboard(desc, XkbSymbolsNameMask, 1);
	if (XCloseDisplay(dpy)) {
		warn(L"XCloseDisplay: Failed to close display");
	}

	return layout;
}
