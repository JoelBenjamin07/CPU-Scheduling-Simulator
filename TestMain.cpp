#include "../src/Process.h"
#include "../src/SchedulingEngine.h"
#include "../src/SchedulingPolicy.h"
#include "../src/ResourceManager.h"
#include <cmath>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
void require(bool condition, const std::string& message) {
    if (!condition) throw std::runtime_error(message);
}

void requireNear(double actual, double expected, const std::string& message) {
    if (std::fabs(actual - expected) > 0.0001) {
        throw std::runtime_error(message + " expected " + std::to_string(expected) +
                                 " but got " + std::to_string(actual));
    }
}

std::vector<Process> fcfsWorkload() {
    return {
        Process("P1", 0, 5, 2),
        Process("P2", 1, 3, 1),
        Process("P3", 2, 1, 3),
        Process("P4", 4, 2, 2)
    };
}

void testFCFS() {
    SchedulingEngine engine;
    FCFSPolicy policy;
    SimulationResult result = engine.run(fcfsWorkload(), policy);
    require(result.processes.size() == 4, "FCFS process count");
    require(result.processes[0].completionTime == 5, "FCFS P1 completion");
    require(result.processes[1].completionTime == 8, "FCFS P2 completion");
    require(result.processes[2].completionTime == 9, "FCFS P3 completion");
    require(result.processes[3].completionTime == 11, "FCFS P4 completion");
    requireNear(result.metrics.averageWaitingTime, 3.75, "FCFS average waiting time");
    requireNear(result.metrics.averageTurnaroundTime, 6.5, "FCFS average turnaround time");
    requireNear(result.metrics.cpuUtilization, 100.0, "FCFS CPU utilization");
}

void testSJF() {
    SchedulingEngine engine;
    SJFPolicy policy;
    SimulationResult result = engine.run(fcfsWorkload(), policy);
    require(result.processes[0].completionTime == 5, "SJF P1 completion");
    require(result.processes[2].completionTime == 6, "SJF P3 completion");
    require(result.processes[1].completionTime == 11, "SJF P2 completion");
    require(result.processes[3].completionTime == 8, "SJF P4 completion");
}

void testSRTF() {
    SchedulingEngine engine;
    SRTFPolicy policy;
    std::vector<Process> workload = {
        Process("P1", 0, 8, 2),
        Process("P2", 1, 4, 1),
        Process("P3", 2, 2, 3)
    };
    SimulationResult result = engine.run(workload, policy);
    require(result.processes[0].completionTime == 14, "SRTF P1 completion");
    require(result.processes[1].completionTime == 7, "SRTF P2 completion");
    require(result.processes[2].completionTime == 4, "SRTF P3 completion");
    require(result.timeline.size() == 5, "SRTF should record five execution segments");
    require(result.timeline[0].processId == "P1" && result.timeline[1].processId == "P2" &&
            result.timeline[2].processId == "P3" && result.timeline[3].processId == "P2" &&
            result.timeline[4].processId == "P1", "SRTF preemption order");
    requireNear(result.metrics.averageWaitingTime, 8.0 / 3.0, "SRTF average waiting time");
}

void testPriority() {
    SchedulingEngine engine;
    PriorityPolicy policy;
    std::vector<Process> workload = {
        Process("P1", 0, 4, 3),
        Process("P2", 0, 2, 1),
        Process("P3", 0, 1, 2)
    };
    SimulationResult result = engine.run(workload, policy);
    require(result.processes[1].completionTime == 2, "Priority high-priority process completes first");
    require(result.processes[2].completionTime == 3, "Priority second process");
    require(result.processes[0].completionTime == 7, "Priority last process");
}

void testRoundRobin() {
    SchedulingEngine engine;
    RoundRobinPolicy policy(2);
    std::vector<Process> workload = {
        Process("P1", 0, 5, 1),
        Process("P2", 0, 3, 2)
    };
    SimulationResult result = engine.run(workload, policy);
    require(result.processes[0].completionTime == 8, "RR P1 completion");
    require(result.processes[1].completionTime == 7, "RR P2 completion");
    requireNear(result.processes[0].waitingTime, 3.0, "RR P1 waiting time");
    requireNear(result.processes[1].waitingTime, 4.0, "RR P2 waiting time");
}

void testIdleTime() {
    SchedulingEngine engine;
    FCFSPolicy policy;
    SimulationResult result = engine.run({Process("P1", 3, 2, 1)}, policy);
    require(result.timeline.front().processId == "IDLE", "Idle interval is recorded");
    require(result.processes[0].completionTime == 5, "Idle-time completion");
    requireNear(result.metrics.cpuUtilization, 40.0, "Idle-time CPU utilization");
    requireNear(result.metrics.throughput, 0.2, "Idle-time throughput");
}

