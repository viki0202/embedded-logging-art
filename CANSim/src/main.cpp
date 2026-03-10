#include "utils.hpp"
#include "scheduler.hpp"
#include "tasks.hpp"
#include "can_dictionary.hpp"
#include "diag.hpp"
#include "log_levels.hpp"
#include <iostream>

static void dumpBlackBox(const Diagnostics& diag, size_t maxItems = 20) {
    auto snap = diag.blackbox().snapshot(maxItems);
    std::cout << "\n=== SNAPSHOT BlackBox (ostatnie " << snap.size() << " wpisów) ===\n";
    for (size_t i = 0; i < snap.size(); ++i) {
        std::cout << "[" << i << "] " << snap[i] << "\n";
    }
    std::cout << "=== END SNAPSHOT ===\n";
}

int main() {
    setupUTF8();

    CanDriverSim can;
    auto dict = buildDict();
    Diagnostics diag;

    LOG.diag = &diag;

    TickContext ctx;
    ctx.can      = &can;
    ctx.dict     = &dict;
    ctx.corr_id  = "drive#42";
    ctx.diag     = &diag;   

    Scheduler sch(ctx);
    sch.addTask<CanTxTask>();
    sch.addTask<CanRxTask>();
    sch.addTask<HousekeepingTask>();
    sch.addTask<LogStormTask>();       
    sch.addTask<SafetyMonitorTask>();


    LOGI("Main","Boot", ctx.corr_id, "boot#1",
        "Start CANSim z diagnostyką w kontekście.",DTL({"mode","diag-on"}));


    sch.runTicks(20);
    dumpBlackBox(diag, 25);

    diag.trigger(5);
    LOGW("Main","DiagTrigger", ctx.corr_id, "diag#kick",
        "Ręczne włączenie trybu diagnostycznego na 5 tików.",DTL_EMPTY); 

    sch.runTicks(8);
    dumpBlackBox(diag, 30);

    return 0;
}