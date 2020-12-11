
#include "sys.h"
#include "mFB.h"
#include "wMain.h"
#include "wCamera.h"
#include "SysSecurity.h"

wCamera::wCamera() : mWindow("intell_camera")
{
	m_bkg.setParent(this);
	m_bkg.load(m_style, "bkg");
	m_osd.setParent(this);
	m_osd.load(m_style, "osd");

    m_more.setParent(this);

    for (int i=0; i<8; i++) {
        m_camera[i].setParent(this);
    }

	m_prompt.setParent(this);
	m_prompt.load(m_style, "prompt");

    m_before.setParent(this);
    m_before.load(m_style, "before");

	m_exit.setParent(this);
	m_exit.load(m_style, "exit");

	int x = m_osd.x()+4;
	int y = m_osd.y()+4;
	int w = m_osd.width()-8;
	int h = m_osd.height()-8;
	// fb.osd(x, y, w, h);

	m_ts = 0;
    max = 0;

    this->select(0);
}

wCamera::~wCamera()
{
	dmsg req;
	req.request("/media/rtsp/stop", NULL);
	fb.osd(0, 0, 0, 0);
}

void wCamera::select(int num)
{   
    // this->doStop();
    if (sSecurity.ipc.m_idx)
        max = sSecurity.ipc.m_idx-1;
    else
        return;

    char s[32];
    m_more.load(m_style, "more");
    
    for (int i=0; i<=max; i++) {
        sprintf(s, "camera%d", i);
        m_camera[i].load(m_style, s);
        m_camera[i].setText(sSecurity.ipc.m_name[i]);
        if (num > 4) {
            if (i <= 4)
                m_camera[i].hide();
            else
                m_camera[i].show();
            m_more.hide();
        } else {
            m_more.show();
            if (i <= 4) 
                m_camera[i].show();
            else
                m_camera[i].hide(); 
            if (max < 5)
                m_more.hide();
        }
    }

    sprintf(s, "btn2_camera%d", num);
    m_camera[num].load(m_style, s);
    m_camera[num].setText(sSecurity.ipc.m_name[num]);  
    
    printf("name = %d max = %d\n", num, max);
    this->doStop();
    this->doStart(num);

}

void wCamera::doStop(void)
{
	m_ts = 0;
	m_prompt.setText("");
	dmsg req;
	req.request("/media/rtsp/stop", NULL);
}

void wCamera::doStart(int name)
{
    if (m_ts != 0 || name >= sSecurity.ipc.m_idx)
        return;

    dxml p, p2;
	dmsg req;
    p.setText("/params/url", sSecurity.ipc.m_url[name].c_str());
	req.request("/media/rtsp/play", p.data());
    
    int x = m_osd.x()+4;
	int y = m_osd.y()+4;
	int w = m_osd.width()-8;
	int h = m_osd.height()-8;
	p2.setInt("/params/x", x);
	p2.setInt("/params/y", y);
	p2.setInt("/params/w", w);
	p2.setInt("/params/h", h);
	req.request("/media/rtsp/screen", p2.data());
    fb.osd(x, y, w, h);

    fb.m_osd_alpha = 0xA0;
    m_prompt.show();

	m_ts = time(NULL);  
}

void wCamera::doEvent(mEvent *e)
{
	mWindow::doEvent(e);
	if (e->type == mEvent::TouchEnd) {
        if (max) {
            for (int i=0; i<=max; i++) {
            if (e->wParam == m_camera[i].id()) 
                this->select(i);
            }
        } 

        if (e->wParam == m_exit.id()) {
			wMain *w = new wMain();
			w->show();
		} else if (e->wParam == m_prompt.id()) {
            this->doStop();
        } else if (e->wParam == m_before.id()) {
            this->select(0);
        } else if (e->wParam == m_more.id()) {
            this->select(5);
        }
	}
}

void wCamera::doTimer(void)
{
	mWindow::doTimer();

	if (m_ts && time(NULL) != m_ts) {
		dmsg req;
		if (req.request("/media/rtsp/length", NULL) == 200) {
			dxml p(req.body());
			// int n = p.getInt("/params/length", 0);
			char s[24];
			// sprintf(s, "%02d:%02d", n/60, n%60);
            strcpy(s, "Appartment");
			m_prompt.setText(s);
			m_ts = time(NULL);
		} else {
			m_prompt.setText(m_style.getText("/style/text/failed"));
			m_ts = 0;
			req.request("/media/rtsp/stop", NULL);
		}
		fb.enable(1);
	}
}
