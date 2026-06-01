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

void MediaRecordAudioRecvTime()
{
    struct timeval timeStart;
    gettimeofday(&timeStart, nullptr);
    const uint64_t timeUs = 1000000;
    uint64_t secToUs = 0;
    bool isOverflow =
        UIntMulIsOverflow64(static_cast<uint64_t>(timeStart.tv_sec), timeUs, &secToUs);
    if (isOverflow) {
        MEDIA_ERR_LOG("PUIntMulIsOverflow64 calculate reslut overflow!");
        return;
    }
    g_receiveAudiotime = secToUs + timeStart.tv_usec;
}

void MediaPrintSyncTimeDiffUs()
{
    if (g_receiveAudiotimeLast == g_receiveAudiotime) {
        return;
    }
    g_receiveAudiotimeLast.store(g_receiveAudiotime.load());
    struct timeval timeEnd;
    gettimeofday(&timeEnd, nullptr);
    const uint64_t timeUs = 1000000;
    uint64_t secToUs = 0;
    bool isOverflow =
        UIntMulIsOverflow64(static_cast<uint64_t>(timeEnd.tv_sec), timeUs, &secToUs);
    if (isOverflow) {
        MEDIA_ERR_LOG("CalcTimeDiffUsPrint calculate reslut overflow!");
        return;
    }
    uint64_t endUsec = static_cast<uint64_t>(timeEnd.tv_usec + secToUs);
    MEDIA_DFX_LOG("audio and video sync elapsed time: %lld us", endUsec - g_receiveAudiotime);
}
#endif

