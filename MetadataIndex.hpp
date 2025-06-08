#pragma once

#include <memory>
#include "MetadataIndexStrategy.hpp"

class MetadataIndex {
private:
    std::unique_ptr<MetadataIndexStrategy> strategy;
public:
    MetadataIndex(std::unique_ptr<MetadataIndexStrategy> strat) : strategy(std::move(strat)) {}

    void addFile(const std::string& filePath) {
        strategy->add(filePath);
    }

    std::vector<std::string> search(const std::string& param) {
        return strategy->query(param);
    }
};