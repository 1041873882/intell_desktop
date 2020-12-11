#ifndef __YBH_M_KEYBOART_H__
#define __YBH_M_KEYBOART_H__

#include "mWindow.h"
#include "mButton.h"

#define MAX_KEY_SZ 26

class ybh_mKeyboard : public mWindow {
public:
    ybh_mKeyboard();
    virtual ~ybh_mKeyboard();

    volatile bool num_switch = false;
    volatile bool en_switch = false;

	void doTouch(int x, int y, int m);
	void setParent(mObject *obj);
	void resize(void);
	void doEvent(mEvent *e);

   char key_map_special[5] = {
        0x7F, ',', '@', ' ', '.',
    };

    char key_map_num[MAX_KEY_SZ] = {
        '1','2','3','4','5','6','7','8','9','0',
        '#', '$' ,'_', '&','-', '+', '(', ')', 0x2F,
        '*', 0x22, 0x27, ':', ';', '!', '?',
    };


    char key_map_en_small[MAX_KEY_SZ] = {
        'q','w','e','r','t','y','u','i','o','p',
        'a','s','d','f','g','h','j','k','l',
            'z','x','c','v','b','n','m',
    };

    char key_map_en_big[MAX_KEY_SZ] = {
        'Q','W','E','R','T','Y','U','I','O','P',
        'A','S','D','F','G','H','J','K','L',
            'Z','X','C','V','B','N','M',
    };

    // void setSwitch(void);
    //void doTimer(void);

private:
    mPixmap m_bkg;
    mObject *m_edit;
    mButton m_key_num[MAX_KEY_SZ];
    mButton m_key_en_big[MAX_KEY_SZ];
    mButton m_key_en_small[MAX_KEY_SZ];
    mButton m_key_special[5];

    mButton m_en_switch;
    mButton m_num_switch;
    // mButton m_num_switch_1;
    // mButton m_num_switch_2;
    mButton m_done;
};

#if 0
typedef struct key_map {
	const char *key_map_en_big[MAX_KEY_SZ] = {
        "qwertyuiopasdfghjklzxcvbnm"
    };
	const char *key_map_en_small[MAX_KEY_SZ] = {
        "QWERTYUIOPASDFGHJKLZXCVBNM"
    };
	const char key_map_num[MAX_KEY_SZ] = {
        '1','2','3','4','5','6','7','8','9','0',
         '#', '$' ,'_', '&','-', '+', '(', ')', 0x2F,
        '*', 0x22, 0x27, ':', ';', '!', '?',
    }; 
	const char key_map_special_2[8] = {
        0x00, 0x7F, 0x00, ',', '@', ' ', '.', 0x00,
    };
}m_key_map;
#endif

extern ybh_mKeyboard *y_mkeyboard;
//extern m_key_map *m_key;

#endif 