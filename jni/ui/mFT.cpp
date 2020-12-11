
#include "mFT.h"

mFT ft;

mFT::mFT()
{
	FT_Init_FreeType(&m_library);
	FT_New_Face(m_library, "/system/fonts/DroidSansFallback.ttf", 0, &m_face);
	FT_New_Face(m_library, "/system/fonts/DroidSans.ttf", 0, &m_face2);

	this->setPixel(24);
}

mFT::~mFT()
{
	FT_Done_Face(m_face);
	FT_Done_Face(m_face2);
	FT_Done_FreeType(m_library);
}

int mFT::load(FT_ULong uc)
{
	FT_UInt index;
	int err = -1;

	index = FT_Get_Char_Index(m_face, uc);

	if (index > 0) {
		err = FT_Load_Glyph(m_face, index, FT_LOAD_DEFAULT | FT_LOAD_NO_BITMAP);
		if (!err) {
			err = FT_Render_Glyph(m_face->glyph, FT_RENDER_MODE_NORMAL);
			m_glyph = m_face->glyph;
			m_bmp = m_face->glyph->bitmap;
		}
	} else {
		index = FT_Get_Char_Index(m_face2, uc);
		if (index > 0) {
			err = FT_Load_Glyph(m_face2, index, FT_LOAD_DEFAULT | FT_LOAD_NO_BITMAP);
			if (!err) {
				err = FT_Render_Glyph(m_face2->glyph, FT_RENDER_MODE_NORMAL);
				m_glyph = m_face2->glyph;
				m_bmp = m_face2->glyph->bitmap;
			}
		}
	}
	return err;
}

int mFT::pixel(int x, int y)
{
	if (m_bmp.pixel_mode == FT_PIXEL_MODE_MONO) { //单色黑白
		uint8_t *p = &m_bmp.buffer[y*m_bmp.pitch];
		int d = (p[x/8]>>(8-(x%8)))&0x01 ? 0xFF : 0x00;
		return d;
	} else if (m_bmp.pixel_mode == FT_PIXEL_MODE_GRAY) { //8位彩色
		return m_bmp.buffer[y*m_bmp.pitch+x];
	}
	return 0;
}

int mFT::width(void)
{
	return m_bmp.width;
}

int mFT::height(void)
{
	return m_bmp.rows;
}

int mFT::left(void)
{
	return m_glyph->bitmap_left;
}

int mFT::top(void)
{
	return m_glyph->bitmap_top;
}

FT_Vector mFT::advance(void)
{
	return m_glyph->advance;
}

void mFT::setPixel(int val)
{
	FT_Set_Pixel_Sizes(m_face, 0, val);
	FT_Set_Pixel_Sizes(m_face2, 0, val);
}

int mFT::u8_uc(const char *src, uint16_t *uc, int sz)
{
	const uint8_t *p = (const uint8_t *)src;
	uint8_t *result = (uint8_t *)uc;
	int uc_sz = 0, uc_max = sz/2;

	memset(uc, 0, sz);
	while(*p) {  
		if (*p <= 0x7F) {
			//说明最高位为'0'，这意味着utf8编码只有1个字节
			*result++ = *p;  
			result++;

			uc_sz++;  
		} else if ((*p & 0xE0)== 0xC0) {
			//只保留最高三位，看最高三位是不是110，如果是则意味着utf8编码有2个字节
			char b[2] = {0, 0};
  
			b[0] = *p++ & (0x1F);//高位的后5位 (去除了头部的110这个标志位)
			b[1] = *p & (0x3F);//低位的后6位 (去除了头部的10这个标志位)

			*result++ = b[1] | ((b[0] & (0x03)) << 6);
			*result++ = b[0] >> 2;//留下其保留的三位

			uc_sz++; 
		} else if ((*p & (0xF0))== 0xE0) {//只保留最高四位，看最高三位是不是1110，如果是则意味着utf8编码有3个字节
			char b[3] = {0, 0, 0};
			b[0] = *p++ & (0x1F);
			b[1] = *p++ & (0x3F);
			b[2] = *p & (0x3F);

			*result++ = ((b[1] & (0x03)) << 6) | b[2];
			*result++ = (b[0] << 4) | (b[1] >> 2);
			uc_sz++;
		}
		p++;

		if (uc_sz >= uc_max)
			break;
	}
	return uc_sz;
}
