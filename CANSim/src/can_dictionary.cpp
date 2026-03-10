#include <cstdio>
#include "can_dictionary.hpp"

std::string CanDictionary::narrativeFor(const CanFrame& f) const {
    auto it = byId.find(f.id);
    if (it == byId.end()) return "Nieznana ramka CAN.";
    return it->second.toNarrative(f);
}

std::map<std::string,std::string> CanDictionary::detailsFor(const CanFrame& f) const {
    auto it = byId.find(f.id);
    if (it == byId.end()) {
        return {{"can_id", toHex(f.id)}, {"dlc", std::to_string(f.dlc)}};
    }
    auto d = it->second.toDetails(f);
    d["can_id"] = toHex(f.id);
    d["dlc"] = std::to_string(f.dlc);
    return d;
}

std::string CanDictionary::toHex(uint32_t v) {
    char b[16]; std::snprintf(b, sizeof(b), "0x%03X", v);
    return std::string(b);
}

CanDictionary buildDict() {
    CanDictionary d;

    d.byId[0x100] = {
        "DoorStatus",
        [](const CanFrame& f) -> std::string {
            bool open = (f.data[0] & 0x01);
            return open ? "Drzwi kierowcy są otwarte." : "Drzwi kierowcy zostały zamknięte.";
        },
        [](const CanFrame& f) -> std::map<std::string, std::string> {
            bool open = (f.data[0] & 0x01);
            return std::map<std::string, std::string>{
                {"signal", "DoorStatus"},
                {"state", open ? "OPEN" : "CLOSED"}
            };
        }
    };

    d.byId[0x180] = {
        "BrakeRequest",
        [](const CanFrame& f) -> std::string {
            int pct = f.data[0];
            if (pct == 0)
                return std::string("Układ hamulcowy otrzymuje polecenie zwolnienia.");
            return std::string("Układ hamulcowy otrzymuje prośbę o dohamowanie: ") + std::to_string(pct) + "%.";
        },
        [](const CanFrame& f) -> std::map<std::string, std::string> {
            int pct = f.data[0];
            return std::map<std::string, std::string>{
                {"signal", "BrakeRequest"},
                {"percent", std::to_string(pct)}
            };
        }
    };

    return d;
}