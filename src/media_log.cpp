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
#include "securec.h"
#ifdef HI_LOG_ENABLE
#include "hilog/log.h"
#else
#include <cstdio>
#endif

static const char* MEDIA_LOG_TITLE_TAG = "MEDIA_COMMON";
constexpr int32_t LOG_MAX_LEN = 4096;
static MEDIA_LOG_LEVEL g_enabledLevel = MEDIA_LOG_ERR;

#ifdef HI_LOG_ENABLE
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
#endif

static int32_t MediaLogPrintfOut(MEDIA_LOG_LEVEL level, const char *fmt)
{
    if (level < g_enabledLevel) {
        return MEDIA_OK;
    }
    if (fmt == nullptr) {
        return MEDIA_ERR;
    }
#ifdef HI_LOG_ENABLE
    LogLevel hiLogLevel = MapLogLevel(level);
    (void)HiLogPrint(LOG_CORE, hiLogLevel, LOG_DOMAIN, MEDIA_LOG_TITLE_TAG, "%{public}s", fmt);
#else
    const char *levelTag = "E";
    switch (level) {
        case MEDIA_LOG_DEBUG:
            levelTag = "D";
            break;
        case MEDIA_LOG_INFO:
            levelTag = "I";
            break;
        case MEDIA_LOG_WARN:
            levelTag = "W";
            break;
        case MEDIA_LOG_ERR:
            levelTag = "E";
            break;
        case MEDIA_LOG_FATAL:
            levelTag = "F";
            break;
        default:
            break;
    }
    (void)fprintf(stderr, "[%s]%s\n", levelTag, fmt);
#endif
    return MEDIA_OK;
}

int32_t MediaLogPrintf(MEDIA_LOG_LEVEL level, const char *fmt, ...)
{
    /* Per-thread buffer avoids 4KB stack use and needs no lock. */
    thread_local char logBuf[LOG_MAX_LEN];
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
    thread_local char logBuf[LOG_MAX_LEN];
    va_list arg;
    (void)memset_s(&arg, sizeof(va_list), 0, sizeof(va_list));
    va_start(arg, fmt);

    int32_t ret = vsprintf_s(logBuf, sizeof(logBuf), fmt, arg);
    va_end(arg);
    if (ret < 0) {
#ifdef HI_LOG_ENABLE
        (void)HiLogPrint(LOG_CORE, LOG_ERROR, LOG_DOMAIN, MEDIA_LOG_TITLE_TAG, "%{public}s",
            "media log length error.");
#else
        (void)fprintf(stderr, "media log length error.\n");
#endif
        return MEDIA_ERR;
    }
#ifdef HI_LOG_ENABLE
    (void)HiLogPrint(LOG_CORE, LOG_INFO, LOG_DOMAIN, MEDIA_LOG_TITLE_TAG, "%{public}s", logBuf);
#else
    (void)fprintf(stderr, "%s\n", logBuf);
#endif
    return MEDIA_OK;
}
