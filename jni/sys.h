
#ifndef __SYS_H__
#define __SYS_H__

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <string>

#include "types.h"
#include "dxml.h"
#include "dmsg.h"

#define SYS_MAJOR			1
#define SYS_MINOR			0
#define SYS_MINOR2			0

enum {
	SYS_NORMAL = 0, //分机模式
	SYS_PANEL, //门口机模式
};

enum {
	LANG_CHS = 0,
	LANG_EN,
	LANG_CHT,
	LANG_PL,
	LANG_FR,
	LANG_TUR,
	LANG_RUS,
};

class __sys {
public:
	__sys();
	~__sys();

	void load(void);
	void save(void);

	class __mute {
	public:
		int m_enable;
		time_t m_ts;
	} mute;

	class __user {
	public:
		int login;
		char m_passwd[16];
		int m_language;
		int m_ring;
		int m_ban; //禁止被分机呼叫
		std::string m_tz; //时区

		void passwd(const char *val) { if (val) strcpy(m_passwd, val); };
		const char *passwd(void) { return m_passwd; };
		void tz(const char *val) { m_tz = val; };
		const char *tz(void) { return m_tz.c_str(); };
		void language(int val) { m_language = val; };
		int language(void) { return m_language; };
		void ring(int val) { m_ring = val; };
		int ring(void) { return m_ring; };
		void ban(int val) { m_ban = val; };
		int ban(void) { return m_ban; };
	} user;

	class __admin {
	public:
		int login;
		char m_passwd[16];

		void passwd(const char *val) { if (val) strcpy(m_passwd, val); };
		const char *passwd(void) { return m_passwd; };
	} admin;

	class __net {
	public:
		char m_mac[20];
		int m_dhcp;
		char m_ip[20];
		char m_mask[20];
		char m_gateway[20];
		char m_dns[20];
		char m_ntp[64];

		void mac(const char *val) { if (val) strcpy(m_mac, val); };
		const char *mac(void) { return m_mac; };
		void dhcp(int val) { m_dhcp = val; };
		int dhcp(void) { return m_dhcp; };
		void ip(const char *val) { if (val) strcpy(m_ip, val); };
		const char *ip(void) { return m_ip; };
		void mask(const char *val) { if (val) strcpy(m_mask, val); };
		const char *mask(void) { return m_mask; };
		void gateway(const char *val) { if (val) strcpy(m_gateway, val); };
		const char *gateway(void) { return m_gateway; };
		void dns(const char *val) { if (val) strcpy(m_dns, val); };
		const char *dns(void) { return m_dns; };
		void ntp(const char *val) { if (val) strcpy(m_ntp, val); };
		const char *ntp(void) { return m_ntp; };
	} net;

	class __talk {
	public:
		int m_build; //栋号
		int m_unit; //单元号
		int m_floor; //楼层号
		int m_family; //户主号
		int m_dcode; //设备码
		char m_sync[16]; //同步码
		char m_server[32];
		char m_passwd[32];
		int m_arp;
		int m_answer; //自动接听

		void build(int val) { m_build = val; };
		int build(void) { return m_build; };
		void unit(int val) { m_unit = val; };
		int unit(void) { return m_unit; };
		void floor(int val) { m_floor = val; };
		int floor(void) { return m_floor; };
		void family(int val) { m_family = val; };
		int family(void) { return m_family; };
		void dcode(int val) { m_dcode = val; };
		int dcode(void) { return m_dcode; };
		void sync(const char *val) { if (val) strcpy(m_sync, val); };
		const char *sync(void) { return m_sync; };
		void server(const char *val) { if (val) strcpy(m_server, val); };
		const char *server(void) { return m_server; };
		void passwd(const char *val) { if (val) strcpy(m_passwd, val); };
		const char *passwd(void) { return m_passwd; };
		void arp(int val) { m_arp = val; };
		int arp(void) { return m_arp; };
		void answer(int val) { m_answer = val; };
		int answer(void) { return m_answer; };
	} talk;

	class __sip {
	public:
		int m_enable;
		char m_proxy[64];
		char m_realm[32];
		char m_user[32];
		char m_passwd[32];
		int m_timeout; //通话时间
		int m_host2id;

		void enable(int val) { m_enable = val; };
		int enable(void) { return m_enable; };
		void proxy(const char *val) { if (val) strcpy(m_proxy, val); };
		const char *proxy(void) { return m_proxy; };
		void realm(const char *val) { if (val) strcpy(m_realm, val); };
		const char *realm(void) { return m_realm; };
		void user(const char *val) { if (val) strcpy(m_user, val); };
		const char *user(void) { return m_user; };
		void passwd(const char *val) { if (val) strcpy(m_passwd, val); };
		const char *passwd(void) { return m_passwd; };
		void host2id(int val) { m_host2id = val; };
		int host2id(void) { return m_host2id; };
		void timeout(int val) { m_timeout = val; };
		int timeout(void) { return m_timeout; };
	} sip;

	class __stun {
	public:
		char m_ip[20];
		int m_port;

		void ip(const char *val) { if (val) strcpy(m_ip, val); };
		const char *ip(void) { return m_ip; };
		void port(int val) { m_port = val; };
		int port(void) { return m_port; };
	} stun;

	class __panel {
	public:
		__panel() { m_mode = 0; m_index = 1; };

		int m_mode;
		int m_index;

		void mode(int val) { m_mode = val; };
		int mode(void) { return m_mode; };
		void index(int val) { m_index = val; };
		int index(void) { return m_index; };
	} panel;

	class __volume {
	public:
		int m_talk;
		int m_music;

		void talk(int val) { m_talk = val; };
		int talk(void) { return m_talk; };
		void music(int val) { m_music = val; };
		int music(void) { return m_music; };
	} volume;

	class __payload {
	public:
		int m_h264;

		void h264(int val) { m_h264 = val; };
		int h264(void) { return m_h264; };
	} payload;

	class __lcd {
	public:
		int m_bright;
		void bright(int val) { m_bright = val; };
		int bright(void) { return m_bright; };
	} lcd;

	class __dtmf {
	public:
		int m_enable;
		char m_data[32];

		void enable(int val) { m_enable = val; };
		int enable(void) { return m_enable; };
		void data(const char *val) { if (val) strcpy(m_data, val); };
		const char *data(void) { return m_data; };
	} dtmf;

	class __mqtt {
	public:
		int m_enable;
		char m_url[64];
		int m_port;
		char m_user[64];
		char m_password[64];
		char m_group[64];

		void enable(int val) { m_enable = val; };
		int enable(void) { return m_enable; };
		void url(const char *val) { if (val && strlen(val) < 64) strcpy(m_url, val);}
		const char *url(void) { return m_url; };
		void port(int val) { m_port = val; };
		int port(void) { return m_port; };
		void user(const char *val) { if (val && strlen(val) < 64) strcpy(m_user, val);}
		const char *user(void) { return m_user; };
		void password(const char *val) { if (val && strlen(val) < 64) strcpy(m_password, val);}
		const char *password(void) { return m_password; };
		void group(const char *val) { if (val && strlen(val) < 64) strcpy(m_group, val); }
		const char *group(void) { return m_group; };
	} mqtt;

	void setEthIp(void);
	void setEthMac(void);
	void setHttpUser(void);
	void setTZ(void);
	void resetMac(void);

	void version(char *s);
	const char *style(void);

	void process(void);

public:
	int m_limit;
};

extern __sys sys;

extern int sys_ipwd_err;

extern int sys_mode;

extern int sys_factory;

int ui_msg_init(void);

void sys_ipwd_result(int result, const char *ip, const char *mac);

#endif
