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
};

struct Task {
    virtual ~Task() = default;
    virtual void tick(TickContext& ctx) = 0; 
};

class Scheduler {
    std::vector<std::unique_ptr<Task>> tasks;
    TickContext ctx;
public:
    explicit Scheduler(TickContext c);
    template<typename T, typename...Args>
    T* addTask(Args&&...args) {
        auto p = std::make_unique<T>(std::forward<Args>(args)...);
        T* raw = p.get();
        tasks.push_back(std::move(p));
        return raw;
    }
    void runTicks(size_t n);
};