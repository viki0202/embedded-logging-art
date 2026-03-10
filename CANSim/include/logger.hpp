#pragma once
#include <string>
#include <map>

class Diagnostics;

struct LogRecord {
    std::string ts, level, actor, action, corr_id, trace_id, narrative;
    std::map<std::string, std::string> details;
};

class Logger {
public:
    Diagnostics* diag{nullptr};
    void log(const LogRecord& r);
};

extern Logger LOG;