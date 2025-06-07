#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <sstream>

class FileReader {
public:
    // returns all the words from given file
    static std::vector<std::string> getWordsFromFile(const std::string& filePath) {
        std::vector<std::string> words;
        std::ifstream file(filePath);
        
        // return empty if file cannot be opened
        if(!file.is_open()) {
            return words;
        }

        std::string line;
        while(std::getline(file, line)) {
            std::istringstream iss(line);
            std::string word;
            while(iss >> word) {
                std::string cleanWord;
                for(char ch : word) {
                    if(std::isalpha(ch) || std::isdigit(ch)) {
                        cleanWord += std::tolower(ch);
                    }
                }

                if(!cleanWord.empty()) {
                    words.push_back(cleanWord);
                }
            }
        }

        file.close();

        return words;
    }
};