#include "ConsoleUI.h"
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <sstream>
#include <stdexcept>

ConsoleUI::ConsoleUI(const std::filesystem::path& root) : fileManager_(root) {}

void ConsoleUI::showMainMenu() const {
    std::cout << "\n========================================================\n";
    std::cout << "                       KERNELSIM\n";
    std::cout << "       OS Scheduling and Resource Management Simulator\n";
    std::cout << "========================================================\n";
    std::cout << "  1. View current workload\n";
    std::cout << "  2. Run a scheduling algorithm\n";
    std::cout << "  3. Compare scheduling algorithms\n";
    std::cout << "  4. Banker's algorithm and resource requests\n";
    std::cout << "  5. Create and save a custom workload\n";
    std::cout << "  6. Load a workload file\n";
    std::cout << "  7. Export last scheduling result\n";
    std::cout << "  0. Exit\n";
    std::cout << "--------------------------------------------------------\n";
}

int ConsoleUI::readInt(const std::string& prompt, int minimum, int maximum) const {
    while (true) {
        std::cout << prompt;
        std::string line;
        if (!std::getline(std::cin, line)) return 0;
        std::istringstream stream(line);
        int value = 0;
        char extra = '\0';
        if ((stream >> value) && !(stream >> extra) && value >= minimum && value <= maximum) {
            return value;
        }
        std::cout << "Enter a whole number from " << minimum << " to " << maximum << ".\n";
    }
}

std::string ConsoleUI::readText(const std::string& prompt) const {
    while (true) {
        std::cout << prompt;
        std::string value;
        if (!std::getline(std::cin, value)) return "";
        if (!value.empty()) return value;
        std::cout << "Input cannot be empty.\n";
    }
}

void ConsoleUI::showWorkload() const {
    if (workload_.empty()) {
        std::cout << "No workload loaded. Load the sample workload first.\n";
        return;
    }

    std::cout << "\nCurrent workload\n";
    std::cout << std::left << std::setw(12) << "PID"
              << std::setw(15) << "Arrival"
              << std::setw(12) << "Burst"
              << std::setw(12) << "Priority" << "\n";
    std::cout << "---------------------------------------------------\n";
    for (const auto& process : workload_) {
        std::cout << std::left << std::setw(12) << process.getId()
                  << std::setw(15) << process.getArrivalTime()
                  << std::setw(12) << process.getBurstTime()
                  << std::setw(12) << process.getPriority() << "\n";
    }
}

void ConsoleUI::loadSampleWorkload() {
    try {
        workload_ = fileManager_.loadWorkload("sample_workload.txt");
        lastResult_.reset();
        std::cout << "Loaded " << workload_.size() << " processes from data/sample_workload.txt.\n";
    } catch (const std::exception& error) {
        std::cout << "Could not load sample workload: " << error.what() << "\n";
    }
}


void ConsoleUI::loadWorkloadFromFile() {
    std::string filename = readText("File name inside data/ (for example sample_workload.txt): ");
    try {
        workload_ = fileManager_.loadWorkload(filename);
        lastResult_.reset();
        std::cout << "Loaded " << workload_.size() << " processes from data/" << filename << ".\n";
    } catch (const std::exception& error) {
        std::cout << "Could not load workload: " << error.what() << "\n";
    }
}

void ConsoleUI::createCustomWorkload() {
    int count = readInt("Number of processes: ", 1, 100);
    std::vector<Process> custom;
    std::vector<std::string> ids;
    custom.reserve(static_cast<std::size_t>(count));

    try {
        for (int i = 0; i < count; ++i) {
            std::cout << "\nProcess " << (i + 1) << "\n";
            std::string id = readText("Process ID: ");
            if (std::find(ids.begin(), ids.end(), id) != ids.end()) {
                throw std::invalid_argument("Process IDs must be unique. Duplicate ID: " + id);
            }
            int arrival = readInt("Arrival time (>= 0): ", 0, 1000000);
            int burst = readInt("Burst time (> 0): ", 1, 1000000);
            int priority = readInt("Priority (smaller number means higher priority): ", -100000, 100000);
            custom.emplace_back(id, arrival, burst, priority);
            ids.push_back(id);
        }

        std::filesystem::path saved = fileManager_.saveWorkload(custom, "custom_workload.txt");
        workload_ = custom;
        lastResult_.reset();
        std::cout << "\nCustom workload created and saved to " << saved.string() << ".\n";
    } catch (const std::exception& error) {
        std::cout << "Custom workload was not saved: " << error.what() << "\n";
    }
}

