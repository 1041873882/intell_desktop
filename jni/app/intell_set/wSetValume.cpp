#include "sys.h"
#include "SysSound.h"
#include "mFB.h"
#include "wSetValume.h"

wSetValume::wSetValume() : mWindow("intell_set/valume")
{
    m_talk_bkg.setParent(this);
    m_talk_bkg.load(m_style, "talk_bkg");
    m_talk.setParent(this);
    m_talk.load(m_style, "talk");
    m_talk.setRange(0, 100);
    m_talk.setValue((5-sys.volume.talk())*20);
    // m_talk.setRange(100, 255);
    // m_talk.setValue(sys.lcd.bright());

    m_music_bkg.setParent(this);
    m_music_bkg.load(m_style, "music_bkg");
    m_music.setParent(this);
    m_music.load(m_style, "music");
    m_music.setRange(0, 100);
    m_music.setValue((5-sys.volume.music())*20);
}

wSetValume::~wSetValume()
{
    this->save();
}

int wSetValume::value(int i)
{
    int value;
    if (i > 0 && i <= 20)
            value = 1;
    else if (i > 20 && i <= 40)
        value = 2;
    else if (i > 40 && i <= 60)
        value = 3;
    else if (i > 60 && i <= 80)
        value = 4;
    else if (i > 80 && i <= 100)
        value = 5;
    else
        value = 0; 
    
    return value;
}

void wSetValume::save(void)
{
    int i = 0;
    i = this->value(m_talk.value());
    sys.volume.talk(5-i);

    i = this->value(m_music.value());
    sys.volume.music(5-i);
    sys.save();
}