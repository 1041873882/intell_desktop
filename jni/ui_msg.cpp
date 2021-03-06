
#include "dxml.h"
#include "dmsg.h"
#include "sys.h"
#include "wTalk.h"
#include "SysWifi.h"
#include "SysSmart.h"
#include "SysLogger.h"
#include "SysSecurity.h"
#include "SysSmart.h"
#include "SysCaller.h"

static void ui_run(const char *body)
{
	dmsg_ack(200, NULL);
}

static void ui_version(const char *body)
{
	char s[128];
	sys.version(s);

	dxml p;
	p.setText("/params/version", s);
	p.setInt("/params/proxy", sCaller.m_proxy);
	dmsg_ack(200, p.data());
}

static void ui_talk_start(const char *body)
{
	dmsg_ack(200, NULL);
	int ok = 1;
	dxml p(body);
	const char *host = p.getText("/params/host");
	if (sys.user.ban()) { //禁止被分机呼叫
		host2id_t *id = sCaller.host2id(host);
		if (id != NULL) {
			if (id->build == 0 && id->unit == 0 && id->floor == 0) { // 管理机
			} else if (id->floor == 99 && id->build > 0) { // 单元门口机
			} else if (id->floor == 99 && id->build == 0) { // 围墙机
			} else if (strlen(id->host) < 11) { //分机
				if (id->build != sys.talk.build() || id->unit != sys.talk.unit() ||
				    id->floor != sys.talk.floor() || id->family != sys.talk.family()) {
					ok = 0;
				}
			}
		}
	}
	if (ok) {
		sCaller.ringing(host);
	} else {
		dmsg req;
		req.request("/talk/stop", NULL);
	}
}

static void ui_talk_play(const char *body)
{
	dmsg_ack(200, NULL);
	dxml p(body);
	const char *host = p.getText("/params/host");
	sCaller.play(host);
}

static void ui_talk_stop(const char *body)
{
	dmsg_ack(200, NULL);
	sCaller.stop();
}

static void ui_talk_snapshot(const char *body)
{
	dmsg_ack(200, NULL);
	if (SysTalk) {
		static std::string url;

		dxml p(body);
		const char *u = p.getText("/params/url");
		if (u) {
			url = u;
			mEvent e(mEventJpeg);
			e.wParam = (uint32_t)url.c_str();
			mEvent::post(&e);
		}
	}
}

static void ui_device_query(const char *body)
{
	dmsg_ack(200, NULL);

	dxml p(body);
	const char *host = p.getText("/params/name");
	const char *ip = p.getText("/params/ip");
	if (sCaller.d600.host == NULL && host && ip) {
		strcpy(sCaller.d600.__host, host);
		strcpy(sCaller.d600.__ip, ip);
		sCaller.d600.host = sCaller.d600.__host;
		sCaller.d600.ip = sCaller.d600.__ip;
		printf("host: %s\n", sCaller.d600.host);
		printf("ip: %s\n", sCaller.d600.ip);
	}
}

static void ui_talk_busy(const char *body)
{
	dmsg_ack(200, NULL);

	dxml p(body);
	const char *host = p.getText("/params/host");
	if (host) {
		static std::string s_host;

		s_host = host;
		mEvent e(mEventBusy);
		e.wParam = (uint32_t)s_host.c_str();
		mEvent::post(&e);
	}
}

static void ui_sip_query(const char *body)
{
	dmsg_ack(200, NULL);

	dxml p(body);
	const char *url = p.getText("/params/url");
	if (sCaller.sip.url == NULL && url) {
		strcpy(sCaller.sip.__url, url);
		sCaller.sip.url = sCaller.sip.__url;
		printf("url: %s\n", sCaller.sip.url);
	}
}

static void ui_sip_ringing(const char *body)
{
	dmsg_ack(200, NULL);

	dxml p(body);
	const char *id = p.getText("/params/host");
	sCaller.calling(id);
}

static void ui_sip_register(const char *body)
{
	dmsg_ack(200, NULL);
	dxml p(body);
	sCaller.m_proxy = p.getInt("/params/register", 0);
}

static void ui_sip_result(const char *body)
{
	dmsg_ack(200, NULL);
	dxml p(body);
	sCaller.m_result = p.getInt("/params/result", 0);
}

