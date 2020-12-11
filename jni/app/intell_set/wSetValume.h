#ifndef __W_SETUP_VALUME_H__
#define __W_SETUP_VALUME_H__

#include "sys.h"
#include "mWindow.h"
#include "mButton.h"
#include "SysSound.h"
#include "mSeekBar.h"

class wSetValume : public mWindow {
public:
    wSetValume();
    virtual ~wSetValume();

    int value(int i);
    void save(void);
private:
    mPixmap m_talk_bkg;
    mPixmap m_music_bkg;

    mSeekBar m_music;
    mSeekBar m_talk;
};

#endif
