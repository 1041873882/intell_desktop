#include "wSetBrightness.h"

wSetBrightness::wSetBrightness() : mWindow("intell_set/brightness")
{
    m_brightness_bkg.setParent(this);
    m_brightness_bkg.load(m_style, "brightness_bkg");
    m_brightness.setParent(this);
    m_brightness.load(m_style, "brightness");
    m_brightness.setRange(0, 100);
    m_brightness.setValue((sys.lcd.bright()-100)*0.67);
    printf("sys.lcd.bright = %d\n", sys.lcd.bright());
}

wSetBrightness::~wSetBrightness()
{
    this->save();
}


void wSetBrightness::save(void)
{
    int value;

    value = (m_brightness.value()+67)*1.5;
    sys.lcd.bright(value);
    sys.save();
}