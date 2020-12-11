
#include <unistd.h>
#include "dxml.h"
#include "dmsg.h"
#include "mEvent.h"
#include "SysIoctl.h"

SysIoctl sIoctl;

SysIoctl::SysIoctl()
{
	sGpio io;
	io.start("gpio_para", "audio_sw");
	io.dir("gpio_para", "audio_sw", 1);

	io.start("gpio_para", "hook_det");
	io.dir("gpio_para", "hook_det", 0);
	m_hook = io.get("gpio_para", "hook_det");
	this->audio(0);
}

SysIoctl::~SysIoctl()
{
}

void SysIoctl::process(void)
{
	int d[3], ok = 1;
	sGpio io;

	for(int i=0; i<3; i++) {
		d[i] = io.get("gpio_para", "hook_det");
		if (d[i] != d[0]) {
			ok = 0;
			break;
		}
		usleep(5*1000);
	}
	if (ok) {
		if (m_hook != d[0]) {
			m_hook = d[0];
			LOGI("SysIoctl::process hook: %d\n", m_hook);
			mEvent e(mEventHook);
			e.wParam = m_hook;
			mEvent::post(&e);
			this->audio(m_hook ? 0 : 1);
		}
	}
}

void SysIoctl::audio(int mode)
{
	dmsg req;
	dxml p;
	p.setInt("/params/mode", mode);
	req.request("/talk/handset", p.data());

	sGpio io;
	io.set("gpio_para", "audio_sw", mode);
	LOGI("SysIoctl::audio mode: %d\n", mode);
}
