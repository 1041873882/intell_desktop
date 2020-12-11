#include "mFB.h"
#include "ybh_mKeyboard.h"

ybh_mKeyboard *y_mkeyboard = NULL;

//m_key_map *m_key = NULL;

ybh_mKeyboard::ybh_mKeyboard() : mWindow("ybh_keyboart")
{
    int i = 0;
    char s[128];
    char s1[128];
    m_bkg.setParent(this);
    m_bkg.load(m_style, "bkg");
    //fprintf(stderr, "%c\n", key_map_num[1]);

    this->setLevel(1000);
    for(i = 0; i < MAX_KEY_SZ; i++) {
        sprintf(s, "num_key%d", i);
        m_key_num[i].setParent(this);
        m_key_num[i].load(m_style, s);
        sprintf(s1, "%c", key_map_num[i]);
        m_key_num[i].setText(s1);
        m_key_num[i].setLevel(1001); 

        sprintf(s, "en_small_key%d", i);
        m_key_en_small[i].setParent(this);
        m_key_en_small[i].load(m_style, s);
        sprintf(s1, "%c", key_map_en_small[i]);
        m_key_en_small[i].setText(s1);
        m_key_en_small[i].hide();
        m_key_en_small[i].setLevel(1001); 
    }

    for (i = 0; i < 5; i++) {
        sprintf(s, "special_key%d", i);
        m_key_special[i].setParent(this);
        m_key_special[i].load(m_style, s);
        sprintf(s1, "%c", key_map_special[i]);
        if (i) {    //第0个为删除键，不需要设置文本内容
            m_key_special[i].setText(s1);
            m_key_special[i].setLevel(1001); 
        }
    }
    
    m_done.setParent(this);
    m_done.load(m_style, "done");
    m_done.setLevel(1001);

    m_num_switch.setParent(this);
    m_num_switch.load(m_style, "num_switch");
    m_num_switch.setText("&abc");
    m_num_switch.setLevel(1001);

    m_en_switch.setParent(this);
    m_en_switch.load(m_style, "en_switch");
    m_en_switch.setLevel(1001);

    y_mkeyboard = this;
    if (window)
        mObject::setParent(window);
}

ybh_mKeyboard::~ybh_mKeyboard()
{
    y_mkeyboard = NULL;
    if (m_parent)
        m_parent->remove(this);
}

extern sFifo mTouchFifo;
void ybh_mKeyboard::doTouch(int x, int y, int m)
{
    int xx = m_bkg.x();
    int yy = m_bkg.y();
    int ww = m_bkg.width();
    int hh = m_bkg.height();
    if (x >= xx && y >= yy && x <= (xx + ww) && y <= (yy + hh)) {
        mTouchFifo.flush();
        mObject::doTouch(x, y, m);        
    } else {
        if (m > 0) {
            mEvent::doFifo(this);
            mEvent e(mEvent::KeyboardEnd);
            e.wParam = (uint32_t)this;
            mEvent::post(&e);
        }
    }
}

void ybh_mKeyboard::setParent(mObject *obj)
{
    m_edit = obj;
}

void ybh_mKeyboard::resize(void)
{
    int offset = 0;
	if (m_edit && m_edit->y() < fb.height()/2)
		offset = fb.height()-m_bkg.height();
	m_bkg.move(m_bkg.x(), m_bkg.y()+offset);
	for(int i=0; i<MAX_KEY_SZ; i++) {
        if (i < 5) {
            m_key_special[i].move(m_key_special[i].x(), m_key_special[i].y()+offset);
        }
		m_key_num[i].move(m_key_num[i].x(), m_key_num[i].y()+offset);
        m_key_en_small[i].move(m_key_en_small[i].x(), m_key_en_small[i].y()+offset);
        m_key_en_big[i].move(m_key_en_big[i].x(), m_key_en_big[i].y()+offset);
	}
    m_done.move(m_done.x(), m_done.y()+offset);
    m_num_switch.move(m_num_switch.x(), m_num_switch.y()+offset);
    m_en_switch.move(m_en_switch.x(), m_en_switch.y()+offset);
}