std::unique_ptr<SchedulingPolicy> ConsoleUI::createPolicy(int choice) const {
    switch (choice) {
        case 1: return std::make_unique<FCFSPolicy>();
        case 2: return std::make_unique<SJFPolicy>();
        case 3: return std::make_unique<SRTFPolicy>();
        case 4: return std::make_unique<PriorityPolicy>();
        case 5: {
            int quantum = readInt("Enter Round Robin time quantum: ", 1, 1000000);
            return std::make_unique<RoundRobinPolicy>(quantum);
        }
        default: throw std::invalid_argument("Unknown scheduling choice.");
    }
}

void ConsoleUI::runOneSimulation() {
    if (workload_.empty()) {
        std::cout << "No workload loaded. Load the sample workload first.\n";
        return;
    }

    std::cout << "\nScheduling algorithms\n";
    std::cout << "1. FCFS\n2. SJF (Non-preemptive)\n3. SRTF\n4. Priority (Non-preemptive)\n5. Round Robin\n";
    int choice = readInt("Choose algorithm: ", 1, 5);

    try {
        std::unique_ptr<SchedulingPolicy> policy = createPolicy(choice);
        lastResult_ = engine_.run(workload_, *policy);
        printResult(*lastResult_);
    } catch (const std::exception& error) {
        std::cout << "Simulation failed: " << error.what() << "\n";
    }
}

void ConsoleUI::compareAlgorithms() {
    if (workload_.empty()) {
        std::cout << "No workload loaded. Load the sample workload first.\n";
        return;
    }

    std::vector<std::unique_ptr<SchedulingPolicy>> policies;
    policies.push_back(std::make_unique<FCFSPolicy>());
    policies.push_back(std::make_unique<SJFPolicy>());
    policies.push_back(std::make_unique<SRTFPolicy>());
    policies.push_back(std::make_unique<PriorityPolicy>());
    policies.push_back(std::make_unique<RoundRobinPolicy>(2));

    std::cout << "\nComparison uses the same workload and Round Robin quantum 2.\n";
    std::cout << std::left << std::setw(27) << "Algorithm"
              << std::setw(12) << "Avg WT"
              << std::setw(12) << "Avg TAT"
              << std::setw(12) << "Avg RT"
              << std::setw(12) << "CPU %" << "\n";
    std::cout << "---------------------------------------------------------------------------\n";

    for (const auto& policy : policies) {
        SimulationResult result = engine_.run(workload_, *policy);
        std::cout << std::left << std::setw(27) << result.algorithmName
                  << std::fixed << std::setprecision(2)
                  << std::setw(12) << result.metrics.averageWaitingTime
                  << std::setw(12) << result.metrics.averageTurnaroundTime
                  << std::setw(12) << result.metrics.averageResponseTime
                  << std::setw(12) << result.metrics.cpuUtilization << "\n";
    }
}

void ConsoleUI::printGantt(const std::vector<GanttSegment>& timeline) const {
    std::cout << "\nGantt chart\n";
    for (const auto& segment : timeline) {
        std::cout << "| " << segment.processId << " ";
    }
    std::cout << "|\n";
    if (!timeline.empty()) {
        std::cout << timeline.front().startTime;
        for (const auto& segment : timeline) {
            std::cout << std::string(segment.processId.size() + 3, ' ')
                      << segment.endTime;
        }
        std::cout << "\n";
    }
}

void ConsoleUI::printMetrics(const SimulationMetrics& metrics) const {
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "\nAverage waiting time     : " << metrics.averageWaitingTime << "\n";
    std::cout << "Average turnaround time  : " << metrics.averageTurnaroundTime << "\n";
    std::cout << "Average response time    : " << metrics.averageResponseTime << "\n";
    std::cout << "CPU utilization          : " << metrics.cpuUtilization << "%\n";
    std::cout << "Throughput               : " << metrics.throughput << " processes/unit\n";
    std::cout << "Busy time                : " << metrics.busyTime << "\n";
    std::cout << "Total elapsed time       : " << metrics.totalTime << "\n";
}

void ConsoleUI::printResult(const SimulationResult& result) const {
    std::cout << "\nAlgorithm: " << result.algorithmName << "\n";
    printGantt(result.timeline);
    std::cout << "\n";
    std::cout << std::left << std::setw(10) << "PID"
              << std::setw(6) << "AT"
              << std::setw(6) << "BT"
              << std::setw(6) << "CT"
              << std::setw(7) << "TAT"
              << std::setw(7) << "WT"
              << std::setw(7) << "RT" << "\n";
    std::cout << "---------------------------------------------------------\n";
    for (const auto& process : result.processes) {
        std::cout << std::left << std::setw(10) << process.processId
                  << std::setw(6) << process.arrivalTime
                  << std::setw(6) << process.burstTime
                  << std::setw(6) << process.completionTime
                  << std::setw(7) << process.turnaroundTime
                  << std::setw(7) << process.waitingTime
                  << std::setw(7) << process.responseTime << "\n";
    }
    printMetrics(result.metrics);
}

