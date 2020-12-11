
#include <time.h>
#include <unistd.h>
#include <sys/system_properties.h>

#include "sys.h"
#include "dxml.h"
#include "sGpio.h"
#include "sMisc.h"
#include "SysWifi.h"

SysWifi wifi;

SysWifi::SysWifi()
{
	m_mode = 0;
}

SysWifi::~SysWifi()
{
}

void SysWifi::load(void)
{
	uint8_t mac[6];
	if (!local_if_mac("eth1", mac, NULL)) {
		m_mode = 1;

		if (local_if_mac("wlan0", mac, NULL)) {
			sGpio io;
			io.start("gpio_para", "charge");
			io.dir("gpio_para", "charge", 1);
			io.set("gpio_para", "charge", 1);

			io.start("wifi_para", "wifi_power");
			io.dir("wifi_para", "wifi_power", 1);
			this->power(0);
			usleep(100*1000);
			this->power(1);
			usleep(200*1000);
			system("insmod /dnake/drv/bcmdhd.ko firmware_path=/system/etc/firmware/fw_bcmdhd_apsta.bin");
			sleep(1);
		}

		system("ifconfig eth1 down");
		char s[256], s2[32];
		strcpy(s2, sys.net.mac()); s2[6] = '2';
		sprintf(s, "busybox ifconfig eth1 hw ether %s", s2);
		system(s);
		system("ifconfig eth1 up");

		this->forward();
	}
}

void SysWifi::process(void)
{
	if (m_mode) {
		static time_t ts = time(NULL);
		if (labs(time(NULL)-ts) > 5) {
			if (local_if_mac("wlan0", NULL, NULL)) {
				//找不到wlan0网卡,强制重启
				system("sync");
				system("reboot");
			}
			ts = time(NULL);
			this->hostapd();
			this->dnsmasq();
			this->dhcpcd();
		}
	}
}

void SysWifi::hostapd(void)
{
	char result[PROP_VALUE_MAX];
	__system_property_get("init.svc.hostapd", result);
	int running = strcmp(result, "running") ? 0 : 1;
	if (!running) {
		this->configure();
		system("ifconfig wlan0 down");
		system("ifconfig wlan0 193.0.0.1 netmask 255.255.255.0 up");
		system("start hostapd");
	}
}

static int eth1_carrier(void)
{
	int st = 0;
	FILE *fp = fopen("/sys/class/net/eth1/carrier", "r");
	if (fp) {
		fscanf(fp, "%d", &st);
		fclose(fp);
	}
	return st;
}

void SysWifi::dhcpcd(void)
{
	if (eth1_carrier()) {
		char result[PROP_VALUE_MAX];
		__system_property_get("init.svc.dhcpcd_eth1", result);
		int running = strcmp(result, "running") ? 0 : 1;
		if (!running) {
			system("start dhcpcd_eth1");
		}
	}
}

void SysWifi::dnsmasq(void)
{
	char result[PROP_VALUE_MAX];
	__system_property_get("init.svc.dnsmasq", result);
	int running = strcmp(result, "running") ? 0 : 1;
	if (!running) {
		system("start dnsmasq");
	}
}

void SysWifi::power(int val)
{
	sGpio io;
	io.set("wifi_para", "wifi_power", val);
}

void SysWifi::configure(void)
{
	FILE *fp = fopen("/data/misc/wifi/hostapd.conf", "w+");
	if (fp) {
		fprintf(fp, "ctrl_interface=/data/system/hostapd\n");
		fprintf(fp, "macaddr_acl=0\n");
		fprintf(fp, "hw_mode=g\n");
		fprintf(fp, "beacon_int=100\n");
		fprintf(fp, "interface=wlan0\n");
		fprintf(fp, "channel=%d\n", (rand()%9)+1);
		fprintf(fp, "ssid=AP_%d-%d-%d%02d\n", sys.talk.build(), sys.talk.unit(), sys.talk.floor(), sys.talk.family());
		fprintf(fp, "wpa_passphrase=ADBC1423\n");
		fprintf(fp, "auth_algs=1\n");
		fprintf(fp, "wpa=2\n");
		fprintf(fp, "wpa_key_mgmt=WPA-PSK\n");
		fprintf(fp, "wpa_pairwise=TKIP\n");
		fprintf(fp, "rsn_pairwise=CCMP\n");
		fprintf(fp, "country_code=CN\n");
		fprintf(fp, "max_num_sta=3\n");
		fclose(fp);
	}
	fp = fopen("/data/misc/wifi/dnsmasq.conf", "w+");
	if (fp) {
		fprintf(fp, "interface=wlan0\n");
		fprintf(fp, "no-daemon\n");
		fprintf(fp, "no-resolv\n");
		fprintf(fp, "no-poll\n");
		fprintf(fp, "dhcp-range=193.0.0.5,193.0.0.10,255.255.255.0,12h\n");
		fprintf(fp, "dhcp-option=3,193.0.0.1\n");
		fprintf(fp, "dhcp-option=option:dns-server,8.8.8.8,114.114.114.114\n");
		fclose(fp);
	}
	if (strcmp(sys.talk.sync(), "962132")) {
		sys.talk.sync("962132");
		sys.save();
	}
}

void SysWifi::forward(void)
{
	system("echo 1 > /proc/sys/net/ipv4/ip_forward");

	system("iptables -t nat -F");
	system("iptables -P INPUT ACCEPT");
	system("iptables -P OUTPUT ACCEPT");
	system("iptables -P FORWARD ACCEPT");
	system("iptables -t nat -A POSTROUTING -o eth0 -jMASQUERADE");
	system("iptables -t nat -A POSTROUTING -o eth1 -jMASQUERADE");
	system("iptables -A FORWARD -i wlan0 -j ACCEPT");
	system("ip rule add fwmark 3 table 3");
}

void SysWifi::dhcpcd_hooks(const char *gateway)
{
	char s[256];
	sprintf(s, "ip route replace table 3 via %s dev eth1", gateway);
	system(s);

	system("iptables -t mangle -F");
	system("iptables -A PREROUTING -t mangle -i wlan0 -j MARK --set-mark 3");
	int ip[2];
	sscanf(sys.net.ip(), "%d.%d", &ip[0], &ip[1]);
	sprintf(s, "iptables -A PREROUTING -t mangle -i wlan0 -d %d.%d.0.0/16 -j MARK --set-mark 4", ip[0], ip[1]);
	system(s);
}
