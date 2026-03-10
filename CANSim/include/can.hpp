#pragma once
#include <cstdint>
#include <array>
#include <queue>

struct CanFrame {
    uint32_t id{0};
    uint8_t  dlc{0};
    std::array<uint8_t,8> data{};
};

class CanDriverSim {
    std::queue<CanFrame> txq, rxq;
public:
    void send(const CanFrame& f);
    void tick_bus();              
    bool recv(CanFrame& out);
};