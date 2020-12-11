
#include "sys.h"
#include "wMain.h"
#include "wSet.h"
#include "SysSound.h"
#include "wSetValume.h"
#include "wSetRingtones.h"
#include "wSetBrightness.h"
#include "wSetStatus.h"


wSet::wSet() : mWindow("intell_set")
{
	m_bkg.setParent(this);
	m_bkg.load(m_style, "bkg");

	m_valume.setParent(this);
	m_ringtones.setParent(this);
	m_language.setParent(this);	
	m_brightness.setParent(this);
	m_handling.setParent(this);
	m_more.setParent(this);

	m_status.setParent(this);
	

	m_exit.setParent(this);
	m_exit.load(m_style, "exit");

	m_label = NULL;
	this->select(0); /* 指定一开始显示switch里的case 0内容 */
}

wSet::~wSet()
{
	if (m_label) {
		delete m_label;
		m_label = NULL;
	}
}

void wSet::select(int menu)
{
	m_valume.load(m_style, "valume");
	m_ringtones.load(m_style, "ringtones");
	m_language.load(m_style, "language");
	m_brightness.load(m_style, "brightness");
	m_handling.load(m_style, "handling");
	m_more.load(m_style, "more");

	if (m_label) {
		delete m_label;
		m_label = NULL;
	}
	m_select = menu;

	switch(menu) {
	case 0:
		m_valume.load(m_style, "valume2");
		m_label = new wSetValume();
		m_label->setParent(this);
		break;
	case 1:
		m_ringtones.load(m_style, "ringtones2");
		m_label = new wSetRingtones();
		m_label->setParent(this);
		break;
	case 2:
		m_language.load(m_style, "language2");
		// m_label = new wSetValume();
		// m_label->setParent(this);
		break;
	case 3:
		m_brightness.load(m_style, "brightness2");
		m_label = new wSetBrightness();
		m_label->setParent(this);
		break;
	case 4:
		m_handling.load(m_style, "handling2");
		// m_label = new wSetValume();
		// m_label->setParent(this);
		break;
	case 5:
		m_more.load(m_style, "more2");
		this->hide();
		// m_label = new wSetValume();
		// m_label->setParent(this);
		break;
	case 6:
		m_status.load(m_style, "status2");
		m_label = new wSetStatus();
		m_label->setParent(this);
		break;
	}
}

void wSet::doEvent(mEvent *e)
{
	mWindow::doEvent(e);

	if (e->type == mEvent::TouchEnd) {
		if (e->wParam == m_valume.id()) {
			this->select(0);
		} else if (e->wParam == m_ringtones.id()) {
			this->select(1);
		} else if (e->wParam == m_language.id()) {
			this->select(2);
		} else if (e->wParam == m_brightness.id()) {
			this->select(3);
		} else if (e->wParam == m_handling.id()) {
			this->select(4);
		} else if (e->wParam == m_more.id()) {
			this->select(5);
		} else if (e->wParam == m_status.id()) {
			this->select(6);
		} else if (e->wParam == m_exit.id()) {
			wMain *w = new wMain();
			w->show();
		} 
	}
}

void wSet::hide(void)
{

	m_valume.hide();
	m_ringtones.hide();
	m_language.hide();
	m_brightness.hide();
	m_handling.hide();
	m_more.hide();

	m_status.load(m_style, "status2");
	wSetStatus *w= new wSetStatus();
	m_label = new wSetStatus();
		m_label->setParent(this);
}

