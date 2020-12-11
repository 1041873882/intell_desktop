
#ifndef __W_HISTORY_H__
#define __W_HISTORY_H__

#include "mWindow.h"
#include "mButton.h"
#include "mText.h"

#define MAX_LOGGER_LINE		6

class wHistory : public mWindow {
public:
	wHistory();
	virtual ~wHistory();

	void showData(int n);
	void showPrompt(int type, const char *id);

	void doEvent(mEvent *e);
	void doTimer(void);

private:
	mPixmap m_bkg;
	mPixmap m_icon;
	mText m_title;
    mPixmap m_line[7];
    mButton m_del[MAX_LOGGER_LINE];
    mButton m_call[MAX_LOGGER_LINE];

	mText m_host[MAX_LOGGER_LINE];
	mText m_date[MAX_LOGGER_LINE];
    mText m_time[MAX_LOGGER_LINE];

	mButton m_prompt;

	mButton m_up;
	mButton m_down;

	mButton m_exit;


	int m_first;
	int m_max;
	int m_select;
	int m_offset;
	int m_start;
	time_t m_ts;
};

#endif
