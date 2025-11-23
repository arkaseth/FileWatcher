#include <iostream>
#include "FileWatcher.hpp"

int main()
{
    FileWatcher fw("./", std::chrono::milliseconds(5000));

    fw.start([](std::string path, FileStatus status) -> void
             {
        if (!std::filesystem::is_regular_file(std::filesystem::path(path)) && status != FileStatus::erased) {
            return;
        }

        switch (status) {
            case FileStatus::created:
                std::cout << "File created: " << path << "\n";
                break;
            case FileStatus::modified:
                std::cout << "File modified: " << path << "\n";
                break;
            case FileStatus::erased:
                std::cout << "File erased: " << path << "\n";
                break;
            default:
                std::cout << "Unknown file status.\n";
        } });
}