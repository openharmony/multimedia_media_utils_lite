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

#include "media_utils.h"
#ifdef ENABLE_DFX
#include <cinttypes>
#include <ctime>

#include "media_log.h"

static std::atomic<uint64_t> g_receiveAudiotime = 0;
static std::atomic<uint64_t> g_receiveAudiotimeLast = 0;

static bool UIntMulIsOverflow64(uint64_t a, uint64_t b, uint64_t *value)
{
    if (a == 0 || b == 0) {
        return false;
    }
    if (UINT64_MAX / a < b) {
        return true;
    }

    uint64_t result = static_cast<uint64_t>((a) * (b));
    if (value != nullptr) {
        *value = result;
    }
    return false;
}

static bool GetMonotonicTimeUs(uint64_t &timeUs, const char *errTag)
{
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
        MEDIA_ERR_LOG("%s clock_gettime failed", errTag);
        return false;
    }
    const uint64_t usecPerSec = 1000000;
    const uint64_t nsecPerUsec = 1000;
    uint64_t secToUs = 0;
    if (UIntMulIsOverflow64(static_cast<uint64_t>(ts.tv_sec), usecPerSec, &secToUs)) {
        MEDIA_ERR_LOG("%s calculate result overflow!", errTag);
        return false;
    }
    timeUs = secToUs + static_cast<uint64_t>(ts.tv_nsec) / nsecPerUsec;
    return true;
}

void MediaRecordAudioRecvTime()
{
    uint64_t timeUs = 0;
    if (!GetMonotonicTimeUs(timeUs, "MediaRecordAudioRecvTime")) {
        return;
    }
    g_receiveAudiotime = timeUs;
}

void MediaPrintSyncTimeDiffUs()
{
    uint64_t current = g_receiveAudiotime.load();
    uint64_t expected = g_receiveAudiotimeLast.load();
    if (expected == current) {
        return;
    }
    /* Atomically claim this timestamp; fail if another thread already updated last. */
    if (!g_receiveAudiotimeLast.compare_exchange_strong(expected, current)) {
        return;
    }
    uint64_t endUsec = 0;
    if (!GetMonotonicTimeUs(endUsec, "MediaPrintSyncTimeDiffUs")) {
        return;
    }
    if (endUsec < current) {
        MEDIA_ERR_LOG("CalcTimeDiffUsPrint invalid time: endUsec=%" PRIu64 " current=%" PRIu64,
            endUsec, current);
        return;
    }
    MEDIA_DFX_LOG("audio and video sync elapsed time: %" PRIu64 " us", endUsec - current);
}
#endif