ResourceManager makeBankerExample() {
    return ResourceManager(
        {"P0", "P1", "P2", "P3", "P4"},
        {"A", "B", "C"},
        {10, 5, 7},
        {{0, 1, 0}, {2, 0, 0}, {3, 0, 2}, {2, 1, 1}, {0, 0, 2}},
        {{7, 5, 3}, {3, 2, 2}, {9, 0, 2}, {2, 2, 2}, {4, 3, 3}}
    );
}

void testBankerSafeState() {
    ResourceManager manager = makeBankerExample();
    SafetyResult result = manager.evaluateSafety();
    require(result.safe, "Banker's example should be safe");
    require(result.safeSequence.size() == 5, "Safe sequence includes all processes");
    require(result.safeSequence.front() == "P1", "Expected deterministic safe sequence begins P1");
}

void testBankerRejectsUnsafeRequestWithoutMutation() {
    ResourceManager manager = makeBankerExample();
    std::vector<int> before = manager.getAvailable();
    ResourceRequestResult result = manager.requestResources("P0", {0, 0, 2});
    require(!result.accepted, "Unsafe request must be rejected");
    require(manager.getAvailable() == before, "Rejected request must not mutate available resources");
}

void testBankerAcceptsSafeRequest() {
    ResourceManager manager = makeBankerExample();
    ResourceRequestResult result = manager.requestResources("P1", {1, 0, 2});
    require(result.accepted, "Safe request should be accepted");
    require(manager.evaluateSafety().safe, "State after accepted request remains safe");
}

void testProcessValidation() {
    bool rejected = false;
    try {
        Process invalid("BAD", 0, 0, 1);
    } catch (const std::invalid_argument&) {
        rejected = true;
    }
    require(rejected, "Zero burst time must be rejected");
}

void testResourceRequestExceedsNeed() {
    ResourceManager manager = makeBankerExample();
    std::vector<int> before = manager.getAvailable();
    ResourceRequestResult result = manager.requestResources("P0", {8, 0, 0});
    require(!result.accepted, "Request exceeding remaining need must be rejected");
    require(manager.getAvailable() == before, "Need violation must not mutate resources");
}

void testAllAlgorithmsMetricsInvariants() {
    SchedulingEngine engine;
    std::vector<Process> workload = fcfsWorkload();
    std::vector<std::unique_ptr<SchedulingPolicy>> policies;
    policies.push_back(std::make_unique<FCFSPolicy>());
    policies.push_back(std::make_unique<SJFPolicy>());
    policies.push_back(std::make_unique<SRTFPolicy>());
    policies.push_back(std::make_unique<PriorityPolicy>());
    policies.push_back(std::make_unique<RoundRobinPolicy>(2));
    for (const auto& policy : policies) {
        SimulationResult result = engine.run(workload, *policy);
        int totalDuration = 0;
        for (const auto& segment : result.timeline) {
            require(segment.endTime > segment.startTime, "Timeline segments must have positive duration");
            totalDuration += segment.endTime - segment.startTime;
        }
        require(totalDuration == result.metrics.totalTime, "Timeline duration must equal elapsed time");
        require(result.metrics.busyTime == 11, "Total busy time equals sum of burst times");
        for (const auto& process : result.processes) {
            require(process.waitingTime >= 0, "Waiting time must not be negative");
            require(process.turnaroundTime == process.completionTime - process.arrivalTime,
                    "Turnaround time formula");
            require(process.waitingTime == process.turnaroundTime - process.burstTime,
                    "Waiting time formula");
            require(process.responseTime >= 0, "Response time must not be negative");
        }
    }
}

void testInvalidRequest() {
    ResourceManager manager = makeBankerExample();
    ResourceRequestResult result = manager.requestResources("P0", {99, 0, 0});
    require(!result.accepted, "Request beyond available/need must be rejected");
}

}

int main() {
    try {
        testFCFS();
        testSJF();
        testSRTF();
        testPriority();
        testRoundRobin();
        testIdleTime();
        testBankerSafeState();
        testBankerRejectsUnsafeRequestWithoutMutation();
        testBankerAcceptsSafeRequest();
        testInvalidRequest();
        testProcessValidation();
        testResourceRequestExceedsNeed();
        testAllAlgorithmsMetricsInvariants();
        std::cout << "All 13 KernelSim tests passed.\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "TEST FAILURE: " << error.what() << "\n";
        return 1;
    }
}
