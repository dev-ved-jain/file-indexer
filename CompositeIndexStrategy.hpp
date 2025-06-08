#pragma once

#include "MetadataIndexStrategy.hpp"
#include "QueryRequest.hpp"
#include <memory>
#include <set>
#include <algorithm>
#include <iterator>
#include <vector>

class CompositeIndexStrategy : public MetadataIndexStrategy {
private:
    std::vector<std::shared_ptr<MetadataIndexStrategy>> strategies;

public:
    void addStrategy(const std::shared_ptr<MetadataIndexStrategy>& strategy) {
        strategies.push_back(strategy);
    }

    void add(const std::string& filePath) override {
        for (const auto& strategy : strategies) {
            strategy->add(filePath);
        }
    }

    std::vector<std::string> query(const QueryRequest& request) override {
        if (strategies.empty()) return {};

        std::set<std::string> resultSet(
            strategies[0]->query(request).begin(),
            strategies[0]->query(request).end()
        );

        for (size_t i = 1; i < strategies.size(); ++i) {
            std::set<std::string> currentSet(
                strategies[i]->query(request).begin(),
                strategies[i]->query(request).end()
            );

            std::set<std::string> intersection;
            std::set_intersection(
                resultSet.begin(), resultSet.end(),
                currentSet.begin(), currentSet.end(),
                std::inserter(intersection, intersection.begin())
            );

            resultSet = std::move(intersection);
        }

        return std::vector<std::string>(resultSet.begin(), resultSet.end());
    }
};
