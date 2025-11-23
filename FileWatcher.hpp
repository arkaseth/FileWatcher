#pragma once

#include <filesystem>
#include <chrono>
#include <thread>
#include <unordered_map>
#include <string>
#include <functional>

enum class FileStatus { created, modified, erased };

class FileWatcher {
public:
    std::string path;
    // Time interval for checking base folder for changes
    std::chrono::duration<int, std::milli> delay;

    // Record of files from base dir and their last modified time
    FileWatcher(std::string path, std::chrono::duration<int, std::milli> delay) : path(path), delay(delay) {
        for (auto &file : std::filesystem::recursive_directory_iterator(path)) {
            paths_[file.path().string()] = std::filesystem::last_write_time(file);
        }
    }

private:
    std::unordered_map<std::string, std::filesystem::file_time_type> paths_;
};