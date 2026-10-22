#pragma once
#include "Process.h"
#include "SchedulingPolicy.h"
#include <string>
#include <vector>

struct GanttSegment {
    std::string processId;
    int startTime;
    int endTime;
};

struct ProcessResult {
    std::string processId;
    int arrivalTime;
    int burstTime;
    int priority;
    int firstStartTime;
    int completionTime;
    int turnaroundTime;
    int waitingTime;
    int responseTime;
};

struct SimulationMetrics {
    double averageWaitingTime = 0.0;
    double averageTurnaroundTime = 0.0;
    double averageResponseTime = 0.0;
    double cpuUtilization = 0.0;
    double throughput = 0.0;
    int busyTime = 0;
    int totalTime = 0;
};

struct SimulationResult {
    std::string algorithmName;
    std::vector<GanttSegment> timeline;
    std::vector<ProcessResult> processes;
    SimulationMetrics metrics;
};

class SchedulingEngine {
public:
    SimulationResult run(const std::vector<Process>& input, const SchedulingPolicy& policy) const;
};
