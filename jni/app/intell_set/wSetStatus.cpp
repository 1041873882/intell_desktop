#include "sys.h"
#include "dxml.h"
#include "dmsg.h"
#include "sMisc.h"
#include "wSetStatus.h"

wSetStatus::wSetStatus() : mWindow("intell_set/status")
{
	char s[128];
    now = time(NULL);
    struct tm *tm = localtime(&now);
    

    m_time_date.setParent(this);
    m_time_date.load(m_style, "time_date");
    sprintf(s, "%02d-%02d-%04d", tm->tm_mon+1, tm->tm_mday, tm->tm_year+1900);
    m_time_date.setText(s);

    m_time_hour.setParent(this);
    m_time_hour.load(m_style, "time_hour");
    sprintf(s, "%02d:%02d:%02d", tm->tm_hour, tm->tm_min, tm->tm_sec);
    m_time_hour.setText(s);

    m_ip_text.setParent(this);
	m_ip_text.load(m_style, "ip_text");
	m_ip.setParent(this);
	m_ip.load(m_style, "ip");
	struct sockaddr_in addr;
	if (local_if_addr(&addr))
		m_ip.setText("0.0.0.0");
	else
		m_ip.setText(inet_ntoa(addr.sin_addr));

    m_mac_text.setParent(this);
	m_mac_text.load(m_style, "mac_text");
	m_mac.setParent(this);
	m_mac.load(m_style, "mac");
	m_mac.setText(sys.net.mac());

    m_sip_text.setParent(this);
	m_sip_text.load(m_style, "sip_text");
	m_sip.setParent(this);
	m_sip.load(m_style, "sip");
	m_sip.setText(sys.sip.proxy());

    m_uptime_text.setParent(this);
	m_uptime_text.load(m_style, "uptime_text");

	m_fw_text.setParent(this);
	m_fw_text.load(m_style, "fw_text");
	m_fw.setParent(this);
	m_fw.load(m_style, "fw");

	dmsg req;
	req.request("/talk/version", NULL);
	dxml p(req.body());
	m_fw.setText(p.getText("/params/version"));

	

	
}

wSetStatus::~wSetStatus()
{
}
