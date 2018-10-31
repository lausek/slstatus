/* See LICENSE file for copyright and license details. */
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <X11/Xlib.h>

#include "arg.h"
#include "slstatus.h"
#include "util.h"

struct arg {
	const wchar_t *(*func)();
	const wchar_t *fmt;
	const wchar_t *args;
};

wchar_t buf[1024];
static int done;
static Display *dpy;

#include "config.h"

static void
terminate(const int signo)
{
	(void)signo;

	done = 1;
}

static void
difftimespec(struct timespec *res, struct timespec *a, struct timespec *b)
{
	res->tv_sec = a->tv_sec - b->tv_sec - (a->tv_nsec < b->tv_nsec);
	res->tv_nsec = a->tv_nsec - b->tv_nsec +
	               (a->tv_nsec < b->tv_nsec) * 1E9;
}

static void
usage(void)
{
	die(L"usage: %s [-s]", argv0);
}

int
main(int argc, wchar_t *argv[])
{
	struct sigaction act;
	struct timespec start, current, diff, intspec, wait;
	size_t i, len;
	int sflag, ret;
	wchar_t status[MAXLEN];
	const wchar_t *res;

	sflag = 0;
	ARGBEGIN {
		case 's':
			sflag = 1;
			break;
		default:
			usage();
	} ARGEND

	if (argc) {
		usage();
	}

	memset(&act, 0, sizeof(act));
	act.sa_handler = terminate;
	sigaction(SIGINT,  &act, NULL);
	sigaction(SIGTERM, &act, NULL);

	if (!sflag && !(dpy = XOpenDisplay(NULL))) {
		die(L"XOpenDisplay: Failed to open display");
	}

	while (!done) {
		if (clock_gettime(CLOCK_MONOTONIC, &start) < 0) {
			die(L"clock_gettime:");
		}

		status[0] = '\0';
		for (i = len = 0; i < LEN(args); i++) {
			if (!(res = args[i].func(args[i].args))) {
				res = unknown_str;
			}
			if ((ret = esnprintf(status + len, sizeof(status) - len,
			                    args[i].fmt, res)) < 0) {
				break;
			}
			len += ret;
		}

		if (sflag) {
			puts(status);
			fflush(stdout);
			if (ferror(stdout))
				die(L"puts:");
		} else {
			if (XStoreName(dpy, DefaultRootWindow(dpy), status)
                            < 0) {
				die(L"XStoreName: Allocation failed");
			}
			XFlush(dpy);
		}

		if (!done) {
			if (clock_gettime(CLOCK_MONOTONIC, &current) < 0) {
				die(L"clock_gettime:");
			}
			difftimespec(&diff, &current, &start);

			intspec.tv_sec = interval / 1000;
			intspec.tv_nsec = (interval % 1000) * 1E6;
			difftimespec(&wait, &intspec, &diff);

			if (wait.tv_sec >= 0) {
				if (nanosleep(&wait, NULL) < 0 &&
				    errno != EINTR) {
					die(L"nanosleep:");
				}
			}
		}
	}

	if (!sflag) {
		XStoreName(dpy, DefaultRootWindow(dpy), NULL);
		if (XCloseDisplay(dpy) < 0) {
			die(L"XCloseDisplay: Failed to close display");
		}
	}

	return 0;
}
