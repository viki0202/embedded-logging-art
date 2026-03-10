#pragma once
#include "logger.hpp"

#define LOG_LEVEL_ERROR 0
#define LOG_LEVEL_WARN  1
#define LOG_LEVEL_INFO  2
#define LOG_LEVEL_DEBUG 3

#ifndef LOG_LEVEL_MIN
#define LOG_LEVEL_MIN LOG_LEVEL_INFO
#endif

#include <cstdarg>
#include <cstdio>
#include <string>
#include <map>

using LogDetails = std::map<std::string,std::string>;

// DTL({"k1","v1"}, {"k2","v2"})
#define DTL(...) (LogDetails{ __VA_ARGS__ })
#define DTL_EMPTY LogDetails{}

inline std::string _logx_fmt(const char* fmt, ...) {
    char buf[512];
    va_list ap;
    va_start(ap, fmt);
    std::vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    return std::string(buf);
}

#if LOG_LEVEL_MIN >= LOG_LEVEL_ERROR
  #define LOGE(acr, acn, cor, trc, nar, dtl) do { \
      LOG.log({ "", "ERROR", (acr), (acn), (cor), (trc), (nar), (dtl) }); \
  } while(0)
#else
  #define LOGE(...) do {} while(0)
#endif

#if LOG_LEVEL_MIN >= LOG_LEVEL_WARN
  #define LOGW(acr, acn, cor, trc, nar, dtl) do { \
      LOG.log({ "", "WARN", (acr), (acn), (cor), (trc), (nar), (dtl) }); \
  } while(0)
#else
  #define LOGW(...) do {} while(0)
#endif

#if LOG_LEVEL_MIN >= LOG_LEVEL_INFO
  #define LOGI(acr, acn, cor, trc, nar, dtl) do { \
      LOG.log({ "", "INFO", (acr), (acn), (cor), (trc), (nar), (dtl) }); \
  } while(0)
#else
  #define LOGI(...) do {} while(0)
#endif

#if LOG_LEVEL_MIN >= LOG_LEVEL_DEBUG
  #define LOGD(acr, acn, cor, trc, nar, dtl) do { \
      LOG.log({ "", "DEBUG", (acr), (acn), (cor), (trc), (nar), (dtl) }); \
  } while(0)
#else
  #define LOGD(...) do {} while(0)
#endif