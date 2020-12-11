
#include <unistd.h>
#include <time.h>
#include <linux/input.h>

#include "sFifo.h"
#include "mFB.h"
#include "mEvent.h"
#include "SysTpKey.h"

extern sFifo eEventData;

#include "sx8661.txt"
#include "cy8cmbr3xxx.txt"

SysTpKey sTpKey;

SysTpKey::SysTpKey()
{
	m_chip = -1;
	m_err = 0;
	m_key = 0;
}

void SysTpKey::start(void)
{
	m_i2c.open("/dev/i2c-1");
	m_i2c.addr(0x2B);
	if (m_i2c.read8(0x00) != 0xFF) {
		m_chip = 0;
		this->init_sx8661();
		return;
	}

	m_i2c.addr(0x37);
	for(int i=0; i<5; i++) {
		if (m_i2c.read8(0x90) == 0x05) {
			m_chip = 1;
			this->init_cy8cmbr3xxx();
			return;
		}
		usleep(50*1000);
	}

	if (m_chip < 0)
		m_i2c.close();
}

void SysTpKey::process(void)
{
	if (m_chip == 0)
		this->do_sx8661();
	else if (m_chip == 1)
		this->do_cy8cmbr3xxx();
}

void SysTpKey::init_sx8661(void)
{
	for(int i=0; i<sizeof(sx8661_default); i+=8) {
		m_i2c.write8(0x0D, 0x10);
		m_i2c.write8(0x0E, i);
		m_i2c.write8(0x00, (uint8_t *)&sx8661_default[i], 8);
		m_i2c.write8(0x0D, 0x00);
	}

#if 0
	//¶Ásx8661 SPMÊý¾Ý
	for(int i=0; i<0x80; i+=8) {
		m_i2c.write8(0x0D, 0x18);
		m_i2c.write8(0x0E, i);
		uint8_t d[8];
		m_i2c.read8(0x00, d, 8);
		m_i2c.write8(0x0D, 0x00);
	}
#endif
	LOGI("mTpKey::init_sx8661\n");
}

void SysTpKey::do_sx8661(void)
{
	int d = m_i2c.read8(0x02);
	if (d != 0xFF) {
		for(int i=0; i<8; i++) {
			int k = (d>>i) & 0x01;
			int k2 = (m_key>>i) & 0x01;
			if (k != k2) {
				LOGI("mTpKey::do_sx8661 key: %d %d\n", i, k);
				this->do_key(i, k);
			}
		}

		m_key = d;
		m_err = 0;
	} else
		m_err++;

	if (m_err >= 10) {
		m_err = 0;
		LOGE("mTpKey::do_sx8661 err!\n");
	}
}

void SysTpKey::do_key(int key, int value)
{
	fb.enable(1);

	struct input_event ev;
	ev.type = EV_KEY;
	ev.code = -1;
	ev.value = value;
	switch(key) {
	case 1:
		ev.code = KEY_M_MONITOR;
		break;
	case 2:
		ev.code = KEY_M_CALL;
		break;
	case 3:
		ev.code = KEY_M_MENU;
		break;
	case 4:
		ev.code = KEY_M_ANSWER;
		break;
	case 5:
		ev.code = KEY_M_UNLOCK;
		break;
	}
	eEventData.put((uint8_t *)&ev, sizeof(ev));
}

void SysTpKey::init_cy8cmbr3xxx(void)
{
	int burn = 0;

	uint8_t d[128];
	m_i2c.read8(0x00, d, 80);
	for(int i=0; i<80; i++) {
		if (d[i] != cy8cmbr3xxx_default[i]) {
			burn = 1;
			break;
		}
	}

	if (burn) {
		m_i2c.write8(0x00, (uint8_t *)cy8cmbr3xxx_default, sizeof(cy8cmbr3xxx_default));
		usleep(40*1000);
		m_i2c.write8(0x86, 0x02);
		usleep(300*1000);
	}
	m_i2c.write8(0x86, 0xFF);

	LOGI("mTpKey::init_cy8cmbr3xxx\n");
}

static const int cy8cmbr3_map[] = {4, 3, 0, 0, 2, 1, 5, 0};
void SysTpKey::do_cy8cmbr3xxx(void)
{
	int d = m_i2c.read8(0xAA);
	if (d != 0xFF) {
		for(int i=0; i<8; i++) {
			int k = (d>>i) & 0x01;
			int k2 = (m_key>>i) & 0x01;
			if (k != k2) {
				LOGI("mTpKey::do_cy8cmbr3xxx key: %d %d\n", i, k);
				this->do_key(cy8cmbr3_map[i], k);
			}
		}

		m_key = d;
		m_err = 0;
	} else
		m_err++;

	if (m_err >= 10) {
		m_err = 0;
		LOGE("mTpKey::do_cy8cmbr3xxx err!\n");
	}
}
