
#include "sys.h"
#include "d600.h"
#include "SysEdhcp.h"
#include "SysSound.h"
#include "SysCaller.h"
#include "SysSecurity.h"
#include "SysLogger.h"
#include "SysRs485.h"
#include "SysTpKey.h"
#include "SysSmart.h"
#include "SysIoctl.h"
#include "SysWifi.h"
#include "ipwatchd.h"
#include "mFB.h"
#include "wNtMain.h"
#include "wMain.h"
#include "mqtt.h"

static int boot_b = 0; //锟斤拷台锟斤拷锟斤拷

static void sys_cpufreq_init(void)
{
	//锟斤拷锟斤拷CPU锟斤拷锟�120M
	system("echo 120000 > /sys/devices/system/cpu/cpu0/cpufreq/scaling_min_freq");
	//锟斤拷锟矫讹拷态锟斤拷频
	system("echo interactive > /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor");
}

static void *dnake_misc_thread(void *)
{
	usleep(500*1000);
	if (boot_b) {
		dmsg req;
		req.request("/talk/slave/reset", NULL);
		sys.setEthMac();
	}
	sound.stop();
	sound.load();
	sLogger.load();
	sSecurity.start();
	rs485.start();
	ipwd_start();
	c600.start();
	eDhcp.start();
	sTpKey.start();
	smart.start();
	wifi.load();

	if (sys.mqtt.enable() == 1) {
		mqtt.start();
	}

	while (1) {
		sys.process();
		eDhcp.process();
		ipwd_process();
		sTpKey.process();
		sIoctl.process();
		wifi.process();
		usleep(100*1000);
	}
	return NULL;
}

void ui_main_process(void)
{
	if (fb.m_enabled) {
		if (fb.timeout() >= 8*60*60) {
			//锟斤拷时锟斤拷锟斤拷时锟斤拷偏锟斤拷太锟斤拷,刷锟铰匡拷锟斤拷时锟斤拷
			fb.enable(1);
		} else if (fb.timeout() >= 60) {
			fb.enable(0);
			sys.admin.login = 0;
			sys.user.login = 0;

			//锟斤拷锟斤拷锟斤拷锟皆讹拷锟斤拷转锟斤拷锟斤拷页
			if (SysAlarm == NULL && SysMain == NULL) {
				if (sys.m_limit == 220) {
					wNtMain *w = new wNtMain();
					w->show();
				} else {
					wMain *w = new wMain();
					w->show();
				}
			}
		}
	} else {
		if (fb.enabled()) {
			//锟节猴拷状态锟斤拷UI状态锟斤拷一锟斤拷
			fb.enable(1);
		}
	}

	//锟斤拷锟斤拷锟斤拷锟斤拷8小时锟皆讹拷取锟斤拷
	if (sys.mute.m_enable && labs(time(NULL)-sys.mute.m_ts) >= 8*60*60)
		sys.mute.m_enable = 0;

	sCaller.ui_process();
	sSecurity.ui_process();
}

//extern void *dnake_mqtt_thread(void *);

int main(int argc, const char *argv[])
{
	ui_msg_init();
	sys.load();

	if (sys.m_limit == 220) {
		wNtMain *w = new wNtMain();
		w->show();
	} else {
		wMain *w = new wMain();
		w->show();
	}

	for(int i=1; i<argc; i++) {
		if (!strcmp(argv[i], "-b")) {
			boot_b = 1;
		}
	}
	pthread_t pid;
	if (pthread_create(&pid, NULL, dnake_misc_thread, NULL) != 0)
		perror("pthread_create dnake_misc_thread!\n");
	//if (pthread_create(&pid, NULL, dnake_mqtt_thread, NULL) != 0)

	fprintf(stderr, "desktop start...\n");

	mEvent::process();
	return 0;
}
