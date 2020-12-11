
#include "sys.h"
#include "SysSound.h"
#include "wMain.h"
#include "wIntercom.h"
#include "wNumber.h"

wNumber::wNumber() : mWindow("intell_number")
{
	m_bkg.setParent(this);
	m_bkg.load(m_style, "bkg");

    m_line.setParent(this);
    m_line.load(m_style, "line");

    m_input.setParent(this);
    m_input.load(m_style, "input");
    m_input.setEnable(0);
    

    m_pb_s = -1;

    for (int i=0; i<12; i++) {
        char s[12];
        sprintf(s, "k%d", i);
		m_key[i].setParent(this);
		m_key[i].load(m_style, s);
    }

    m_exit.setParent(this);
	m_exit.load(m_style, "exit");

    memset(m_text, 0, sizeof(m_text));
    m_idx = 0;
    m_mode = 0;
    m_start = 0;

    sCaller.stop();
}

wNumber::~wNumber()
{
}

void wNumber::showText(int type)
{
    char s[128];
    if (m_mode == 0) {
        if (m_pb_s != -1) {
            strcpy(s, m_pb_d[m_pb_s].text());
        } else {
            if (m_idx > 0) {
                if (m_idx == 1) {
                    sprintf(s, "%d-%s", m_data[0], m_text);
                } else if (m_idx == 2) {
                    if (sys.talk.unit() == 0)
                        sprintf(s, "%d-%s", m_data[0], m_text);
                    else
                        sprintf(s, "%d-%d-%s", m_data[0], m_data[1], m_text);
                }
            } else
                strcpy(s, m_text);
        }
        if (type == 1) {
            char s2[128];
            sprintf(s2, "%s %s", s, m_style.getText("/style/text/calling"));
            m_input.setText(s2);
        } else if (type == 2) {
            char s2[128];
            sprintf(s2, "%s %s", s, m_style.getText("/style/text/failed"));
            m_input.setText(s2);
        } else
            m_input.setText(s);
    }
}

void wNumber::showPbText(void)
{
}

void wNumber::key(int key)
{
    int n = strlen(m_text);
    if (key == 10) { //呼叫
        this->doCall();
        return;
    } else if (key == 11) { //删除
        m_pb_s = -1;
        if (n > 0) {
            m_text[n-1] = 0;
        } else {
            if (m_idx == 1) {
                m_idx = 0;
                sprintf(m_text, "%d", m_data[0]);
            } else if (m_idx == 2) {
                if (sys.talk.unit() == 0) {
                    m_idx = 0;
                    sprintf(m_text, "%d", m_data[0]);
                } else {
                    m_idx = 1;
                    sprintf(m_text, "%d", m_data[1]);
                }
            }
        }
        this->showText(0);
    } else if (m_pb_s == -1) {
        if (!sys.sip.enable()) {
            if (m_idx == 0) {
                if (n >= 4) {
                    m_idx = 1;
                    m_data[0] = atoi(m_text);
                    memset(m_text, 0, sizeof(m_text));
                    if (sys.talk.unit() == 0) {
                        m_data[1] = 0;
                        m_idx = 2;
                    }
                    m_text[0] = '0'+key;
                } else {
                    m_text[n++] = '0'+key;
                    m_text[n] = 0;
                }
            } else if (m_idx == 1) {
                if (n >= 2) {
                    m_idx = 2;
                    m_data[1] = atoi(m_text);
                    memset(m_text, 0, sizeof(m_text));
                    m_text[0] = '0'+key;
                } else {
                    m_text[n++] = '0'+key;
					m_text[n] = 0;
                }
            } else if (m_idx == 2) {
                if (n < 4) {
                    m_text[n++] = '0'+key;
                    m_text[n] = 0;
                }
            }
        } else {
            m_text[n++] = '0'+key;
            m_text[n] = 0;
        }
        this->showText(0);
    }
}

