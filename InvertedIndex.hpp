#pragma once

#include <unordered_map>
#include <set>
#include <mutex>

class InvertedIndex {
private:
    // each word with set of files containing that word
    std::unordered_map<std::string, std::set<std::string>> index;
    // mutex for index map for thread safety
    mutable std::mutex indexMutex;
public:
    int getSize() {
        return index.size();
    }

    // method to add word and its corresponding file into index map
    void addWord(const std::string& word, const std::string& file) {
        std::lock_guard<std::mutex> lock(indexMutex);
        index[word].insert(file);
    }

    // method to search word and return list of files containing it
    std::vector<std::string> search(const std::string& word) const {
        std::lock_guard<std::mutex> lock(indexMutex);

        std::vector<std::string> result;
        auto it = index.find(word);
        if(it != index.end()) {
            result.assign(it->second.begin(), it->second.end());
        }

        return result;
    }
};