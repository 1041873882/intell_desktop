
#ifndef __W_NUMBER_H__
#define __W_NUMBER_H__

#include "mWindow.h"
#include "mButton.h"

#include "SysCaller.h"

class wNumber : public mWindow {
public:
	wNumber();
	virtual ~wNumber();

	void key(int key);
	void doCall(void);
	void doCenter(void);
	void doCenterNext(void);
	void doEvent(mEvent *e);
	void doTimer(void);
	void doCallTimer(void);
	void doStop(void);

	// type 0:��ͨ��ʾ 1:������ 2:����ʧ��
	void showText(int type);
	void showPbText(void);

private:
	mPixmap m_bkg;
	mButton m_pb;
	mPixmap m_line;
	mButton m_input;
	mButton m_key[12];
	mButton m_build;
	mButton m_unit;
	mButton m_center;
	mButton m_exit;

	mButton m_pb_d[MAX_SLAVE_SZ];
	std::string m_pb_url[MAX_SLAVE_SZ];
	int m_pb_s;

	char m_text[64];
	int m_data[2];
	int m_idx;

	int m_mode; //0:��ͨģʽ 1:��������
	int m_start; //�Ƿ��ں���
};

#endif
