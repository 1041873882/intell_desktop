#ifndef __SYS_IOCTL_H__
#define __SYS_IOCTL_H__

#include "types.h"
#include "sGpio.h"

class SysIoctl {
public:
	SysIoctl();
	~SysIoctl();

	void process(void);

	//音频切换 0:外放 1:手柄
	void audio(int mode);

public:
	int m_hook; // 0:提机 1:挂机
};

extern SysIoctl sIoctl;

#endif
