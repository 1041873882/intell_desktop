
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>

#include "SysUtils.h"

int sws_scale_argb(void *src, int sw, int sh, void *dst, int dw, int dh)
{
	if (sw > 1024 && sw>dw) { //大图片缩放，直接缩小到1/4
		uint32_t *p = (uint32_t *)src;
		uint32_t *d = p;
		for(int yy=0; yy<sh; yy+=2) {
			int offset = yy*sw;
			for(int xx=0; xx<sw; xx+=2) {
				*d++ = p[offset+xx];
			}
		}
		sw /= 2;
		sh /= 2;
	}

	struct SwsContext *sws = sws_getContext(sw, sh, PIX_FMT_ARGB, dw, dh, PIX_FMT_ARGB, SWS_FAST_BILINEAR, NULL, NULL, NULL);
	if (sws) {
		AVPicture s, d;
		avpicture_fill(&s, (uint8_t *)src, PIX_FMT_ARGB, sw, sh);
		avpicture_fill(&d, (uint8_t *)dst, PIX_FMT_ARGB, dw, dh);
		int r = sws_scale(sws, s.data, s.linesize, 0, sh, d.data, d.linesize); 
		sws_freeContext(sws);
		return r;
	}
	return -1;
}

#include "iconv.h"
int gbk_utf8(const char *s, char *d, size_t d_sz)
{
	memset(d, 0, d_sz);
	iconv_t c = iconv_open("utf-8", "gbk");
	if (c) {
		size_t s_sz = strlen(s);
		char *ss = new char[s_sz+1];
		strcpy(ss, s);

		char *dst = d;
		char *src = ss;
		int r = iconv(c, &src, &s_sz, &dst, &d_sz);

		delete []ss;
		iconv_close(c);
		return r;
	}
	return -1;
}

int utf8_utf16(const char *s, char *d, size_t d_sz)
{
	memset(d, 0, d_sz);
	iconv_t c = iconv_open("utf-16le", "utf-8");
	if (c) {
		size_t s_sz = strlen(s);
		char *ss = new char[s_sz+1];
		strcpy(ss, s);

		char *dst = d;
		char *src = ss;
		int r = iconv(c, &src, &s_sz, &dst, &d_sz);

		delete []ss;
		iconv_close(c);
		return r;
	}
	return -1;
}

int is_utf8(const char *data, int length)
{
	if (data == NULL)
		return 0;

	int asc = 1;
	int utf8_n = 0, gbk_n = 0;
	for(int i=0; i<length; i++) {
		if (data[i] > 0 && data[i] < 0x7F)
			continue;

		asc = 0;
		if ((data[i+0] & 0xE0) == 0xC0 && i+1<=length) { //双字节格式
			if ((data[i+1] & 0xC0) == 0x80) {
				int n = data[i+0] & 0xFF;
				int n2 = data[i+1] & 0xFF;
				if ((0x81 <= n && n <= 0xFE) && (0x40 <= n2 && n2 <= 0xFE) && (n2 != 0x7F)) {
				} else {
					utf8_n++;
					i++;
					continue;
				}
			}
		} else if ((data[i+0] & 0xF0) == 0xE0 && i+2<=length) { //三字节格式
			if (((data[i+1] & 0xC0) == 0x80) && ((data[i+2] & 0xC0) == 0x80)) {
				utf8_n++;
				i += 2;
				continue;
			}
		} else if ((data[i+0] & 0xF8) == 0xF0 && i+3<=length) { //四字节格式
			if (((data[i+1] & 0xC0) == 0x80) && ((data[i+2] & 0xC0) == 0x80) && ((data[i+3] & 0xC0) == 0x80)) {
				utf8_n++;
				i += 3;
				continue;
			}
		}
		i++;
		gbk_n++;
	}
	if (asc == 0) {
		if (gbk_n > 0 && 10*utf8_n < gbk_n)
			return 0;
		return 1;
	}
	return 1;
}
