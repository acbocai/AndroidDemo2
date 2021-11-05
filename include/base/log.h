//
// Created by admin on 2021/11/05.
//

#ifndef MAIN_LOG_H
#define MAIN_LOG_H
#include <android/log.h>

#ifndef LOG_TAG
#define LOG_TAG "Demo2"
#endif
#ifdef ENABLE_LOGGING
#define ALOGD(fmt, args...)  do {__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, fmt, ##args);} while(0)
#define ALOGI(fmt, args...)  do {__android_log_print(ANDROID_LOG_INFO, LOG_TAG, fmt, ##args);} while(0)
#else
#define ALOGD(fmt, args...)  do {} while(0)
#define ALOGI(fmt, args...)  do {} while(0)
#endif
#define ALOGW(fmt, args...)  do {__android_log_print(ANDROID_LOG_WARN, LOG_TAG, fmt, ##args);} while(0)
#define ALOGE(fmt, args...)  do {__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, fmt, ##args);} while(0)

#endif //MAIN_LOG_H
