#include "FileManager.h"
#include <fstream>
#include <iomanip>
#include <set>
#include <sstream>
#include <stdexcept>

FileManager::FileManager(const std::filesystem::path& root) : root_(root) {
    std::filesystem::create_directories(root_ / "data");
    std::filesystem::create_directories(root_ / "results");
}

const std::filesystem::path& FileManager::getRoot() const { return root_; }

std::vector<Process> FileManager::loadWorkload(const std::string& filename) const {
    std::ifstream input(root_ / "data" / filename);
    if (!input) throw std::runtime_error("Could not open workload file: " + filename);

    int count = 0;
    if (!(input >> count) || count <= 0 || count > 1000) {
        throw std::runtime_error("Workload must begin with a process count from 1 to 1000.");
    }

    std::vector<Process> processes;
    std::set<std::string> ids;
    processes.reserve(static_cast<std::size_t>(count));

    for (int i = 0; i < count; ++i) {
        std::string id;
        int arrival = 0;
        int burst = 0;
        int priority = 0;
        if (!(input >> id >> arrival >> burst >> priority)) {
            throw std::runtime_error("Malformed workload record at process row " + std::to_string(i + 1) + ".");
        }
        if (!ids.insert(id).second) {
            throw std::runtime_error("Duplicate process ID in workload: " + id);
        }
        processes.emplace_back(id, arrival, burst, priority);
    }

    std::string extra;
    if (input >> extra) throw std::runtime_error("Unexpected extra data at the end of workload file.");
    return processes;
}


std::filesystem::path FileManager::saveWorkload(const std::vector<Process>& processes, const std::string& filename) const {
    if (processes.empty()) throw std::invalid_argument("Cannot save an empty workload.");
    std::filesystem::create_directories(root_ / "data");
    std::filesystem::path path = root_ / "data" / filename;
    std::ofstream output(path);
    if (!output) throw std::runtime_error("Could not create workload file.");

    output << processes.size() << "\n";
    for (const auto& process : processes) {
        output << process.getId() << " "
               << process.getArrivalTime() << " "
               << process.getBurstTime() << " "
               << process.getPriority() << "\n";
    }
    if (!output) throw std::runtime_error("Failed while writing workload file.");
    return path;
}

ResourceManager FileManager::loadResources(const std::string& filename) const {
    std::ifstream input(root_ / "data" / filename);
    if (!input) throw std::runtime_error("Could not open resource file: " + filename);

    int processCount = 0;
    int resourceCount = 0;
    if (!(input >> processCount >> resourceCount) ||
        processCount <= 0 || processCount > 1000 ||
        resourceCount <= 0 || resourceCount > 100) {
        throw std::runtime_error("Resource file must begin with valid process and resource counts.");
    }

    std::vector<std::string> resourceNames(static_cast<std::size_t>(resourceCount));
    for (auto& name : resourceNames) {
        if (!(input >> name)) throw std::runtime_error("Missing resource name.");
    }

    std::vector<int> total(static_cast<std::size_t>(resourceCount));
    for (int& value : total) {
        if (!(input >> value)) throw std::runtime_error("Missing total resource count.");
    }

    std::vector<std::string> processIds(static_cast<std::size_t>(processCount));
    std::vector<std::vector<int>> allocation(static_cast<std::size_t>(processCount),
                                              std::vector<int>(static_cast<std::size_t>(resourceCount)));
    std::vector<std::vector<int>> maximum(static_cast<std::size_t>(processCount),
                                           std::vector<int>(static_cast<std::size_t>(resourceCount)));

    for (int i = 0; i < processCount; ++i) {
        if (!(input >> processIds[static_cast<std::size_t>(i)])) {
            throw std::runtime_error("Missing process ID in resource data.");
        }
        for (int j = 0; j < resourceCount; ++j) {
            if (!(input >> allocation[static_cast<std::size_t>(i)][static_cast<std::size_t>(j)])) {
                throw std::runtime_error("Missing allocation value.");
            }
        }
        for (int j = 0; j < resourceCount; ++j) {
            if (!(input >> maximum[static_cast<std::size_t>(i)][static_cast<std::size_t>(j)])) {
                throw std::runtime_error("Missing maximum-claim value.");
            }
        }
    }

    std::string extra;
    if (input >> extra) throw std::runtime_error("Unexpected extra data at the end of resource file.");

    return ResourceManager(processIds, resourceNames, total, allocation, maximum);
}

std::filesystem::path FileManager::saveReport(const SimulationResult& result) const {
    std::filesystem::create_directories(root_ / "results");
    std::filesystem::path path = root_ / "results" / "last_report.txt";
    std::ofstream output(path);
    if (!output) throw std::runtime_error("Could not create report file.");

    output << "KERNELSIM SCHEDULING REPORT\n";
    output << "Algorithm: " << result.algorithmName << "\n\n";
    output << "Gantt chart\n";
    for (const auto& segment : result.timeline) {
        output << "|" << segment.processId << " " << segment.startTime << "-" << segment.endTime << " ";
    }
    output << "|\n\n";

    output << std::left << std::setw(10) << "PID"
           << std::setw(7) << "AT"
           << std::setw(7) << "BT"
           << std::setw(7) << "CT"
           << std::setw(8) << "TAT"
           << std::setw(8) << "WT"
           << std::setw(8) << "RT" << "\n";

    for (const auto& process : result.processes) {
        output << std::left << std::setw(10) << process.processId
               << std::setw(7) << process.arrivalTime
               << std::setw(7) << process.burstTime
               << std::setw(7) << process.completionTime
               << std::setw(8) << process.turnaroundTime
               << std::setw(8) << process.waitingTime
               << std::setw(8) << process.responseTime << "\n";
    }

    output << std::fixed << std::setprecision(2);
    output << "\nAverage waiting time: " << result.metrics.averageWaitingTime << "\n";
    output << "Average turnaround time: " << result.metrics.averageTurnaroundTime << "\n";
    output << "Average response time: " << result.metrics.averageResponseTime << "\n";
    output << "CPU utilization (%): " << result.metrics.cpuUtilization << "\n";
    output << "Throughput (processes per time unit): " << result.metrics.throughput << "\n";
    output << "Busy time: " << result.metrics.busyTime << "\n";
    output << "Total elapsed time: " << result.metrics.totalTime << "\n";

    if (!output) throw std::runtime_error("Failed while writing report.");
    return path;
}
