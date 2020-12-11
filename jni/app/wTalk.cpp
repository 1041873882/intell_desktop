
#include "dxml.h"
#include "dmsg.h"
#include "mFB.h"
#include "SysIoctl.h"
#include "SysCaller.h"
#include "SysLogger.h"
#include "SysWifi.h"
#include "wTalk.h"

mWindow *SysTalk = NULL;

wTalk::wTalk() : mWindow("intell_talk")
{
	SysTalk = this;
	fb.enable(1);

	m_bkg.setParent(this);
	m_bkg.load(m_style, "bkg");

	m_osd.setParent(this);
	m_osd.load(m_style, "osd");
	// m_osd.hide();

	m_prompt.setParent(this);
	m_prompt.load(m_style, "prompt");
	m_prompt.setText("Appartment");
	m_prompt.setEnable(0);

	m_answer.setParent(this);
	m_answer.load(m_style, "answer");
	m_answer.hide();
	m_answer_text.setParent(this);
	m_answer_text.load(m_style, "answer_text");
	m_answer_text.hide();

	m_reject.setParent(this);
	m_reject.load(m_style, "reject");
	// m_reject.hide();
	m_reject_text.setParent(this);
	m_reject_text.load(m_style, "reject_text");
	// m_reject_text.hide();

	m_unlock.setParent(this);
	m_unlock.load(m_style, "unlock");
	// m_unlock.hide();
	m_unlock_text.setParent(this);
	m_unlock_text.load(m_style, "unlock_text");

	m_mic.setParent(this);
	m_mic.load(m_style, "mic");
	// m_unlock_text.hide();

	// m_mic.setParent(this);
	// m_mic.load(m_style, "mic");

	m_vo_st = 0;
	m_ts = time(NULL);
	m_jpeg = time(NULL);
	m_busy_ts = 0;
	m_touch_st = 0;
}

wTalk::~wTalk()
{
	SysTalk = NULL;
	fb.osd(0, 0, 0, 0);

	sLogger.talk.insert(m_type);
}

void wTalk::doEvent(mEvent *e)
{
	mWindow::doEvent(e);

	if (e->type == mEvent::TouchBegin) {
		if (e->wParam == m_answer.id()) {
			m_answer.load(m_style, "answer1");
		} else if (e->wParam == m_unlock.id()) {
			m_unlock.load(m_style, "unlock1");
		} else if (e->wParam == m_reject.id()) {
			m_reject.load(m_style, "reject1");
		}
	}

	if (e->type == mEventJpeg) {
		const char *url = (const char *)e->wParam;
		this->jpeg(url);
	} else if (e->type == mEventBusy) {
		const char *id = (const char *)e->wParam;
		char s[128], host[64];
		this->host2id(id, host);
		m_busy_ts = time(NULL);
		sCaller.m_h2id_mark = 0;
	} else if (e->type == mEventHook) {
		int st = e->wParam;
		if (st == 0) { //提机
			if (!m_answer_st) {
				m_type = sLogger.talk.IN;
				m_answer_st = 1;
				m_answer.load(m_style, "stop");
				sCaller.answer();
			}
		} else {
			if (m_answer_st)
				sCaller.stop();
		}
	} else if (e->type == mEvent::TouchEnd) {
		if (e->wParam == m_answer.id()) {
			if (!m_answer_st) {
				m_type = sLogger.talk.IN;
				m_answer_st = 1;
				m_answer.load(m_style, "answer2");
				m_touch_st = 1;
				// m_answer.load(m_style, "answer2");
				// m_answer.hide();
				sCaller.answer();
			}
		} else if (e->wParam == m_reject.id()) {
				m_reject.load(m_style, "reject2");
				// m_reject.hide();
				sCaller.stop();
		} else if (e->wParam == m_unlock.id()) {
			m_unlock.load(m_style, "unlock2");
			if (!this->doElevVisit(sCaller.m_id))
				sCaller.unlock();
		}
		//  else if (e->wParam == m_mic.id()) {
		// 	if (sCaller.mode() == sCaller.TALKING) {
		// 		if (sCaller.m_mute[0]) {
		// 			sCaller.mute(0, 0);
		// 			m_mic.load(m_style, "mic");
		// 		} else {
		// 			sCaller.mute(0, 1);
		// 			m_mic.load(m_style, "mic2");
		// 		}
		// 	}
		// } 
	}
}

