#pragma once

#include <string>
#include <vector>
#include "QueryRequest.hpp"

// defines Interface of Indexing strategy
class MetadataIndexStrategy {
public:
    // add a file with its metadata to index
    virtual void add(const std::string& filePath) = 0;

    // query based on some parameter
    virtual std::vector<std::string> query(const QueryRequest& request) = 0;

    virtual ~MetadataIndexStrategy() {}
};