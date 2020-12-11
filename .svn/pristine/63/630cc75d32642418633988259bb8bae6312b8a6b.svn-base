#ifndef __SYS_WIFI_H__
#define __SYS_WIFI_H__

#include "types.h"

class SysWifi {
public:
	SysWifi();
	~SysWifi();

	void load(void);
	void process(void);

	void hostapd(void);
	void dhcpcd(void);
	void dnsmasq(void);
	void power(int val);

	void configure(void);
	void forward(void);
	void dhcpcd_hooks(const char *gateway);

public:
	int m_mode; //0: 普通分机 1:底座分机
};

extern SysWifi wifi;

#endif
