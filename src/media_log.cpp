/*
 * Copyright (c) 2025 HiSilicon (Shanghai) Technologies Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "media_log.h"
#include <string>
#include <ctime>
#include <sys/time.h>
#include "securec.h"
#ifdef HI_LOG_ENABLE
#include "hilog/log.h"
#else
#include <cstdio>
#endif

static const char* MEDIA_LOG_TITLE_TAG = "MEDIA_COMMON";
constexpr int32_t LOG_MAX_LEN = 4096;
static MEDIA_LOG_LEVEL g_enabledLevel = MEDIA_LOG_ERR;

static void LogTime()
{
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    struct tm tm;
    localtime_r(&tv.tv_sec, &tm);
    const int64_t usecToMsec = 1000;
    printf("[%02d:%02d:%02d:%03ld]", tm.tm_hour, tm.tm_min, tm.tm_sec, tv.tv_usec / usecToMsec);
}

static LogLevel MapLogLevel(MEDIA_LOG_LEVEL level)
{
    switch (level) {
        case MEDIA_LOG_DEBUG:
            return LOG_DEBUG;
        case MEDIA_LOG_INFO:
            return LOG_INFO;
        case MEDIA_LOG_WARN:
            return LOG_WARN;
        case MEDIA_LOG_ERR:
            return LOG_ERROR;
        default:
            return LOG_INFO;
    }
}

static int32_t MediaLogPrintfOut(MEDIA_LOG_LEVEL level, const char *fmt)
{
    if (level < g_enabledLevel) {
        return MEDIA_OK;
    }
    if (fmt == nullptr) {
        return MEDIA_ERR;
    }
#if defined(HI_LOG_ENABLE) && !defined(ENABLE_DFX)
    LogLevel hiLogLevel = MapLogLevel(level);
    (void)HiLogPrint(LOG_CORE, hiLogLevel, LOG_DOMAIN, MEDIA_LOG_TITLE_TAG, "%{public}s", fmt);
#else
    LogTime();
    switch (level) {
        case MEDIA_LOG_DEBUG:
            printf("[D]%s\n", fmt);
            break;
        case MEDIA_LOG_INFO:
            printf("[I]%s\n", fmt);
            break;
        case MEDIA_LOG_WARN:
            printf("[W]%s\n", fmt);
            break;
        case MEDIA_LOG_ERR:
            printf("[E]%s\n", fmt);
            break;
        case MEDIA_LOG_FATAL:
            printf("[F]%s\n", fmt);
            break;
        default:
            printf("[E]%s\n", fmt);
            break;
    }
#endif
    return MEDIA_OK;
}

int32_t MediaLogPrintf(MEDIA_LOG_LEVEL level, const char *fmt, ...)
{
    char logBuf[LOG_MAX_LEN] = { 0 };
    va_list arg;
    (void)memset_s(&arg, sizeof(va_list), 0, sizeof(va_list));
    va_start(arg, fmt);

    int32_t ret = vsprintf_s(logBuf, sizeof(logBuf), fmt, arg);
    va_end(arg);
    if (ret < 0) {
        MediaLogPrintfOut(level, "media log length error.");
        return MEDIA_ERR;
    }
    MediaLogPrintfOut(level, logBuf);
    return MEDIA_OK;
}

int32_t MediaDfxLogPrintf(const char *fmt, ...)
{
    char logBuf[LOG_MAX_LEN] = { 0 };
    va_list arg;
    (void)memset_s(&arg, sizeof(va_list), 0, sizeof(va_list));
    va_start(arg, fmt);

    int32_t ret = vsprintf_s(logBuf, sizeof(logBuf), fmt, arg);
    va_end(arg);
    if (ret < 0) {
        printf("media log length error.\n");
        return MEDIA_ERR;
    }
    LogTime();
    printf("%s\n", logBuf);
    return MEDIA_OK;
}