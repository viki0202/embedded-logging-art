#pragma once
#include <memory>
#include <vector>
#include "can.hpp"
#include "diag.hpp"
#include "can_dictionary.hpp"

struct TickContext {
    std::string corr_id{"drive#42"};
    CanDriverSim* can{nullptr};
    const CanDictionary* dict{nullptr};
    Diagnostics* diag{nullptr};
    uint64_t now = 0;
};

struct Task {
    virtual ~Task() = default;
    virtual bool tick(TickContext& ctx) = 0;
    virtual std::string name() const = 0;
    uint64_t next_run = 0;
    bool ready(uint64_t now) const noexcept { return now >= next_run; }
    void sleep(uint64_t ticks, uint64_t now) noexcept { next_run = now + ticks; }
};

class Scheduler {
    std::vector<std::unique_ptr<Task>> tasks;
    TickContext ctx;
    size_t current = 0;
    size_t quantum = 3;
    size_t slice = 0;
public:
    explicit Scheduler(TickContext c);

    void addTask(std::unique_ptr<Task> task) {tasks.push_back(std::move(task));}
    void runTicks(size_t n);
};