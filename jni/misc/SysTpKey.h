#ifndef __SYS_TP_KEY_H__
#define __SYS_TP_KEY_H__

#include "types.h"
#include "sGpio.h"

class SysTpKey {
public:
	SysTpKey();

	void start(void);
	void process(void);
	void do_key(int key, int value);
	void init_sx8661(void);
	void do_sx8661(void);
	void init_cy8cmbr3xxx(void);
	void do_cy8cmbr3xxx(void);

public:
	i2c_dev m_i2c;
	int m_chip; //0:SX8661 1:CY8CMBR3116
	int m_err;
	int m_key;
};

extern SysTpKey sTpKey;

#endif
