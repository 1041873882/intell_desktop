
#ifndef __W_SETUP_STATUS_H__
#define __W_SETUP_STATUS_H__

#include "mWindow.h"
#include "mText.h"

class wSetStatus : public mWindow {
public:
	wSetStatus();
	virtual ~wSetStatus();

private:
    time_t now;

    mText m_time_date;
    mText m_time_hour;
    mText m_ip_text;
	mText m_ip;
    mText m_mac_text;
	mText m_mac;

    mText m_sip_text;
    mText m_sip;

    mText m_uptime_text;
    mText m_uptime;
    
	mText m_fw_text;
	mText m_fw;
};

#endif
