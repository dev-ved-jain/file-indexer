#pragma once

#include <map>
#include <filesystem>
#include <iostream>
#include <vector>
#include <string>
#include <mutex>
#include "MetadataIndexStrategy.hpp"
#include "QueryRequest.hpp"

class SizeIndexStrategy : public MetadataIndexStrategy {
private:
    std::map<uintmax_t, std::vector<std::string>> sizeToFileMap;
    mutable std::mutex mapMutex;

public:
    void add(const std::string& filePath) override {
        try {
            uintmax_t size = std::filesystem::file_size(filePath);
            std::lock_guard<std::mutex> lock(mapMutex);
            sizeToFileMap[size].push_back(filePath);
        } catch (const std::filesystem::filesystem_error& error) {
            std::cerr << "Error reading size of the file: " << filePath << " -> " << error.what() << std::endl;
        }
    }

    std::vector<std::string> query(const QueryRequest& request) override {
        std::vector<std::string> result;

        if (request.strategyType != "size") {
            return result;
        }

        auto it = request.params.find("value");
        if (it == request.params.end()) {
            return result;
        }

        const std::string& param = it->second;

        std::lock_guard<std::mutex> lock(mapMutex);

        if (param.substr(0, 2) == "gt") {
            uintmax_t threshold = std::stoull(param.substr(3));
            for (auto it = sizeToFileMap.upper_bound(threshold); it != sizeToFileMap.end(); ++it) {
                result.insert(result.end(), it->second.begin(), it->second.end());
            }
        } else if (param.substr(0, 2) == "lt") {
            uintmax_t threshold = std::stoull(param.substr(3));
            for (auto it = sizeToFileMap.begin(); it != sizeToFileMap.lower_bound(threshold); ++it) {
                result.insert(result.end(), it->second.begin(), it->second.end());
            }
        } else if (param.substr(0, 7) == "between") {
            size_t pos1 = param.find(':', 8);
            uintmax_t low = std::stoull(param.substr(8, pos1 - 8));
            uintmax_t high = std::stoull(param.substr(pos1 + 1));
            for (auto it = sizeToFileMap.lower_bound(low); it != sizeToFileMap.upper_bound(high); ++it) {
                result.insert(result.end(), it->second.begin(), it->second.end());
            }
        }

        return result;
    }
};
