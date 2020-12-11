
#ifndef __W_MAIN_H__
#define __W_MAIN_H__

#include "mWindow.h"
#include "mButton.h"

class wMain : public mWindow {
public:
	wMain();
	virtual ~wMain();

	void doEvent(mEvent *e);
	void doTimer(void);

	void loadToolbar(void);

private:
	mPixmap m_bkg;
	// mPixmap m_logo;

	mText m_ip_err;
	mText m_ip_err2;
	mText m_name_text;
	mText m_number_text;
	mText m_camera_text;
	mText m_concierge_text;
	mText m_maintenance_text;
	mText m_history_text;
	
	mButton m_history;
	mButton m_concierge;
	mButton m_maintenance;
	mButton m_name;
	mButton m_number;
	mButton m_camera;
	mButton m_setup;

	uint32_t m_ts_st;
	int m_eth_st;
	int m_proxy_st;
	int m_mute_st;
	int m_msg_st;
	int m_defence_st;
	int m_ip_st;

	int m_point;
	int m_x, m_y;
	int m_x2, m_y2;
};

extern mWindow *SysMain;

#endif
