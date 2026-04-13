#include "tasks.hpp"
#include "log_levels.hpp"
#include <string>

bool CanTxTask::tick(TickContext& ctx) {
    if (!ctx.can) return false;

    if (step == 0) {
        CanFrame f{0x100, 1, {0x01}};
        ctx.can->send(f);
        LOGI("CAN.Tx","FrameSend",ctx.corr_id,"tx-door-open",
            "Kierowca otwiera drzwi.",DTL({"signal","DoorStatus"},{"value","OPEN"}));
        step++;
        this->sleep(3, ctx.now);   
        counter++;
        return true;
    }
    else if (step == 3) {
        CanFrame f{0x100, 1, {0x00}};
        ctx.can->send(f);
        LOGI("CAN.Tx","FrameSend",ctx.corr_id,"tx-door-close",
                 "Kierowca zamyka drzwi.",DTL({"signal","DoorStatus"},{"value","CLOSED"}));
        step++;
        this->sleep(2, ctx.now);
        counter++;
        return true;
    }
    else if (step == 6) {
        uint8_t pct = 10;
        CanFrame f{0x180, 1, {pct}};
        ctx.can->send(f);
        LOGI("CAN.Tx","FrameSend",ctx.corr_id,"tx-brake-10",
             "Pojazd prosi układ hamulcowy o lekkie dohamowanie (10%).",
             DTL({"signal","BrakeRequest"},{"percent","10"}));

        step++;
        this->sleep(5, ctx.now);   
        counter++;
        return true;
    }
    else if (step == 10) {
        uint8_t pct = 0;
        CanFrame f{0x180, 1, {pct}};
        ctx.can->send(f);
        LOGI("CAN.Tx","FrameSend",ctx.corr_id,"tx-brake-0",
                 "Pojazd przestaje hamować.",DTL({"signal","BrakeRequest"},{"percent","0"}));
        step++;
        counter++;
        return true;
    }
    else {
        step++;
        counter++;
        this->sleep(1, ctx.now);
        return false;
    }
}

bool CanRxTask::tick(TickContext& ctx) {
    if (!ctx.can || !ctx.dict) {
        this->sleep(1, ctx.now);
        return false;
    }
    CanFrame f{};
    if (ctx.can->recv(f)) {
        auto narrative = ctx.dict->narrativeFor(f);
        auto details   = ctx.dict->detailsFor(f);
        LOGI("CAN.Rx","FrameRecv",ctx.corr_id,"rx-"+details["can_id"],
                 narrative, details);
        this->sleep(0, ctx.now); 
        return true;
    }
    this->sleep(1, ctx.now);
    return false;
}

bool HousekeepingTask::tick(TickContext& ctx) {
    // Zamiast modulo – harmonogram przez sleep co 5 tików.
        LOGD("SYS","Heartbeat",ctx.corr_id,"hb",
                 "System daje znak życia.", DTL({"tick", std::to_string(beat)}));
    beat++;

    this->sleep(5, ctx.now);
    return false;
}

bool SafetyMonitorTask::tick(TickContext& ctx) {
    tickCount++;

    if (tickCount % expectedCycle == 0) {
        LOGI("Safety", "Cycle", ctx.corr_id, "safety-cycle",
                 "Zadanie SafetyMonitor wykonuje cykl kontrolny.",
                 DTL({"cycle","ok"}, {"tick", std::to_string(tickCount)}));
        this->sleep(expectedCycle, ctx.now);
        return false;
    } else if (tickCount % expectedCycle == 1) {
        LOGW("Safety", "JitterCheck", ctx.corr_id, "safety-jitter",
                 "SafetyMonitor wykrywa potencjalny jitter.",
                 DTL({"tick", std::to_string(tickCount)}));
        if (expectedCycle > 1) {
            this->sleep(expectedCycle - 1, ctx.now);
        } else {
            this->sleep(1, ctx.now);
        }
        return false;
    }
    this->sleep(1, ctx.now);
    return false;
}

bool LogStormTask::tick(TickContext& ctx) {
    for (int i = 0; i < 10; i++) {
        LOGD("Storm", "Spam",
                 ctx.corr_id, "storm-"+std::to_string(counter+i),
                 "Moduł LogStorm generuje intensywne logowanie.",
                 DTL({"seq", std::to_string(counter+i)}));
    }
    counter += 10;
    this->sleep(1, ctx.now);
    return false;
}