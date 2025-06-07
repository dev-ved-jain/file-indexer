#pragma once


#include <filesystem>

class FileScanner {
public:
    // given a directory get all files inside it
    static std::vector<std::string> getAllFiles(const std::string& directoryPath) {
        std::vector<std::string> fileList;
        
        for(const auto& entry : std::filesystem::recursive_directory_iterator(directoryPath)) {
            if(entry.is_regular_file()) {
                fileList.push_back(entry.path().string());
            }
        }

        return fileList;
    }
};