#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>
#include <functional>
#include <map>
#include "can.hpp"

struct CanDictionary {
    struct Entry {
        std::string signal;
        std::function<std::string(const CanFrame&)> toNarrative;
        std::function<std::map<std::string,std::string>(const CanFrame&)> toDetails;
    };
    std::unordered_map<uint32_t, Entry> byId;

    std::string narrativeFor(const CanFrame& f) const;
    std::map<std::string,std::string> detailsFor(const CanFrame& f) const;

    static std::string toHex(uint32_t v);
};

CanDictionary buildDict();