#ifndef __W_SETUP_BRIGHTNESS_H__
#define __W_SETUP_BRIGHTNESS_H__

#include "sys.h"
#include "mWindow.h"
#include "mButton.h"
#include "SysSound.h"
#include "mSeekBar.h"

class wSetBrightness : public mWindow {
public:
    wSetBrightness();
    virtual ~wSetBrightness();

    void save(void);
private:
    mPixmap m_brightness_bkg;

    mSeekBar m_brightness;
};


#endif