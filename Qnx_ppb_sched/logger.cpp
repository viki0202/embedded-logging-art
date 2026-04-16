#include <iostream>
#include <sstream>
#include "logger.hpp"
#include "utils.hpp"

Logger LOG;

void Logger::log(const LogRecord& r) {
    std::ostringstream oss;
    oss << "{"
        << "\"ts\":\""      << (r.ts.empty()? now_iso8601() : r.ts) << "\","
        << "\"level\":\""   << r.level    << "\","
        << "\"actor\":\""   << r.actor    << "\","
        << "\"action\":\""  << r.action   << "\","
        << "\"corr_id\":\"" << r.corr_id  << "\","
        << "\"trace_id\":\""<< r.trace_id << "\","
        << "\"narrative\":\""<< r.narrative << "\","
        << "\"details\":{";

    bool first = true;
    for (const auto& kv : r.details) {
        if (!first) oss << ",";
        first = false;
        oss << "\"" << kv.first << "\":\"" << kv.second << "\"";
    }
    oss << "}}";

    const std::string line = oss.str();
    std::cout << line << "\n";
}
