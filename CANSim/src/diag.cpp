#include "diag.hpp"

void BlackBox::add(const std::string& line) {
    if (cap_ == 0) return;
    if (buf_.size() == cap_) buf_.pop_front();
    buf_.push_back(line);
}

std::vector<std::string> BlackBox::snapshot(size_t maxItems) const {
    std::vector<std::string> out;
    if (maxItems == 0 || buf_.empty()) return out;
    const size_t start = (buf_.size() > maxItems) ? (buf_.size() - maxItems) : 0;
    out.reserve(buf_.size() - start);
    for (size_t i = start; i < buf_.size(); ++i) {
        out.push_back(buf_[i]);
    }
    return out;
}

void BlackBox::setCapacity(size_t capacity) {
    cap_ = capacity;
    while (buf_.size() > cap_) {
        buf_.pop_front();
    }
}

void Diagnostics::trigger(int ticks) {
    if (ticks <= 0) {
        diag_ = true;
        diag_ticks_left_ = 1;
        return;
    }
    diag_ = true;
    diag_ticks_left_ = ticks;
}

void Diagnostics::onTick() {
    if (!diag_) return;
    if (--diag_ticks_left_ <= 0) {
        diag_ = false;
        diag_ticks_left_ = 0;
    }
}

bool Diagnostics::shouldSample(const std::string& key, size_t everyN) {
    size_t& c = bucket_count_[key];
    ++c;
    if (diag_) return true;
    if (c == 1) return true;
    if (everyN == 0) return false; 
    return (c % everyN) == 0;
}