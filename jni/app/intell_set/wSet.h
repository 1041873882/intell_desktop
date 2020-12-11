
#ifndef __INTELL_W_SET_H__
#define __INTELL_W_SET_H__

#include "mWindow.h"
#include "mButton.h"
#include "mEdit.h"

class wSet : public mWindow {
public:
	wSet();
	virtual ~wSet();

	void select(int menu);
	void doEvent(mEvent *e);
	void hide(void);

private:
	mWindow *m_label;

	mPixmap m_bkg;
	mButton m_valume;
	mButton m_ringtones;
	mButton m_language;
	mButton m_brightness;
	mButton m_handling;
	mButton m_more;

	mButton m_status;

	int m_select;
	mButton m_exit;
};

#endif
