//
// Created by 张昊 on 2017/9/25.
//

#include <cstdarg>
#include <time.h>
#include <sys/time.h>
#include <string>
#include "slog.h"

bool SLog::isEnable = true;
std::ofstream SLog::logStm;

void SLog::print_log(FILE *fout, const char *fmt, ...) {
    if (isEnable) {
        va_list va;
        va_start(va, fmt);
        vfprintf(fout, fmt, va);
        va_end(va);
    }

    if (fout == stderr) {
        char ln[1024] = {0};
        va_list va;
        va_start(va, fmt);
        vsprintf(ln, fmt, va);
        va_end(va);
        writeToFile(std::string(ln));
    }
}

void SLog::setEnable(bool enable) {
    isEnable = enable;
}

std::string SLog::getLogTime() {
    struct timeval tv;
    struct timezone tz;
    gettimeofday(&tv, &tz);

    char tmp[30];
    auto tp = localtime(&tv.tv_sec);
    sprintf(tmp, "%02d/%02d %02d:%02d:%02d.%03d",
            1+tp->tm_mon, tp->tm_mday,
            tp->tm_hour, tp->tm_min, tp->tm_sec, tv.tv_usec/1000);
    return std::string(tmp);
}

void SLog::writeToFile(std::string line) {
    if (SLog::logStm.is_open()) {
        SLog::logStm << line;
        SLog::logStm.flush();
    }
}

void SLog::saveLastLog() {
    const char *log = CTB_LOG_PATH;
    const char *last = CTB_LAST_LOG_PATH;

    rename(log, last);
}

void SLog::createNewLog() {
    logStm.open(CTB_LOG_PATH);
}

void SLog::dumpData(const unsigned char *buf, size_t length) {
    LOGD("dump data: ");
    int n = 0;
    char dat[17] = {0};
    for (size_t i = 0; i < length; ++i) {
        dat[n] = buf[i];
        printf("%02x ", buf[i]);
        if (++n >= 16 || i == length - 1) {
            if (i == length - 1) {
                if (length % 16 != 0) {
                    for (size_t blk = 0; blk < 16 - length % 16; ++blk) {
                        printf("   ");
                    }
                }
            }

            printf("\t");
            for (int k = 0; k < n; k++) {
                if (dat[k] >= 32 && dat[k] <= 126) {
                    printf("%c", dat[k]);
                } else {
                    printf(".");
                }
            }
            printf("\n");
            n = 0;
        }
    }
    printf("\n");
}
