#pragma once
#include "FileManager.h"
#include "SchedulingEngine.h"
#include <filesystem>
#include <memory>
#include <string>
#include <optional>
#include <vector>

class ConsoleUI {
private:
    FileManager fileManager_;
    SchedulingEngine engine_;
    std::vector<Process> workload_;
    std::optional<SimulationResult> lastResult_;

    void showMainMenu() const;
    void showWorkload() const;
    void loadSampleWorkload();
    void loadWorkloadFromFile();
    void createCustomWorkload();
    void runOneSimulation();
    void compareAlgorithms();
    void resourceMenu();
    void exportLastResult() const;
    int readInt(const std::string& prompt, int minimum, int maximum) const;
    std::string readText(const std::string& prompt) const;
    std::unique_ptr<SchedulingPolicy> createPolicy(int choice) const;
    void printResult(const SimulationResult& result) const;
    void printGantt(const std::vector<GanttSegment>& timeline) const;
    void printMetrics(const SimulationMetrics& metrics) const;

public:
    explicit ConsoleUI(const std::filesystem::path& root);
    void run();
};
