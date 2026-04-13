#pragma once
#include "scheduler.hpp"

class CanTxTask : public Task {
    size_t step{0};
    uint32_t counter{0};
    std::string name() const override { return "CanTx"; }
public:
    bool tick(TickContext& ctx) override;
};

class CanRxTask : public Task {
    std::string name() const override { return "CanRx"; }
public:
    bool tick(TickContext& ctx) override;
};

class HousekeepingTask : public Task {
    size_t beat{0};
    std::string name() const override { return "Housekeeping"; }
public:
    bool tick(TickContext& ctx) override;
};

class SafetyMonitorTask : public Task {
    size_t tickCount{0};
    const size_t expectedCycle = 3;
    std::string name() const override { return "SafetyMonitor"; }
public:
    bool tick(TickContext& ctx) override;
};

class LogStormTask : public Task {
    size_t counter{0};
    std::string name() const override { return "LogStorm"; }
public:
    bool tick(TickContext& ctx) override;
};