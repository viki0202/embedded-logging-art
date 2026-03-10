#ifdef _WIN32
  #include <io.h>
  #include <fcntl.h>
#endif

#include <locale.h>
#include <chrono>
#include <ctime>
#include <cstdio>
#include "utils.hpp"

void setupUTF8() {
#ifdef _WIN32
    setlocale(LC_ALL, ".UTF8");
#else
    setlocale(LC_ALL, "en_US.UTF-8");
#endif
}

std::string now_iso8601() {
    using namespace std::chrono;
    auto tp = system_clock::now();
    auto us = duration_cast<microseconds>(tp.time_since_epoch()) % 1000000;
    time_t t = system_clock::to_time_t(tp);
    std::tm tm = *gmtime(&t);
    char buf[64];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);
    char out[80];
    snprintf(out, sizeof(out), "%s.%06d", buf, (int)us.count());
    return std::string(out);
}