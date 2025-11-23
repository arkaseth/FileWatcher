#pragma once

#include <filesystem>
#include <chrono>
#include <thread>
#include <unordered_map>
#include <string>
#include <functional>

enum class FileStatus
{
    created,
    modified,
    erased
};

class FileWatcher
{
public:
    std::string path;
    // Time interval in milliseconds for checking base folder for changes
    std::chrono::duration<int, std::milli> delay;

    // Record of files from base dir and their last modified time
    FileWatcher(std::string path, std::chrono::duration<int, std::milli> delay) : path(path), delay(delay)
    {
        for (auto &file : std::filesystem::recursive_directory_iterator(path))
        {
            paths_[file.path().string()] = std::filesystem::last_write_time(file);
        }
    }

    // Monitor path for changes, and execute user-supplied action in case of a change
    void start(const std::function<void(std::string, FileStatus)> &action)
    {
        while (running_)
        {
            // Wait for "delay"
            std::this_thread::sleep_for(delay);

            // Check if file was deleted
            auto it = paths_.begin();
            while (it != paths_.end())
            {
                if (!std::filesystem::exists(it->first))
                {
                    action(it->first, FileStatus::erased);
                    it = paths_.erase(it);
                }
                else
                    it++;
            }

            // Check if file was modified or created
            for (auto &file : std::filesystem::recursive_directory_iterator(path))
            {
                auto current_file_last_write_time = std::filesystem::last_write_time(file);

                // File creation
                if (paths_.find(file.path().string()) == paths_.end()) // OR paths_.contains(file.path().string())
                {
                    paths_[file.path().string()] = current_file_last_write_time;
                    action(file.path().string(), FileStatus::created);
                }
                // File modification
                else
                {
                    if (paths_[file.path().string()] != current_file_last_write_time)
                    {
                        paths_[file.path().string()] = current_file_last_write_time;
                        action(file.path().string(), FileStatus::modified);
                    }
                }
            }
        }
    }

private:
    std::unordered_map<std::string, std::filesystem::file_time_type> paths_;
    bool running_ = true;
};