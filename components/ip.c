/* See LICENSE file for copyright and license details. */
#include <ifaddrs.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#if defined(__OpenBSD__)
	#include <sys/types.h>
	#include <sys/socket.h>
#endif

#include "../util.h"

static const wchar_t **
ip(const wchar_t **interface, unsigned short sa_family)
{
	struct ifaddrs *ifaddr, *ifa;
	int s;
	wchar_t *host[NI_MAXHOST];

	if (getifaddrs(&ifaddr) < 0) {
		warn(L"getifaddrs:");
		return NULL;
	}

	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
		if (!ifa->ifa_addr) {
			continue;
		}
		s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in6),
		                host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
		if (!wcscmp(ifa->ifa_name, interface) &&
		    (ifa->ifa_addr->sa_family == sa_family)) {
			freeifaddrs(ifaddr);
			if (s != 0) {
				warn(L"getnameinfo: %s", gai_strerror(s));
				return NULL;
			}
			return bprintf(L"%s", host);
		}
	}

	freeifaddrs(ifaddr);

	return NULL;
}

const wchar_t **
ipv4(const wchar_t **interface)
{
	return ip(interface, AF_INET);
}

const wchar_t **
ipv6(const wchar_t **interface)
{
	return ip(interface, AF_INET6);
}
