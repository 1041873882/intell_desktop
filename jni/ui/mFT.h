
#ifndef __M_FT_H__
#define __M_FT_H__

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#include "types.h"

class mFT {
public:
	mFT();
	~mFT();

	int load(FT_ULong uc);
	int pixel(int x, int y);
	int width(void);
	int height(void);
	int left(void);
	int top(void);
	FT_Vector advance(void);

	void setPixel(int val);

	static int u8_uc(const char *src, uint16_t *uc, int sz);

protected:
	FT_Library m_library;
	FT_Face m_face;
	FT_Face m_face2;
	FT_Bitmap m_bmp;
	FT_GlyphSlot m_glyph;
};

extern mFT ft;

#endif