static void ui_talk_host2id(const char *body)
{
	dmsg_ack(200, NULL);
	dxml p(body);
	const char *host = p.getText("/params/host");
	if (host) {
		host2id_t id;
		strcpy(id.host, host);
		id.build = p.getInt("/params/building", 0);
		id.unit = p.getInt("/params/unit", 0);
		id.floor = p.getInt("/params/floor", 0);
		id.family = p.getInt("/params/family", 0);
		sCaller.host2id(&id);
	}
}

static void ui_slave_device(const char *body)
{
	dmsg_ack(200, NULL);
	dxml p(body);
	sCaller.slaves(p);
}

static void ui_monitor_start(const char *body)
{
	dmsg_ack(200, NULL);

	mEvent e(mEventMonitor);
	e.wParam = 1;
	mEvent::post(&e);
}

static void ui_monitor_stop(const char *body)
{
	dmsg_ack(200, NULL);

	mEvent e(mEventMonitor);
	e.wParam = 0;
	mEvent::post(&e);
}

static void ui_center_text(const char *body)
{
	dmsg_ack(200, NULL);

	dxml p(body);
	const char *data = p.getText("/params/data");
	sLogger.msg.insert(data, time(NULL));
}

#include "wSmartElevLabel.h"
static void ui_smart_elev_data(const char *body)
{
	dmsg_ack(200, NULL);

	dxml p(body);
	uint8_t n = p.getInt("/params/index", 0);
	if (n < 3) {
		sys_elev[n].direct = p.getInt("/params/direct", 0);
		sys_elev[n].sign = p.getInt("/params/sign", 1);
		sys_elev[n].text = p.getText("/params/display", "err");

		mEvent e(mEventElev);
		e.wParam = n;
		mEvent::post(&e);
	}
}

static void ui_smart_control(const char *body)
{
	dmsg_ack(200, NULL);
	dxml p(body);
	smart.rControl(p);
}

static void ui_smart_data(const char *body)
{
	dmsg_ack(200, NULL);
	dxml p(body);
	smart.rData(p);
}

static void ui_security_conf(const char *body)
{
	dmsg_ack(200, NULL);

	dxml p(body);
	sSecurity.sync_load(p);
}

static void ui_security_io(const char *body)
{
	dmsg_ack(200, NULL);

	dxml p(body);
	int io[MAX_SECURITY_SZ];
	for(int i=0; i<MAX_SECURITY_SZ; i++) {
		char s[128];
		sprintf(s, "/params/io%d", i);
		io[i] = p.getInt(s, 0x10);
	}
	sSecurity.process(io, MAX_SECURITY_SZ);
}

static void ui_dhcpcd_hooks(const char *body)
{
	dmsg_ack(200, NULL);

	dxml p(body);
	const char *ifc = p.getText("/params/ifc");
	const char *ip = p.getText("/params/ip");
	const char *mask = p.getText("/params/mask");
	const char *gateway = p.getText("/params/gateway");
	const char *dns = p.getText("/params/dns");
	char s[256], dns1[32], dns2[32];
	sprintf(s, "ifconfig %s 0.0.0.0", ifc);
	system(s);
	sprintf(s, "ifconfig %s %s netmask %s up", ifc, ip, mask);
	system(s);
	sscanf(dns, "%s %s", dns1, dns2);
	sprintf(s, "setprop net.dns1 %s", dns1);
	system(s);
	sprintf(s, "setprop net.dns2 %s", dns2);
	system(s);

	if (!strcmp(ifc, "eth1")) {
		wifi.dhcpcd_hooks(gateway);
	} else {
		sprintf(s, "route add default gw %s dev %s", gateway, ifc);
		system(s);
	}
}

static void ui_web_network_read(const char *body)
{
	dxml p;
	p.setInt("/params/dhcp", sys.net.dhcp());
	p.setText("/params/ip", sys.net.ip());
	p.setText("/params/mask", sys.net.mask());
	p.setText("/params/gateway", sys.net.gateway());
	p.setText("/params/dns", sys.net.dns());
	dmsg_ack(200, p.data());
}

