#pragma once

#include <memory>
#include "IndexStrategy.hpp"
#include "ExtensionIndexStrategy.hpp"
#include "SizeIndexStrategy.hpp"

class IndexStrategyFactory {
public:
    static std::unique_ptr<IndexStrategy> createStrategy(const std::string& userChoice) {
        if(userChoice == "extension") {
            return std::make_unique<ExtensionIndexStrategy>();
        } else if(userChoice == "size") {
            return std::make_unique<SizeIndexStrategy>();
        }
        return nullptr;
    }
};