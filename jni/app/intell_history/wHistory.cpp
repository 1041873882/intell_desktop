
#include "SysCaller.h"
#include "SysLogger.h"
#include "mFB.h"
#include "SysCaller.h"
#include "wIntercom.h"
#include "wHistory.h"
#include "wMain.h"

wHistory::wHistory() : mWindow("intell_history")
{
	m_bkg.setParent(this);
	m_bkg.load(m_style, "bkg");
	m_icon.setParent(this);
	m_icon.load(m_style, "icon");

    m_title.setParent(this);
    m_title.load(m_style, "title");

	m_offset = m_style.getInt("/style/offset", 69);
	for(int i=0; i<7; i++) {
        m_line[i].setParent(this);
		m_line[i].load(m_style, "line");
		m_line[i].move(m_line[0].x(), m_line[0].y()+i*m_offset);
	}

    for(int i=0; i<MAX_LOGGER_LINE; i++) {
        m_time[i].setParent(this);
        m_time[i].load(m_style, "time");
        m_time[i].move(m_time[0].x(), m_time[0].y()+i*m_offset);

		m_host[i].setParent(this);
		m_host[i].load(m_style, "host");
		m_host[i].move(m_host[0].x(), m_host[0].y()+i*m_offset);

		m_date[i].setParent(this);
		m_date[i].load(m_style, "date");
		m_date[i].move(m_date[0].x(), m_date[0].y()+i*m_offset);   

		m_call[i].setParent(this);
		m_call[i].load(m_style, "call");
		m_call[i].move(m_call[0].x(), m_call[0].y()+i*m_offset);  
		m_call[i].hide();

		m_del[i].setParent(this);
		m_del[i].load(m_style, "del");
		m_del[i].move(m_del[0].x(), m_del[0].y()+i*m_offset);  
		m_del[i].hide();
    }

	m_prompt.setParent(this);
	m_prompt.load(m_style, "prompt");
	m_prompt.hide();

	// m_call.setParent(this);
	// m_call.load(m_style, "call");
	m_up.setParent(this);
	m_up.load(m_style, "up");
	m_down.setParent(this);
	m_down.load(m_style, "down");
	// m_del.setParent(this);
	// m_del.load(m_style, "del");

	m_exit.setParent(this);
	m_exit.load(m_style, "exit");

	m_ts = 0;
	m_start = 0;
	sLogger.talk.m_NA = 0;
	m_first = 0;
	this->showData(m_first);
}

wHistory::~wHistory()
{
}

void wHistory::showData(int n)
{
	m_max = MAX_LOGGER_LINE;
	if (n+MAX_LOGGER_LINE > sLogger.talk.m_sz)
		m_max = sLogger.talk.m_sz-n;

	for(int i=0; i<MAX_LOGGER_LINE; i++) {
		m_host[i].setColor(0xFFFFFF);
		m_time[i].setColor(0xFFFFFF);
		m_date[i].setColor(0xFFFFFF);

		if (i < m_max) {
			char s[128];

			m_host[i].setText(sLogger.talk.data[n+i].id);
			m_host[i].show();

			time_t ts = sLogger.talk.data[n+i].ts;
			struct tm *tm = localtime(&ts);
			sprintf(s, "%d-%d-%d", tm->tm_year%100, tm->tm_mon+1, tm->tm_mday);
			m_date[i].setText(s);
			m_date[i].show();

			sprintf(s, "%02d:%02d:%02d", tm->tm_hour, tm->tm_min, tm->tm_sec);
			m_time[i].setText(s);
			m_time[i].show();

			m_call[i].show();
			m_del[i].show();

		} else {
			m_time[i].hide();
			m_host[i].hide();
			m_date[i].hide();
			m_call[i].hide();
			m_del[i].hide();
		}
	}
	m_select = -1;
}

void wHistory::doEvent(mEvent *e)
{
	mWindow::doEvent(e);
	if (e->type == mEvent::TouchEnd) {
		for (int i=0; i<MAX_LOGGER_LINE; i++) {
			if (e->wParam == m_call[i].id()) {
				m_select = i;
				if (!m_start && m_select != -1) {
					int n = i + m_first;
					const char *id = sLogger.talk.data[n].id.c_str();
					if (id[0] >= '0' && id[0] <= '9') {
						sCaller.query(id);
						m_start = 1;
						this->showPrompt(1, NULL);
					} else if (id[0] == 'M' || id[0] == 'H' || id[0] == 'W' || id[0] == 'P') {
					return;
					} else {
						sCaller.sip.url = NULL;
						sCaller.q600(id);
						m_start = 1;
						this->showPrompt(3, id);
					}
				}
			} else if (e->wParam == m_del[i].id()) {
				m_select = i;
				if (m_select != -1) {
					sLogger.talk.remove(i + m_first);
					if (m_first >= sLogger.talk.m_sz)
						m_first = 0;
					this->showData(m_first);
				}
			}
		}

		if (e->wParam == m_exit.id()) {
			sCaller.stop();
			wMain *w = new wMain();
			w->show();
		} else if (e->wParam == m_up.id()) {
			if (m_first-MAX_LOGGER_LINE >= 0) {
				m_first -= MAX_LOGGER_LINE;
				this->showData(m_first);
			}
		} else if (e->wParam == m_down.id()) {
			if (m_first+MAX_LOGGER_LINE < sLogger.talk.m_sz) {
				m_first += MAX_LOGGER_LINE;
				this->showData(m_first);
			}
		} 
	}
}

void wHistory::doTimer(void)
{
	mWindow::doTimer();

	if (m_start) {
		if (sCaller.mode() == sCaller.NONE) {
			this->showPrompt(2, NULL);
		} else if (sCaller.mode() == sCaller.QUERY) {
			if (sCaller.sip.url) {
				sCaller.start(sCaller.sip.url);
			} else if (sCaller.d600.host) {
				sCaller.s600(sCaller.d600.host, sCaller.d600.ip);
			} else if (sCaller.ts() >= 2) {
				this->showPrompt(2, NULL);
			}
		} else if (sCaller.mode() == sCaller.CALLING) {
			if (sCaller.m_result >= 400 || sCaller.ts() >= 5) {
				this->showPrompt(2, NULL);
			}
		}
	}

	if (m_ts && labs(time(NULL)-m_ts) > 1) {
		m_ts = 0;
		m_start = 0;
		m_prompt.hide();
	}
}

void wHistory::showPrompt(int type, const char *id)
{
	char s[512] = "";
	if (type == 1) {
		sprintf(s, "%s %s", sCaller.m_id, m_style.getText("/style/text/calling"));
	} else if (type == 2) {
		m_start = 0;
		m_ts = time(NULL);
		sprintf(s, "%s", m_style.getText("/style/text/failed"));
	} else if (type == 3 && id != NULL) {
		sprintf(s, "%s %s", id,  m_style.getText("/style/text/calling"));
	}
	m_prompt.setText(s);
	m_prompt.show();
}