void ConsoleUI::resourceMenu() {
    try {
        ResourceManager manager = fileManager_.loadResources("sample_resources.txt");
        while (true) {
            std::cout << "\nBanker's algorithm\n";
            std::cout << "1. Display resource state\n";
            std::cout << "2. Check safety\n";
            std::cout << "3. Request resources\n";
            std::cout << "0. Return to main menu\n";
            int choice = readInt("Choose option: ", 0, 3);
            if (choice == 0) return;

            if (choice == 1) {
                std::vector<int> available = manager.getAvailable();
                std::vector<std::vector<int>> allocation = manager.getAllocation();
                std::vector<std::vector<int>> maximum = manager.getMaximum();
                std::vector<std::vector<int>> need = manager.getNeed();

                std::cout << "\nAvailable: ";
                for (int value : available) std::cout << value << " ";
                std::cout << "\n\n";
                std::cout << std::left << std::setw(8) << "PID"
                          << std::setw(20) << "Allocation"
                          << std::setw(20) << "Maximum"
                          << std::setw(20) << "Need" << "\n";
                for (std::size_t i = 0; i < manager.getProcessIds().size(); ++i) {
                    std::cout << std::left << std::setw(8) << manager.getProcessIds()[i];
                    for (int value : allocation[i]) std::cout << value << " ";
                    std::cout << std::string(12, ' ');
                    for (int value : maximum[i]) std::cout << value << " ";
                    std::cout << std::string(12, ' ');
                    for (int value : need[i]) std::cout << value << " ";
                    std::cout << "\n";
                }
                std::cout << "Resource order: ";
                for (const auto& name : manager.getResourceNames()) std::cout << name << " ";
                std::cout << "\n";
            } else if (choice == 2) {
                SafetyResult safety = manager.evaluateSafety();
                if (safety.safe) {
                    std::cout << "Safe state: YES\nSafe sequence: ";
                    for (std::size_t i = 0; i < safety.safeSequence.size(); ++i) {
                        if (i) std::cout << " -> ";
                        std::cout << safety.safeSequence[i];
                    }
                    std::cout << "\n";
                } else {
                    std::cout << "Safe state: NO. No safe sequence exists for the current allocation.\n";
                }
            } else if (choice == 3) {
                std::string id = readText("Process ID: ");
                std::vector<int> request;
                for (const auto& name : manager.getResourceNames()) {
                    request.push_back(readInt("Request for " + name + ": ", 0, 1000000));
                }
                ResourceRequestResult result = manager.requestResources(id, request);
                std::cout << result.message << "\n";
                if (result.accepted) {
                    std::cout << "Safe sequence after grant: ";
                    for (std::size_t i = 0; i < result.safety.safeSequence.size(); ++i) {
                        if (i) std::cout << " -> ";
                        std::cout << result.safety.safeSequence[i];
                    }
                    std::cout << "\n";
                }
            }
        }
    } catch (const std::exception& error) {
        std::cout << "Resource module failed: " << error.what() << "\n";
    }
}

void ConsoleUI::exportLastResult() const {
    if (!lastResult_) {
        std::cout << "No scheduling result to export. Run an algorithm first.\n";
        return;
    }
    try {
        std::filesystem::path path = fileManager_.saveReport(*lastResult_);
        std::cout << "Report saved to: " << path.string() << "\n";
    } catch (const std::exception& error) {
        std::cout << "Export failed: " << error.what() << "\n";
    }
}

void ConsoleUI::run() {
    std::cout << "\nWelcome to KernelSim.\n";
    std::cout << "Choose how to provide the process workload before running a simulation.\n";
    std::cout << "  1. Enter my own processes\n";
    std::cout << "  2. Use the sample workload\n";
    std::cout << "  3. Load a workload file from data/\n";
    std::cout << "  0. Exit\n";
    int startupChoice = readInt("Choose input method: ", 0, 3);
    if (startupChoice == 0) {
        std::cout << "Exiting KernelSim.\n";
        return;
    }
    if (startupChoice == 1) createCustomWorkload();
    else if (startupChoice == 2) loadSampleWorkload();
    else loadWorkloadFromFile();

    while (true) {
        showMainMenu();
        int choice = readInt("Select an option: ", 0, 7);
        if (choice == 0) {
            std::cout << "Exiting KernelSim.\n";
            return;
        }
        switch (choice) {
            case 1: showWorkload(); break;
            case 2: runOneSimulation(); break;
            case 3: compareAlgorithms(); break;
            case 4: resourceMenu(); break;
            case 5: createCustomWorkload(); break;
            case 6: loadWorkloadFromFile(); break;
            case 7: exportLastResult(); break;
            default: break;
        }
    }
}
