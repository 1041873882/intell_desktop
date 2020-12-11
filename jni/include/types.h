
#ifndef __TYPES_H__
#define __TYPES_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/time.h>

#define UNUSED(x) (void)x

#ifndef MIN
#define MIN(a, b) ((a)>(b) ? (b) : (a))
#endif

static inline int __ts(struct timeval &tv)
{
	struct timeval tv2;
	gettimeofday(&tv2, NULL);
	return labs((tv2.tv_sec-tv.tv_sec)*1000+(tv2.tv_usec-tv.tv_usec)/1000);
}

#include <android/log.h>
#define LOGI(...)	__android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...)	__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#endif
