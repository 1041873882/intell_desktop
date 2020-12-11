
#ifndef __W_CAMERA_H__
#define __W_CAMERA_H__

#include "mWindow.h"
#include "mButton.h"


class wCamera : public mWindow {
public:
	wCamera();
	virtual ~wCamera();

	void doStart(int name);
	void doStop(void);

	void doEvent(mEvent *e);
    void select(int num);
	void doTimer(void);

private:
    mButton m_prompt;
	mPixmap m_bkg;

	mPixmap m_osd;
    mButton m_camera[8];
    mButton m_more;
    mButton m_before;
	mButton m_exit;

	int m_ts;
    int max;
};

#endif
