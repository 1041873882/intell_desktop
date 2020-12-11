
#ifndef __W_TALK_H__
#define __W_TALK_H__

#include "mWindow.h"
#include "mButton.h"

class wTalk : public mWindow {
public:
	wTalk();
	virtual ~wTalk();

	void doEvent(mEvent *e);
	void doTimer(void);

	void host2id(const char *sid, char *s);
	void jpeg(const char *url);

	int doElevVisit(const char *host);
	
private:
	mPixmap m_bkg;
	mButton m_answer;	//接听
	mButton m_reject;	//挂断
	mButton m_unlock;	
	mButton m_mic;
	mButton m_prompt;
	mPixmap m_osd;

	mText m_answer_text;
	mText m_reject_text;
	mText m_unlock_text;

	int m_vo_st;
	int m_answer_st; //0:未接听状态 1:呼叫或已接听
	int m_type;
	int m_touch_st;

	time_t m_ts;
	time_t m_jpeg;
	time_t m_busy_ts;
};

#endif
