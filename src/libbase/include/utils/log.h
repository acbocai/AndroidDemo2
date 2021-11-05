#ifndef KXQPPLATFORM_LOG_H
#define KXQPPLATFORM_LOG_H

#define FORCE_DEBUG
#define FORCE_DEBUG_VV
#ifdef FORCE_DEBUG
#ifndef LOG_TAG
#define LOG_TAG "libdebugger"
#endif
#endif
#include <android/log.h>

#ifdef FORCE_DEBUG
#define ALOGD(fmt, args...)  do {__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, fmt, ##args);} while(0)
#define ALOGI(fmt, args...)  do {__android_log_print(ANDROID_LOG_INFO, LOG_TAG, fmt, ##args);} while(0)
#define ALOGW(fmt, args...)  do {__android_log_print(ANDROID_LOG_WARN, LOG_TAG, fmt, ##args);} while(0)
#define ALOGE(fmt, args...)  do {__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, fmt, ##args);} while(0)
#else
#define ALOGD(fmt, args...) do {} while(0)
#define ALOGI(fmt, args...) do {} while(0)
#define ALOGW(fmt, args...)  do {} while(0)
#define ALOGE(fmt, args...)  do {} while(0)
#endif

#ifdef FORCE_DEBUG_VV
#define ALOGV(fmt, args...)  do {__android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, fmt, ##args);} while(0)
#else
#define ALOGV(fmt, args...) do {} while(0)
#endif

#endif //KXQPPLATFORM_LOG_H
