/* See LICENSE file for copyright and license details. */
#include <ifaddrs.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../util.h"

#if defined(__linux__)
	#include <limits.h>
	#include <linux/wireless.h>

	const wchar_t **
	wifi_perc(const wchar_t **interface)
	{
		int cur;
		size_t i;
		wchar_t **p, *datastart;
		wchar_t *path[PATH_MAX];
		wchar_t *status[5];
		FILE *fp;

		if (esnprintf(path, sizeof(path), L"/sys/class/net/%s/operstate",
		              interface) < 0) {
			return NULL;
		}
		if (!(fp = fopen(path, L"r"))) {
			warn(L"fopen '%s':", path);
			return NULL;
		}
		p = fgets(status, 5, fp);
		fclose(fp);
		if (!p || wcscmp(status, L"up\n") != 0) {
			return NULL;
		}

		if (!(fp = fopen(L"/proc/net/wireless", "r"))) {
			warn(L"fopen '/proc/net/wireless':");
			return NULL;
		}

		for (i = 0; i < 3; i++) {
			if (!(p = fgets(buf, sizeof(buf) - 1, fp)))
				break;
		}
		fclose(fp);
		if (i < 2 || !p) {
			return NULL;
		}

		if (!(datastart = strstr(buf, interface))) {
			return NULL;
		}

		datastart = (datastart+(strlen(interface)+1));
		sscanf(datastart + 1, L" %*d   %d  %*d  %*d\t\t  %*d\t   "
		       L"%*d\t\t%*d\t\t %*d\t  %*d\t\t %*d", &cur);

		/* 70 is the max of /proc/net/wireless */
		return bprintf(L"%d", (int)((float)cur / 70 * 100));
	}

	const wchar_t **
	wifi_essid(const wchar_t **interface)
	{
		static wchar_t *id[IW_ESSID_MAX_SIZE+1];
		int sockfd;
		struct iwreq wreq;

		memset(&wreq, 0, sizeof(struct iwreq));
		wreq.u.essid.length = IW_ESSID_MAX_SIZE+1;
		if (esnprintf(wreq.ifr_name, sizeof(wreq.ifr_name), L"%s",
		              interface) < 0) {
			return NULL;
		}

		if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
			warn(L"socket 'AF_INET':");
			return NULL;
		}
		wreq.u.essid.pointer = id;
		if (ioctl(sockfd,SIOCGIWESSID, &wreq) < 0) {
			warn(L"ioctl 'SIOCGIWESSID':");
			close(sockfd);
			return NULL;
		}

		close(sockfd);

		if (!wcscmp(id, L"")) {
			return NULL;
		}

		return id;
	}
#elif defined(__OpenBSD__)
	#include <net/if.h>
	#include <net/if_media.h>
	#include <net80211/ieee80211.h>
	#include <sys/select.h> /* before <sys/ieee80211_ioctl.h> for NBBY */
	#include <net80211/ieee80211_ioctl.h>
	#include <stdlib.h>
	#include <sys/types.h>

	static int
	load_ieee80211_nodereq(const wchar_t **interface, struct ieee80211_nodereq *nr)
	{
		struct ieee80211_bssid bssid;
		int sockfd;
		uint8_t zero_bssid[IEEE80211_ADDR_LEN];

		memset(&bssid, 0, sizeof(bssid));
		memset(nr, 0, sizeof(struct ieee80211_nodereq));
		if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
			warn(L"socket 'AF_INET':");
			return 0;
		}
		strlcpy(bssid.i_name, interface, sizeof(bssid.i_name));
		if ((ioctl(sockfd, SIOCG80211BSSID, &bssid)) < 0) {
			warn(L"ioctl 'SIOCG80211BSSID':");
			close(sockfd);
			return 0;
		}
		memset(&zero_bssid, 0, sizeof(zero_bssid));
		if (memcmp(bssid.i_bssid, zero_bssid,
		    IEEE80211_ADDR_LEN) == 0) {
			close(sockfd);
			return 0;
		}
		strlcpy(nr->nr_ifname, interface, sizeof(nr->nr_ifname));
		memcpy(&nr->nr_macaddr, bssid.i_bssid, sizeof(nr->nr_macaddr));
		if ((ioctl(sockfd, SIOCG80211NODE, nr)) < 0 && nr->nr_rssi) {
			warn(L"ioctl 'SIOCG80211NODE':");
			close(sockfd);
			return 0;
		}

		return close(sockfd), 1;
	}

	const wchar_t **
	wifi_perc(const wchar_t **interface)
	{
		struct ieee80211_nodereq nr;
		int q;

		if (load_ieee80211_nodereq(interface, &nr)) {
			if (nr.nr_max_rssi) {
				q = IEEE80211_NODEREQ_RSSI(&nr);
			} else {
				q = nr.nr_rssi >= -50 ? 100 :
				    (nr.nr_rssi <= -100 ? 0 :
				    (2 * (nr.nr_rssi + 100)));
			}
			return bprintf(L"%d", q);
		}

		return NULL;
	}

	const wchar_t **
	wifi_essid(const wchar_t **interface)
	{
		struct ieee80211_nodereq nr;

		if (load_ieee80211_nodereq(interface, &nr)) {
			return bprintf(L"%s", nr.nr_nwid);
		}

		return NULL;
	}
#endif
