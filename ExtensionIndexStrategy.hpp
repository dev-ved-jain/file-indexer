#pragma once

#include "MetadataIndexStrategy.hpp"
#include <unordered_map>
#include <vector>
#include <string>
#include <mutex>

class ExtensionIndexStrategy : public MetadataIndexStrategy {
private:
    std::unordered_map<std::string, std::vector<std::string>> extensionToFileMap;
    mutable std::mutex mapMutex;

    std::string getFileExtension(const std::string& filePath) {
        size_t pos = filePath.find_last_of(".");
        
        if(pos == std::string::npos) return "";

        return filePath.substr(pos + 1);
    }

public:
    void add(const std::string& filePath) override {
        std::string fileExtension = getFileExtension(filePath);

        extensionToFileMap[fileExtension].push_back(filePath);
    }

    std::vector<std::string> query(const QueryRequest& request) override {
        std::vector<std::string> fileList;

        // Check if this strategy is being called correctly
        if (request.strategyType != "extension") {
            return fileList;
        }

        // Check for required param
        auto it = request.params.find("value");
        if (it == request.params.end()) {
            return fileList;
        }

        const std::string& extensionValue = it->second;

        {
            std::lock_guard<std::mutex> lock(mapMutex);

            auto extIt = extensionToFileMap.find(extensionValue);
            if (extIt != extensionToFileMap.end()) {
                fileList = extIt->second;
            }
        }

        return fileList;
    }
};