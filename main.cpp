#include <iostream>
#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include "ThreadPool.hpp"
#include "FileScanner.hpp"
#include "InvertedIndex.hpp"
#include "FileReader.hpp"

std::atomic<int> activeTasks{0};
std::mutex mtx;
std::condition_variable cv;

int main() {
    std::cout << "File Indexer starting..." << std::endl;
    
    const int WORKER_THREADS = std::thread::hardware_concurrency();
    ThreadPool pool(WORKER_THREADS);

    // get all files from input directory
    std::string path = "C:/VDI";
    std::vector<std::string> files = FileScanner::getAllFiles(path);
    std::cout << "Total Files Found :" << files.size() << std::endl;

    // enqueue the file reading and indexing task
    InvertedIndex index;
    for(const auto& file : files) {
        // increment task counter before starting the task
        ++activeTasks;

        pool.enqueue([file, &index]() {
            std::vector<std::string> words = FileReader::getWordsFromFile(file);
            for(const auto& word : words) {
                index.addWord(word, file);
            }
            std::cout << "Indexed: " << file << std::endl;

            // decrement counter after task execution finish
            --activeTasks;
            std::unique_lock<std::mutex> lock(mtx);
            cv.notify_one();
        });
    }

    // wait for all threads to finish it work
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [] { return activeTasks == 0; });

    std::cout << "Indexing Done" << std::endl;

    std::cout << "Total Words Indexed : " << index.getSize() << std::endl;

    std::cout << "Enter a word to search" << std::endl;
    std::string word;
    std::cin >> word;

    std::vector<std::string> searchedFiles = index.search(word);

    for(const auto& file : searchedFiles) {
        std::cout << file << std::endl;
    }

    std::cout << "Done." << std::endl;

    return 0;
}