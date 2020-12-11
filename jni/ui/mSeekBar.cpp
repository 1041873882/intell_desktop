
#include "mSeekBar.h"

mSeekBar::mSeekBar()
{
	m_min = 0;
	m_max = 10;
	m_value = 0;
	m_value_width = 0;
	m_seek.setParent(this);
	m_text.setParent(this);
}

mSeekBar::~mSeekBar()
{
}

void mSeekBar::load(dxml &p, const char *zone)
{
	mPixmap::load(p, zone);
	// m_value_text.load(p, zone);

	char s[512];
	sprintf(s, "/style/%s/seek", zone);
	const char *url = p.getText(s);
	const char *root = p.getText("/style/root");
	if (root) {
		sprintf(s, "%s/%s", root, url);
		m_seek.loadFile(s);
	}
	sprintf(s, "/style/%s/style", zone);
	m_text.setStyle(p.getText(s));

	// sprintf(s, "%d", m_value);
	// m_text.setText(s);
}

void mSeekBar::doTouch(int x, int y, int m)
{
	if (m && m_visible && x >= (m_x-10) && y >= (m_y-20) && x <= (m_x+m_width+20) && y <= (m_y+m_height+40)) {
		m_tp_x = x;
		mEvent::doFifo(this);
	}
	mPixmap::doTouch(x, y, m);
}

void mSeekBar::doPaint(void)
{	
	char s[12];
	int m = m_max-m_min;
	if (m <= 0)
		m = 1;
	int v = m_value-m_min;
	int offset = (m_width-m_seek.width())*v/m;

	m_value_width = m_width/(1.007*(m_max-m_min)) * (m_value-m_min);

	m_seek.move(m_x+offset, m_y-m_seek.height()/2); //确定滑点的坐标位置

	m_text.move(m_x+offset-7, m_y-m_seek.height());
	sprintf(s, "%d", m_value);
	m_text.setText(s);

	if (m_visible && m_argb && fb.m_argb) {
		if (m_jpeg && (m_x+m_width <= fb.m_width) && (m_y+m_height) <= fb.m_height) {
			int fb_offset = m_y*fb.m_width+m_x;
			int offset = 0;
			for(int y=0; y<m_height; y++) {
				memcpy(&fb.m_argb[fb_offset], &m_argb[offset], m_width<<2);
				offset += m_width;
				fb_offset += fb.m_width;
			}
		} else {
			if (m_alpha != 0xFF) {
				uint32_t c;
				uint8_t *p = (uint8_t *)&c;
				for(int y=0; y<m_height; y++) {
					for(int x=0; x<m_value_width; x++) {
						c = m_argb[y*m_width+x];
						p[3] = (p[3]*m_alpha)>>8;
						fb.pixel(m_x+x, m_y+y, c);
					}
				}
			} else {
				for(int y=0; y<m_height; y++) {
					for(int x=0; x<m_value_width; x++) {
						fb.pixel(m_x+x, m_y+y, m_argb[y*m_width+x]);
						// m_argb[y*m_width+] = 0XFF000000;
					}
				}
			}
		}
	}

	mObject::doPaint();
}

void mSeekBar::setFocus(int val)
{
	char s[12];
	if (val) {
		m_value = 1.1*(m_max-m_min)*(m_tp_x-m_x)/m_width+m_min;
		if (m_value > m_max)
			m_value = m_max;
		else if (m_value < m_min)
			m_value = m_min;
			
		// sprintf(s, "%d", m_value);
		// m_text.setText(s);

		this->paint();
	}
	mPixmap::setFocus(val);
}