void ybh_mKeyboard::doEvent(mEvent *e)
{
    char s[128];
    char s1[128];
    mWindow::doEvent(e);
	if (e->type == mEvent::TouchEnd) {
		for(int i=0; i<MAX_KEY_SZ; i++) {
			if (e->wParam == m_key_num[i].id() && key_map_num[i] != ' ') {
				m_edit->key(key_map_num[i]);
				break;
			} else if (e->wParam == m_key_special[i].id() ) {
                if (i < 5)
                    m_edit->key(key_map_special[i]);

                break;
            } else if (e->wParam == m_key_en_small[i].id()) {
                m_edit->key(key_map_en_small[i]);
                break;
            } 
            else if (e->wParam == m_key_en_big[i].id()) {
                m_edit->key(key_map_en_big[i]);
                break;
            }
		}
        if (e->wParam == m_done.id()) {
            mEvent e(mEvent::KeyboardEnd);
            e.wParam = (uint32_t)this;
            mEvent::post(&e);               
        } else if (e->wParam == m_num_switch.id()) {
            num_switch = !num_switch;
            if (num_switch) {
                m_num_switch.load(m_style, "num_switch");
                m_num_switch.move(m_bkg.x()+m_num_switch.x(), m_bkg.y()+m_num_switch.y());
                m_num_switch.setText("&123");
            
                for (int i = 0; i < MAX_KEY_SZ; i++) {
                    m_key_num[i].hide();
                    m_key_en_small[i].show();
                } 
            } else {
                m_num_switch.load(m_style, "num_switch");
                m_num_switch.move(m_bkg.x()+m_num_switch.x(), m_bkg.y()+m_num_switch.y());
                m_num_switch.setText("&abc");
                for (int i = 0; i < MAX_KEY_SZ; i++) {
                    m_key_en_small[i].hide();
                    m_key_num[i].show();
                }
            }
        } else if (e->wParam == m_en_switch.id()) {
            en_switch =  !en_switch;
            if (en_switch) {
                for (int i = 0; i < MAX_KEY_SZ; i++) {
                    key_map_en_small[i] -= 32;
                    sprintf(s, "en_small_key%d", i);
                    m_key_en_small[i].load(m_style, s);
                    sprintf(s1, "%c", key_map_en_small[i]);
                    m_key_en_small[i].setText(s1);
                    m_key_en_small[i].setLevel(1001); 
                    m_key_en_small[i].move(m_bkg.x()+m_key_en_small[i].x(), m_bkg.y()+m_key_en_small[i].y());
                }   
            } else {
                for (int i = 0; i < MAX_KEY_SZ; i++) {
                    key_map_en_small[i] += 32;
                    sprintf(s, "en_small_key%d", i);
                    m_key_en_small[i].load(m_style, s);
                    sprintf(s1, "%c", key_map_en_small[i]);
                    m_key_en_small[i].setText(s1);
                    m_key_en_small[i].setLevel(1001);
                    m_key_en_small[i].move(m_bkg.x()+m_key_en_small[i].x(), m_bkg.y()+m_key_en_small[i].y());
                }
            }
        }
	}
}

#if 0
void ybh_mKeyboard::setSwitch(void)
{
    char s[128];
    char s1[128];

    if (en_switch) {
        for (int i = 0; i < MAX_KEY_SZ; i++) {
            key_map_en_small[i] -= 32;
            sprintf(s, "en_small_key%d", i);
            m_key_en_small[i].load(m_style, s);
            sprintf(s1, "%c", key_map_en_small[i]);
            m_key_en_small[i].setText(s1);
            m_key_en_small[i].setLevel(1001); 
            m_key_en_small[i].move(m_bkg.x()+m_key_en_small[i].x(), m_bkg.y()+m_key_en_small[i].y());
        }   
    } else {
        for (int i = 0; i < MAX_KEY_SZ; i++) {
            key_map_en_small[i] += 32;
            sprintf(s, "en_small_key%d", i);
            m_key_en_small[i].load(m_style, s);
            sprintf(s1, "%c", key_map_en_small[i]);
            m_key_en_small[i].setText(s1);
            m_key_en_small[i].setLevel(1001);
            m_key_en_small[i].move(m_bkg.x()+m_key_en_small[i].x(), m_bkg.y()+m_key_en_small[i].y());
        }
    } 

    if (num_switch) {
        m_num_switch.load(m_style, "num_switch");
        m_num_switch.move(m_bkg.x()+m_num_switch.x(), m_bkg.y()+m_num_switch.y());
        m_num_switch.setText("&123");
    
        for (int i = 0; i < MAX_KEY_SZ; i++) {
            m_key_num[i].hide();
            m_key_en_small[i].show();
        } 
    } else {
        m_num_switch.load(m_style, "num_switch");
        m_num_switch.move(m_bkg.x()+m_num_switch.x(), m_bkg.y()+m_num_switch.y());
        m_num_switch.setText("&abc");
        for (int i = 0; i < MAX_KEY_SZ; i++) {
            m_key_en_small[i].hide();
            m_key_num[i].show();
        }
    }
}
#endif