static void ui_web_network_write(const char *body)
{
	dmsg_ack(200, NULL);
	dxml p(body);
	sys.net.dhcp(p.getInt("/params/dhcp", 0));
	sys.net.ip(p.getText("/params/ip"));
	sys.net.mask(p.getText("/params/mask"));
	sys.net.gateway(p.getText("/params/gateway"));
	sys.net.dns(p.getText("/params/dns"));
	sys.save();
	sys.setEthIp();
}

static void ui_web_room_read(const char *body)
{
	dxml p;
	p.setInt("/params/build", sys.talk.build());
	p.setInt("/params/unit", sys.talk.unit());
	p.setInt("/params/floor", sys.talk.floor());
	p.setInt("/params/family", sys.talk.family());
	p.setInt("/params/dcode", sys.talk.dcode());
	p.setText("/params/sync", sys.talk.sync());
	p.setText("/params/server", sys.talk.server());
	p.setText("/params/passwd", sys.talk.passwd());
	dmsg_ack(200, p.data());
}

static void ui_web_room_write(const char *body)
{
	dmsg_ack(200, NULL);
	dxml p(body);
	sys.talk.build(p.getInt("/params/build", 1));
	sys.talk.unit(p.getInt("/params/unit", 1));
	sys.talk.floor(p.getInt("/params/floor", 11));
	sys.talk.family(p.getInt("/params/family", 12));
	sys.talk.dcode(p.getInt("/params/dcode", 0));
	sys.talk.sync(p.getText("/params/sync"));
	sys.talk.server(p.getText("/params/server"));
	sys.talk.passwd(p.getText("/params/passwd"));
	sys.save();
}

static void ui_web_voip_read(const char *body)
{
	dxml p;
	p.setInt("/params/enable", sys.sip.enable());
	p.setText("/params/proxy", sys.sip.proxy());
	p.setText("/params/realm", sys.sip.realm());
	p.setText("/params/user", sys.sip.user());
	p.setText("/params/passwd", sys.sip.passwd());
	p.setInt("/params/host2id", sys.sip.host2id());
	p.setText("/params/stun/ip", sys.stun.ip());
	p.setInt("/params/stun/port", sys.stun.port());
	p.setInt("/params/timeout", sys.sip.timeout());
	p.setInt("/params/payload/h264", sys.payload.h264());
	dmsg_ack(200, p.data());
}

static void ui_web_voip_write(const char *body)
{
	dmsg_ack(200, NULL);
	dxml p(body);
	sys.sip.enable(p.getInt("/params/enable", 0));
	sys.sip.proxy(p.getText("/params/proxy"));
	sys.sip.realm(p.getText("/params/realm"));
	sys.sip.user(p.getText("/params/user"));
	sys.sip.passwd(p.getText("/params/passwd"));
	sys.sip.host2id(p.getInt("/params/host2id", 1));
	sys.stun.ip(p.getText("/params/stun/ip"));
	sys.stun.port(p.getInt("/params/stun/port", 5060));
	sys.sip.timeout(p.getInt("/params/timeout", 300));
	sys.payload.h264(p.getInt("/params/payload/h264", 102));
	sys.save();
}

static void ui_web_ipc_read(const char *body)
{
	dxml p;
	p.setInt("/params/max", sSecurity.ipc.m_idx);
	for(int i=0; i<sSecurity.ipc.m_idx; i++) {
		char s[128];
		sprintf(s, "/params/r%d/name", i);
		p.setText(s, sSecurity.ipc.m_name[i].c_str());
		sprintf(s, "/params/r%d/url", i);
		p.setText(s, sSecurity.ipc.m_url[i].c_str());
	}
	dmsg_ack(200, p.data());
}

static void ui_web_ipc_write(const char *body)
{
	dmsg_ack(200, NULL);

	dxml p(body);
	sSecurity.ipc.m_idx = p.getInt("/params/max", 0);
	for(int i=0; i<sSecurity.ipc.m_idx; i++) {
		char s[128];
		sprintf(s, "/params/r%d/name", i);
		sSecurity.ipc.m_name[i] = p.getText(s, "");
		sprintf(s, "/params/r%d/url", i);
		sSecurity.ipc.m_url[i] = p.getText(s, "");
	}
	sSecurity.ipc.save();
}

static void ui_web_advanced2_read(const char *body)
{
	dxml p;
	p.setText("/params/mac", sys.net.mac());
	dmsg_ack(200, p.data());
}

