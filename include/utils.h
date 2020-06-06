#ifndef LJSTACK_UTILS_H
#define LJSTACK_UTILS_H

#include <cstdio>
#include <chrono>
#include <cstring>

#define __FILENAME__ ((strrchr(__FILE__, '/')) + 1)
#define LOG_OUT(fmt, ...) fprintf(stdout, "[%s] [%s:%d %s] " fmt "\n", ljstack::GET_TIME().c_str(), __FILENAME__, \
__LINE__, __FUNCTION__, ##__VA_ARGS__)
#define LOG_ERR(fmt, ...) fprintf(stderr, "ERROR [%s] [%s:%d %s] " fmt "\n", ljstack::GET_TIME().c_str(), __FILENAME__, \
__LINE__, __FUNCTION__, ##__VA_ARGS__)

using hrs = std::chrono::high_resolution_clock;
using us = std::chrono::microseconds;
using ms = std::chrono::milliseconds;

namespace ljstack {
    // 计算时间差
    template <class T>
    inline long cal_deltatime(hrs::time_point t1, hrs::time_point t2) {
        return std::chrono::duration_cast<T>(t2 - t1).count();
    }

    inline std::string GET_TIME() {
        char timebuf[80];
        std::time_t t1 = hrs::to_time_t(hrs::now());
        std::strftime(timebuf, 80, "%Y-%m-%d %H:%M:%S", std::localtime(&t1));
        return std::string(timebuf);
    }
}

#endif //LJSTACK_UTILS_H
