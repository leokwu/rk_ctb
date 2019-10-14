//
// Created by 张昊 on 2017/9/25.
//

#ifndef PROJECT_LOG_H
#define PROJECT_LOG_H
#include <stdio.h>
#include <string>
#include <fstream>
#include <libgen.h>

#define LOGD(fmt, args...) do{SLog::print_log(stdout, "[%s] D/: " fmt "\n",SLog::getLogTime().c_str(), ##args);}while(false)
#define LOGW(fmt, args...) do{SLog::print_log(stderr, "[%s] W/: (%s in %s:%d): " fmt "\n", SLog::getLogTime().c_str(), __func__, basename((char *)__FILE__), __LINE__, ##args);}while(false)
#define LOGE(fmt, args...) do{SLog::print_log(stderr, "[%s] E/: (%s in %s:%d): " fmt "\n", SLog::getLogTime().c_str(), __func__, basename((char *)__FILE__), __LINE__, ##args);}while(false)
#define ASSERT(cond, fmt, ...) do{ if (!(cond)) {LOGE(fmt, ##__VA_ARGS__);abort();}} while(0)

#define DUMPDATA(ptr, len) do{SLog::dumpData(ptr, len);} while(false)

#define CTB_LOG_PATH "/var/log/ctb.werr"
#define CTB_LAST_LOG_PATH "/var/log/ctb_last.werr"

enum LogLevel {
    LOG_LOWLEST = 0,
    LOG_ERROR = 0,
    LOG_WARNING = 1,
    LOG_VERBOSE = 2,
    LOG_HIGHTEST = 3
};

class SLog {
public:
    static void print_log(FILE* fout, const char* fmt, ...);
    static void setEnable(bool enable);
    static std::string getLogTime();
    static void writeToFile(std::string line);
    static void dumpData(const unsigned char *buf, size_t length);

    static void createNewLog();
    static void saveLastLog();
private:
    static bool isEnable;
    static std::ofstream logStm;
};

static unsigned long long nanoTime(void)
{
    struct timespec t;

    t.tv_sec = t.tv_nsec = 0;
    clock_gettime(CLOCK_MONOTONIC, &t);

    return (unsigned long long)(t.tv_sec) * 1000000000 + t.tv_nsec;
}

template< class T, size_t N >
static constexpr std::size_t ARRAY_LENGTH(const T (&)[N] )
{
    return N;
}

#endif //PROJECT_LOG_H