static void ui_web_advanced2_write(const char *body)
{
	dmsg_ack(200, NULL);

	dxml p(body);
	sys.net.mac(p.getText("/params/mac"));
	sys.save();
	sys.setEthMac();
}

static void ui_web_advanced_read(const char *body)
{
	dxml p;

	p.setInt("/params/unlock/dtmf/enable", sys.dtmf.enable());
	p.setText("/params/unlock/dtmf/data", sys.dtmf.data());

	dmsg_ack(200, p.data());
}

static void ui_web_advanced_write(const char *body)
{
	dmsg_ack(200, NULL);
	dxml p(body);

	sys.dtmf.enable(p.getInt("/params/unlock/dtmf/enable", 0));
	sys.dtmf.data(p.getText("/params/unlock/dtmf/data"));
	sys.save();
}

#include "mqtt.h"
static void ui_mqtt_publish(const char *body)
{
	dmsg_ack(200, NULL);
	dxml p(body);
	const char *topic = p.getText("/params/topic");
	if (topic) {
		if (strcmp(topic, "alarm") == 0) {
			mqtt.alarm(2, 1);
		} else if (strcmp(topic, "defence") == 0) {
			mqtt.defence(2);
		} else if (strcmp(topic, "talk") == 0) {
			mqtt.talk_logger("1019901", 1, 15582, 123456);
		}
	}
}

int ui_msg_init(void)
{
	if (dmsg_init("/ui"))
		exit(-1);
	dmsg_setup("/ui/run", ui_run);
	dmsg_setup("/ui/version", ui_version);
	dmsg_setup("/ui/talk/start", ui_talk_start);
	dmsg_setup("/ui/talk/play", ui_talk_play);
	dmsg_setup("/ui/talk/stop", ui_talk_stop);
	dmsg_setup("/ui/talk/busy", ui_talk_busy);
	dmsg_setup("/ui/talk/snapshot", ui_talk_snapshot);
	dmsg_setup("/ui/talk/host2id", ui_talk_host2id);
	dmsg_setup("/ui/device/query", ui_device_query);
	dmsg_setup("/ui/sip/query", ui_sip_query);
	dmsg_setup("/ui/sip/ringing", ui_sip_ringing);
	dmsg_setup("/ui/sip/register", ui_sip_register);
	dmsg_setup("/ui/sip/result", ui_sip_result);
	dmsg_setup("/ui/slave/device", ui_slave_device);
	dmsg_setup("/ui/slaves", ui_slave_device);

	dmsg_setup("/ui/monitor/start", ui_monitor_start);
	dmsg_setup("/ui/monitor/stop", ui_monitor_stop);

	dmsg_setup("/ui/center/text", ui_center_text);
	dmsg_setup("/ui/smart/elev/data", ui_smart_elev_data);
	dmsg_setup("/ui/smart/control", ui_smart_control);
	dmsg_setup("/ui/smart/data", ui_smart_data);

	dmsg_setup("/ui/security/conf", ui_security_conf);
	dmsg_setup("/ui/security/io", ui_security_io);
	dmsg_setup("/ui/dhcpcd/hooks", ui_dhcpcd_hooks);

	dmsg_setup("/ui/web/network/read", ui_web_network_read);
	dmsg_setup("/ui/web/network/write", ui_web_network_write);
	dmsg_setup("/ui/web/room/read", ui_web_room_read);
	dmsg_setup("/ui/web/room/write", ui_web_room_write);
	dmsg_setup("/ui/web/voip/read", ui_web_voip_read);
	dmsg_setup("/ui/web/voip/write", ui_web_voip_write);
	dmsg_setup("/ui/web/ipc/read", ui_web_ipc_read);
	dmsg_setup("/ui/web/ipc/write", ui_web_ipc_write);
	dmsg_setup("/ui/web/advanced2/read", ui_web_advanced2_read);
	dmsg_setup("/ui/web/advanced2/write", ui_web_advanced2_write);
	dmsg_setup("/ui/web/advanced/read", ui_web_advanced_read);
	dmsg_setup("/ui/web/advanced/write", ui_web_advanced_write);

	dmsg_setup("/ui/mqtt/publish", ui_mqtt_publish);

	return 0;
}