void wNumber::doCall(void)
{
    printf("m_text = %s\n", m_text);
    int n = strlen(m_text);
    if ((m_pb_s == -1 && n==0) || sCaller.mode() != sCaller.NONE)
        return;
    
    m_start = 1;
    m_mode = 0;
    if (m_pb_s != -1) {
        sCaller.start(m_pb_url[m_pb_s].c_str());
        this->showText(1);
    } else if (sys.sip.enable()) {
        char s[128];
        strcpy(s, m_text);
        if (m_idx == 0 && n <= 4)
            sprintf(s, "%d%02d%04d", sys.talk.build(), sys.talk.unit(), atoi(m_text));
        else if (m_idx == 2)
            sprintf(s, "%d%02d%04d", m_data[0], m_data[1], atoi(m_text));
        sCaller.query(s);
        this->showText(1);
    } else {
        if (n <= 4) {
            if (m_idx == 0) {
                m_data[0] = sys.talk.build();
                m_data[1] = sys.talk.unit();
                m_idx = 2;
            }
            if (m_idx == 2) {
                char s[128];
                sprintf(s, "%d%02d%04d", m_data[0], m_data[1], atoi(m_text));
                sCaller.query(s);
                usleep(100*1000);
                sCaller.query(s);
                sprintf(s, "S%04d%02d%04d0", m_data[0], m_data[1], atoi(m_text));
                sCaller.q600(s);
                this->showText(1);
            }
        } else {
            sCaller.query(m_text);
            this->showText(1);
        }
    }
}

void wNumber::doCenter(void)
{
}

void wNumber::doCenterNext(void)
{
}

void wNumber::doTimer(void)
{
    mWindow::doTimer();
    if (m_start)
        this->doCallTimer();
}

void wNumber::doCallTimer(void)
{
    if (m_mode == 1) {
        if (sCaller.mode() == sCaller.NONE) {
            this->doCenterNext();
        } else if (sCaller.mode() == sCaller.QUERY) {
            if (sCaller.sip.url) {
                sCaller.start(sCaller.sip.url);
            } else if (sCaller.ts() > 1 && sCaller.d600.host) {
                sCaller.s600(sCaller.d600.host, sCaller.d600.ip);
            } else if (sCaller.ts() > 2) {
                this->doCenterNext();
            }
        } else if (sCaller.mode() == sCaller.CALLING) {
            if (sCaller.m_result >= 400 || sCaller.ts() >= 5) {
                this->doCenterNext();
            }
        }
    } else {
        if (sCaller.mode() == sCaller.NONE) {
            this->doStop();
        } else if (sCaller.mode() == sCaller.QUERY) {
            if (sCaller.sip.url) {
                sCaller.start(sCaller.sip.url);
            } else if (sCaller.ts() > 1 && sCaller.d600.host) {
                sCaller.s600(sCaller.d600.host, sCaller.d600.ip); 
            } else if (sCaller.ts() > 2) {
                if (m_idx == 0 && sys.sip.enable() && sCaller.m_proxy) {
                    char s[512];
                    sprintf(s, "sip:%s@%s", m_text, sys.sip.m_proxy+4);
                    sCaller.start(s);
                } else
                    this->doStop();
            }
        } else if (sCaller.mode() == sCaller.CALLING) {
            if (sCaller.m_result >= 400 || sCaller.ts() >= 5) {
                this->doStop();
            }
        }
    }
}

void wNumber::doEvent(mEvent *e)
{
    mWindow::doEvent(e);
    if (e->type == mEvent::TouchEnd) {
        for (int i=0; i<12; i++) {
            if (e->wParam == m_key[i].id()) {
                this->key(i);
                break;
            }
        }

        if (e->wParam == m_exit.id()) {
            wMain *w = new wMain();
			w->show();
        }
    }
}

void wNumber::doStop(void)
{
    m_start = 0;
    m_mode = 0;
    sCaller.stop();
    this->showText(2);
}