void wTalk::doTimer(void)
{
	mWindow::doTimer();
	fb.enable(1);

	if (sCaller.m_h2id_mark) {
		sCaller.m_h2id_mark = 0;
		char s[128];
		this->host2id(sCaller.m_id, s);
	}
	if (m_busy_ts && labs(time(NULL)-m_busy_ts) >= 5) {
		m_busy_ts = 0;
		sCaller.m_h2id_mark = 1;
	}

	if (!m_vo_st) {
		m_vo_st = 1;
		int x = m_osd.x()+4;
		int y = m_osd.y()+4;
		int w = m_osd.width()-8;
		int h = m_osd.height()-8;

		dmsg req;
		dxml p;
		p.setInt("/params/x", x);
		p.setInt("/params/y", y);
		p.setInt("/params/width", w);
		p.setInt("/params/height", h);
		req.request("/talk/vo_start", p.data());
		fb.osd(x, y, w, h);
	}

	fb.m_osd_alpha = 0XA0;
	if (sCaller.mode() == sCaller.CALLING) {
		m_type = sLogger.talk.OUT;
		m_answer_st = 1;
		m_touch_st = 1;
	} else {
		m_type = sLogger.talk.NA;
		m_answer_st = 0;
		if (m_touch_st) {
			m_answer.hide();
			m_answer_text.hide();
		} else {
			m_answer.show();
			m_answer_text.show();
		}
		sIoctl.audio(0);
	}

	m_prompt.show();
	m_reject.show();
	m_reject_text.show();
	m_unlock.show();
	m_unlock_text.show();

	if (m_jpeg && labs(time(NULL)-m_jpeg) > 2) {
		m_jpeg = 0;
		char s[128];
		sprintf(s, "%s/%ld.jpg", sLogger.jpeg_dir(), time(NULL));
		dxml p;
		dmsg req;
		p.setText("/params/url", s);
		req.request("/talk/snapshot", p.data());

		if (sys.talk.answer()) {
			//自动接听
			if (!m_answer_st) {
				m_answer_st = 1;
				m_reject.load(m_style, "reject");
				sCaller.answer();
			}			m_type = sLogger.talk.IN;
		}
	}

	if (labs(time(NULL)-m_ts) > 2) {
		m_ts = time(NULL);
		dmsg req;
		dxml p;
		req.request("/talk/active", NULL);
		p.load(req.body());
		if (!p.getInt("/params/data", 0)) {
			sCaller.stop();
		}
	}
}

void wTalk::host2id(const char *sid, char *s)
{
	if (sid == NULL || s == NULL)
		return;

	strcpy(s, sid);
	host2id_t *id = sCaller.host2id(s);
	if (id != NULL) {
		if (id->build == 0 && id->unit == 0 && id->floor == 0) {
			sprintf(s, "%s%02d", m_style.getText("/style/center"), id->family);
		} else if (id->build && id->floor == 99) {
			sprintf(s, "%s%02d", m_style.getText("/style/panel"), id->family);
		} else if (id->build == 0 && id->floor == 99) {
			sprintf(s, "%s%02d", m_style.getText("/style/wall"), id->family);
		} else if (strlen(id->host) > 9) { //小门口机
			char s2[32];
			memcpy(s2, id->host, 1);
			s2[1] = 0;
			sprintf(s, "%s%02d", m_style.getText("/style/person"), atoi(s2));
		} else {
			sprintf(s, "%d-%d-%d%02d", id->build, id->unit, id->floor, id->family);
		}
	} else {
		if (sid[0] == 'S') {
			char s1[32], s2[32], s3[32];
			memcpy(s1, sid+1, 4);
			s1[4] = 0;
			memcpy(s2, sid+5, 2);
			s2[2] = 0;
			memcpy(s3, sid+7, 4);
			s3[4] = 0;
			sprintf(s, "%s-%s-%s", s1, s2, s3);
		} else if (sid[0] == 'Z') {
			char s2[32];
			memcpy(s2, sid+1, 4);
			s2[4] = 0;
			sprintf(s, "%s%s", m_style.getText("/style/center"), s2);
		} else if (sid[0] == 'M') {
			char s2[32];
			memcpy(s2, sid+7, 1);
			s2[1] = 0;
			sprintf(s, "%s%02d", m_style.getText("/style/panel"), atoi(s2)+1);
		} else if (sid[0] == 'W') {
			char s2[32];
			memcpy(s2, sid+1, 4);
			s2[4] = 0;
			sprintf(s, "%s%s", m_style.getText("/style/wall"), s2);
		} else if (sid[0] == 'H') {
			char s2[32];
			memcpy(s2, sid+11, 1);
			s2[1] = 0;
			sprintf(s, "%s%d", m_style.getText("/style/person"), atoi(s2)+1);
		}
	}
}

void wTalk::jpeg(const char *url)
{
	if (url)
		sLogger.jpeg.insert(sCaller.m_ts, url);
}

int wTalk::doElevVisit(const char *host)
{
	host2id_t *id = sCaller.host2id(host);
	if (id != NULL) {
		if (id->build == 0 && id->unit == 0 && id->floor == 0) { // 管理机
		} else if (id->floor == 99 && id->build > 0) { // 单元门口机
		} else if (id->floor == 99 && id->build == 0) { // 围墙机
		} else if (strlen(id->host) < 11) { //分机
			if (id->build != sys.talk.build() || id->unit != sys.talk.unit() ||
			    id->floor != sys.talk.floor() || id->family != sys.talk.family()) {
				dxml p;
				dmsg req;
				p.setText("/event/broadcast_url", "/elev/visit");
				p.setInt("/event/elev/from", id->floor*100+id->family);
				p.setInt("/event/elev/to", sys.talk.floor()*100+sys.talk.family());
				req.request("/talk/broadcast/data", p.data());
				return 1;
			}
		}
	}
	return 0;
}
