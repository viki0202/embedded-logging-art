#include "can.hpp"

void CanDriverSim::send(const CanFrame& f) { txq.push(f); }

void CanDriverSim::tick_bus() {
    while(!txq.empty()) { rxq.push(txq.front()); txq.pop(); }
}

bool CanDriverSim::recv(CanFrame& out) {
    if (rxq.empty()) return false;
    out = rxq.front(); rxq.pop();
    return true;
}