#pragma once

#include <unordered_map>
#include <string>

struct QueryRequest {
    std::string strategyType;  // e.g. "extension", "size", "word"
    std::unordered_map<std::string, std::string> params;
};