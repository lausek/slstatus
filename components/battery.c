/* See LICENSE file for copyright and license details. */
#include <stdio.h>
#include <string.h>

#include "../util.h"

#if defined(__linux__)
	#include <limits.h>
	#include <stdint.h>
	#include <unistd.h>
	#include <wchar.h>
	#include <math.h>

    static const wchar_t charge_states[] = { L'▁', L'▂', L'▃', L'▅', L'▆', L'▇' };

	static const wchar_t *
	pick(const wchar_t *bat, const wchar_t *f1, const wchar_t *f2, wchar_t *path,
	     size_t length)
	{
		if (esnprintf(path, length, f1, bat) > 0 &&
		    access(path, R_OK) == 0) {
			return f1;
		}

		if (esnprintf(path, length, f2, bat) > 0 &&
		    access(path, R_OK) == 0) {
			return f2;
		}

		return NULL;
	}

	const wchar_t *
	battery_perc(const wchar_t *bat)
	{
		int perc;
		wchar_t path[PATH_MAX];

		if (esnprintf(path, sizeof(path),
		              L"/sys/class/power_supply/%s/capacity", bat) < 0) {
			return NULL;
		}
		if (pscanf(path, L"%d", &perc) != 1) {
			return NULL;
		}

		return bprintf(L"%d", perc);
	}

	const wchar_t *
	battery_perc_vis(const wchar_t *bat)
	{
		int perc, charge_index;
		wchar_t path[PATH_MAX];

		if (esnprintf(path, sizeof(path),
		              L"/sys/class/power_supply/%s/capacity", bat) < 0) {
			return NULL;
		}
		if (pscanf(path, L"%d", &perc) != 1) {
			return NULL;
		}

        charge_index = round(sizeof(charge_states) * (perc/100));

		return bprintf(L"%c", charge_states[charge_index]);
	}

	const wchar_t *
	battery_state(const wchar_t *bat)
	{
		static struct {
			wchar_t *state;
			wchar_t *symbol;
		} map[] = {
			{ L"Charging",    "+" },
			{ L"Discharging", "-" },
		};
		size_t i;
		wchar_t path[PATH_MAX], state[12];

		if (esnprintf(path, sizeof(path),
		              L"/sys/class/power_supply/%s/status", bat) < 0) {
			return NULL;
		}
		if (pscanf(path, L"%12s", state) != 1) {
			return NULL;
		}

		for (i = 0; i < LEN(map); i++) {
			if (!strcmp(map[i].state, state)) {
				break;
			}
		}
		return (i == LEN(map)) ? L"?" : map[i].symbol;
	}

	const wchar_t *
	battery_remaining(const wchar_t *bat)
	{
		uintmax_t charge_now, current_now, m, h;
		double timeleft;
		wchar_t path[PATH_MAX], state[12];

		if (esnprintf(path, sizeof(path),
		              L"/sys/class/power_supply/%s/status", bat) < 0) {
			return NULL;
		}
		if (pscanf(path, L"%12s", state) != 1) {
			return NULL;
		}

		if (!pick(bat, L"/sys/class/power_supply/%s/charge_now",
		          L"/sys/class/power_supply/%s/energy_now", path,
		          sizeof(path)) ||
		    pscanf(path, L"%ju", &charge_now) < 0) {
			return NULL;
		}

		if (!strcmp(state, L"Discharging")) {
			if (!pick(bat, L"/sys/class/power_supply/%s/current_now",
			          L"/sys/class/power_supply/%s/power_now", path,
			          sizeof(path)) ||
			    pscanf(path, L"%ju", &current_now) < 0) {
				return NULL;
			}

			if (current_now == 0) {
				return NULL;
			}

			timeleft = (double)charge_now / (double)current_now;
			h = timeleft;
			m = (timeleft - (double)h) * 60;

			return bprintf(L"%juh %jum", h, m);
		}

		return L"";
	}
#elif defined(__OpenBSD__)
	#include <fcntl.h>
	#include <machine/apmvar.h>
	#include <sys/ioctl.h>
	#include <unistd.h>

	static int
	load_apm_power_info(struct apm_power_info *apm_info)
	{
		int fd;

		fd = open(L"/dev/apm", O_RDONLY);
		if (fd < 0) {
			warn(L"open '/dev/apm':");
			return 0;
		}

		memset(apm_info, 0, sizeof(struct apm_power_info));
		if (ioctl(fd, APM_IOC_GETPOWER, apm_info) < 0) {
			warn(L"ioctl 'APM_IOC_GETPOWER':");
			close(fd);
			return 0;
		}
		return close(fd), 1;
	}

	const wchar_t *
	battery_perc(const wchar_t *unused)
	{
		struct apm_power_info apm_info;

		if (load_apm_power_info(&apm_info)) {
			return bprintf(L"%d", apm_info.battery_life);
		}

		return NULL;
	}

	const wchar_t *
	battery_state(const wchar_t *unused)
	{
		struct {
			unsigned int state;
			wchar_t *symbol;
		} map[] = {
			{ APM_AC_ON,      L"+" },
			{ APM_AC_OFF,     L"-" },
		};
		struct apm_power_info apm_info;
		size_t i;

		if (load_apm_power_info(&apm_info)) {
			for (i = 0; i < LEN(map); i++) {
				if (map[i].state == apm_info.ac_state) {
					break;
				}
			}
			return (i == LEN(map)) ? L"?" : map[i].symbol;
		}

		return NULL;
	}

	const wchar_t *
	battery_remaining(const wchar_t *unused)
	{
		struct apm_power_info apm_info;

		if (load_apm_power_info(&apm_info)) {
			if (apm_info.ac_state != APM_AC_ON) {
				return bprintf(L"%uh %02um",
			                       apm_info.minutes_left / 60,
				               apm_info.minutes_left % 60);
			} else {
				return L"";
			}
		}

		return NULL;
	}
#endif
