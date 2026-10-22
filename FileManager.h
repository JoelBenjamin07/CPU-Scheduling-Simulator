#pragma once
#include "Process.h"
#include "ResourceManager.h"
#include "SchedulingEngine.h"
#include <filesystem>
#include <string>
#include <vector>

class FileManager {
private:
    std::filesystem::path root_;

public:
    explicit FileManager(const std::filesystem::path& root);

    const std::filesystem::path& getRoot() const;
    std::vector<Process> loadWorkload(const std::string& filename) const;
    std::filesystem::path saveWorkload(const std::vector<Process>& processes, const std::string& filename) const;
    ResourceManager loadResources(const std::string& filename) const;
    std::filesystem::path saveReport(const SimulationResult& result) const;
};
