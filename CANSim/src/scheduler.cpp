#include "scheduler.hpp"

Scheduler::Scheduler(TickContext c): ctx(std::move(c)) {}

void Scheduler::runTicks(size_t n) {
    for (size_t i=0; i<n; ++i) {
        for (auto& t : tasks) t->tick(ctx);
        if (ctx.can) ctx.can->tick_bus();
    }
}