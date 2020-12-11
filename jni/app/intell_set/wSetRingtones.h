#ifndef __W_SETUP_RINGTONES_H__
#define __W_SETUP_RINGTONES_H__

#include "sys.h"
#include "mWindow.h"
#include "mButton.h"
#include "SysSound.h"


class wSetRingtones : public mWindow {
public:
    wSetRingtones();
    virtual ~wSetRingtones();

    void doEvent(mEvent *e);

private:

};


#endif