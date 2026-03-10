#pragma once
#include <deque>
#include <string>
#include <vector>
#include <unordered_map>

class BlackBox {
public:
    explicit BlackBox(size_t capacity = 500)
        : cap_(capacity) {}

    void add(const std::string& line);

    std::vector<std::string> snapshot(size_t maxItems = 100) const;

    void setCapacity(size_t capacity);
    size_t capacity() const { return cap_; }
    size_t size() const { return buf_.size(); }

private:
    std::deque<std::string> buf_;
    size_t cap_;
};

class Diagnostics {
public:
    bool enabled() const { return diag_; }
    void trigger(int ticks = 20);
    void onTick();
    bool shouldSample(const std::string& key, size_t everyN = 50);

    BlackBox& blackbox() { return blackbox_; }
    const BlackBox& blackbox() const { return blackbox_; }

private:
    bool diag_{false};
    int diag_ticks_left_{0};
    BlackBox blackbox_;

    std::unordered_map<std::string, size_t> bucket_count_;
};