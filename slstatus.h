/* See LICENSE file for copyright and license details. */

/* battery */
const wchar_t *battery_perc(const wchar_t *);
const wchar_t *battery_perc_vis(const wchar_t *);
const wchar_t *battery_state(const wchar_t *);
const wchar_t *battery_remaining(const wchar_t *);

/* cpu */
const wchar_t *cpu_freq(void);
const wchar_t *cpu_perc(void);

/* datetime */
const wchar_t *datetime(const wchar_t *fmt);

/* disk */
const wchar_t *disk_free(const wchar_t *path);
const wchar_t *disk_perc(const wchar_t *path);
const wchar_t *disk_total(const wchar_t *path);
const wchar_t *disk_used(const wchar_t *path);

/* entropy */
const wchar_t *entropy(void);

/* hostname */
const wchar_t *hostname(void);

/* ip */
const wchar_t *ipv4(const wchar_t *interface);
const wchar_t *ipv6(const wchar_t *interface);

/* kernel_release */
const wchar_t *kernel_release(void);

/* keyboard_indicators */
const wchar_t *keyboard_indicators(void);

/* keymap */
const wchar_t *keymap(void);

/* load_avg */
const wchar_t *load_avg(void);

/* netspeeds */
const wchar_t *netspeed_rx(const wchar_t *interface);
const wchar_t *netspeed_tx(const wchar_t *interface);

/* num_files */
const wchar_t *num_files(const wchar_t *path);

/* ram */
const wchar_t *ram_free(void);
const wchar_t *ram_perc(void);
const wchar_t *ram_total(void);
const wchar_t *ram_used(void);

/* run_command */
const wchar_t *run_command(const wchar_t *cmd);

/* swap */
const wchar_t *swap_free(void);
const wchar_t *swap_perc(void);
const wchar_t *swap_total(void);
const wchar_t *swap_used(void);

/* temperature */
const wchar_t *temp(const wchar_t *);

/* uptime */
const wchar_t *uptime(void);

/* user */
const wchar_t *gid(void);
const wchar_t *username(void);
const wchar_t *uid(void);

/* volume */
const wchar_t *vol_perc(const wchar_t *card);

/* wifi */
const wchar_t *wifi_perc(const wchar_t *interface);
const wchar_t *wifi_essid(const wchar_t *interface);
