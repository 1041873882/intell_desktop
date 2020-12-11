
#include "sys.h"
#include "sMisc.h"
#include "SysCaller.h"
#include "SysLogger.h"
#include "SysSecurity.h"
#include "wMessage.h"
#include "wCalibrate.h"
#include "wIntercomCallLabel.h"
#include "wIntercomMonitorLabel.h"
#include "wSmartElevLabel.h"
#include "wSet.h"
#include "wHistory.h"
#include "wName.h"
#include "wNumber.h"
#include "wCamera.h"
#include "wMain.h"

mWindow *SysMain = NULL;

int sys_ipwd_err = 0;
std::string sys_ipwd_ip = "";
std::string sys_ipwd_mac = "";

void sys_ipwd_result(int result, const char *ip, const char *mac)
{
	sys_ipwd_err = result;
	sys_ipwd_ip = ip;
	sys_ipwd_mac = mac;
	if (sys_ipwd_err)
		fb.enable(1);
}

wMain::wMain() : mWindow("intell_main")
{
	// SysMain = this;

	m_bkg.setParent(this);
	m_bkg.load(m_style, "bkg");
	// m_logo.setParent(this);
	// m_logo.load(m_style, "logo");

	// int logo = 0;
	// FILE *fp = fopen("/dnake/etc/logo", "r");
	// if (fp) {
	// 	fscanf(fp, "%d", &logo);
	// 	fclose(fp);
	// }
	// if (!logo)
	// 	m_logo.hide();

	m_bkg.setParent(this);
	m_bkg.load(m_style, "bkg");

	m_name.setParent(this);
	m_name.load(m_style, "name");
	m_name_text.setParent(this);
	m_name_text.setText("Name");
	m_name_text.load(m_style, "name_text");	

	m_number.setParent(this);
	m_number.load(m_style, "number");
	m_number_text.setParent(this);
	m_number_text.setText("Number");
	m_number_text.load(m_style, "number_text");

	m_camera.setParent(this);
	m_camera.load(m_style, "camera");
	m_camera_text.setParent(this);
	m_camera_text.setText("Camera");
	m_camera_text.load(m_style, "camera_text");

	m_concierge.setParent(this);
	m_concierge.load(m_style, "concierge");
	m_concierge_text.setParent(this);
	m_concierge_text.setText("Concierge");
	m_concierge_text.load(m_style, "concierge_text");

	m_maintenance.setParent(this);
	m_maintenance.load(m_style, "maintenance");
	m_maintenance_text.setParent(this);
	m_maintenance_text.setText("Maintenance");
	m_maintenance_text.load(m_style, "maintenance_text");

	m_setup.setParent(this);
	m_setup.load(m_style, "set");
	m_history.setParent(this);
	m_history.load(m_style, "history");

	m_ip_err.setParent(this);
	m_ip_err.load(m_style, "ip_err");
	m_ip_err.hide();
	m_ip_err2.setParent(this);
	m_ip_err2.load(m_style, "ip_err2");
	m_ip_err2.hide();

	m_ts_st = -1;
	m_eth_st = -1;
	m_proxy_st = -1;
	m_mute_st = -1;
	m_msg_st = -1;
	m_defence_st = -1;
	m_ip_st = -1;

	m_point = 0;
	m_x = 10000;
	m_y = 10000;
	m_x2 = 0;
	m_y2 = 0;

	this->loadToolbar();
}

wMain::~wMain()
{
	SysMain = NULL;
}

void wMain::doEvent(mEvent *e)
{
	mWindow::doEvent(e);

	if (e->type == mEvent::TouchEnd) {
		if (e->wParam == m_name.id()) {
			wName *w = new wName();
			w->show();
		} else if (e->wParam == m_number.id()) {
			wNumber *w = new wNumber();
			w->show();
		} else if (e->wParam == m_camera.id()) {
			wCamera *w = new wCamera();
			w->show();
		} else if (e->wParam == m_concierge.id()) {
			
		} else if (e->wParam == m_maintenance.id()) {
			
		} else if (e->wParam == m_history.id()) {
			wHistory *w = new wHistory();
			w->show();
		} else if (e->wParam == m_setup.id()) {
			wSet *w = new wSet();
			w->show();
		}
	} else if (e->type == mEvent::TouchRaw) {
		int m = e->wParam;
		int x = e->lParam&0xFFFF;
		int y = (e->lParam>>16)&0xFFFF;
		if (m == 0) {
			int ok = 0;
			if (m_point > 20) {
				if (m_x2-m_x > 2400 && m_y2-m_y < 300) {
					ok = 1;
				} else if (m_y2-m_y > 2400 && m_x2-m_x < 300) {
					ok = 1;
				}
			}
			m_point = 0;
			m_x = 10000;
			m_y = 10000;
			m_x2 = 0;
			m_y2 = 0;
			if (ok) {
				wCalibrate *w = new wCalibrate();
				w->show();
			}
		} else {
			m_point++;
			if (m_x > x)
				m_x = x;
			if (m_y > y)
				m_y = y;
			if (m_x2 < x)
				m_x2 = x;
			if (m_y2 < y)
				m_y2 = y;
		}
	}
}

void wMain::loadToolbar(void)
{
	if (m_ip_st != sys_ipwd_err) {
		m_ip_st = sys_ipwd_err;
		if (m_ip_st) {
			char s[1024];
			sprintf(s, "%s %s", m_style.getText("/style/text/ip_err"), sys_ipwd_ip.c_str());
			m_ip_err.setText(s);
			m_ip_err.show();
			sprintf(s, "MAC: %s", sys_ipwd_mac.c_str());
			m_ip_err2.setText(s);
			m_ip_err2.show();
		} else {
			m_ip_err.setText("");
			m_ip_err.hide();
			m_ip_err2.setText("");
			m_ip_err2.hide();
		}
	}
}

void wMain::doTimer(void)
{
	mWindow::doTimer();
	this->loadToolbar();
}
