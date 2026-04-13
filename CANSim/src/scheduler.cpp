#include "scheduler.hpp"
#include "log_levels.hpp"

Scheduler::Scheduler(TickContext c): ctx(std::move(c)) {}

void Scheduler::runTicks(size_t n) {
    for (size_t i = 0; i < n; ++i) {

        LOGD("SCHED","{ TickBegin", ctx.corr_id, "tick-begin",
             "Scheduler rozpoczyna nowy tik.",
             DTL({"now", std::to_string(ctx.now)},
                 {"current", std::to_string(current)},
                 {"slice", std::to_string(slice)}));

        if (tasks.empty()) {
            LOGD("SCHED","Idle", ctx.corr_id, "idle-no-tasks",
                 "Brak zadan, scheduler w stanie idle.",
                 DTL_EMPTY);

            if (ctx.can) ctx.can->tick_bus();
            ctx.now++;
            continue;
        }

        size_t attempts = 0;
        size_t idx = current;

        while (attempts < tasks.size() && !tasks[idx]->ready(ctx.now)) {
            LOGD("SCHED","SkipTask", ctx.corr_id, "task-sleeping",
                 "Zadanie pominiete, jeszcze spi.",
                 DTL({"task", tasks[idx]->name()},
                     {"now", std::to_string(ctx.now)},
                     {"next_run", std::to_string(tasks[idx]->next_run)}));

            idx = (idx + 1) % tasks.size();
            attempts++;
        }

        if (attempts == tasks.size()) {
            LOGD("SCHED","Idle", ctx.corr_id, "idle-no-ready",
                 "Zadne zadanie nie jest gotowe, idle tick.",
                 DTL({"now", std::to_string(ctx.now)}));

            if (ctx.can) ctx.can->tick_bus();
            ctx.now++;
            continue;
        }

        current = idx;

        LOGI("SCHED","RunTask", ctx.corr_id, "task-run",
             "Scheduler uruchamia zadanie.",
             DTL({"task", tasks[current]->name()},
                 {"now", std::to_string(ctx.now)}));

        tasks[current]->tick(ctx);

        slice++;
        if (slice >= quantum) {
            LOGD("SCHED","QuantumEnd", ctx.corr_id, "quantum-expired",
                 "Kwant czasu wyczerpany, przelaczenie zadania.",
                 DTL({"task", tasks[current]->name()}));

            slice = 0;
            current = (current + 1) % tasks.size();
        }

        if (ctx.can) ctx.can->tick_bus();

        ctx.now++;

        LOGD("SCHED","} TickEnd", ctx.corr_id, "tick-end",
             "Scheduler konczy tik.",
             DTL({"now", std::to_string(ctx.now)}));
    }
}
