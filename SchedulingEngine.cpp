#include "SchedulingEngine.h"
#include <algorithm>
#include <stdexcept>

namespace {
void appendSegment(std::vector<GanttSegment>& timeline, const std::string& id, int start, int end) {
    if (!timeline.empty() && timeline.back().processId == id && timeline.back().endTime == start) {
        timeline.back().endTime = end;
    } else {
        timeline.push_back({id, start, end});
    }
}

void eraseCandidate(std::vector<int>& values, int index) {
    auto it = std::find(values.begin(), values.end(), index);
    if (it != values.end()) values.erase(it);
}
}

SimulationResult SchedulingEngine::run(const std::vector<Process>& input, const SchedulingPolicy& policy) const {
    if (input.empty()) {
        throw std::invalid_argument("At least one process is required.");
    }

    std::vector<Process> processes = input;
    for (auto& process : processes) process.reset();

    std::vector<int> arrivalOrder(processes.size());
    for (std::size_t i = 0; i < processes.size(); ++i) arrivalOrder[i] = static_cast<int>(i);
    std::stable_sort(arrivalOrder.begin(), arrivalOrder.end(), [&](int a, int b) {
        if (processes[a].getArrivalTime() != processes[b].getArrivalTime()) {
            return processes[a].getArrivalTime() < processes[b].getArrivalTime();
        }
        return processes[a].getId() < processes[b].getId();
    });

    std::vector<int> ready;
    std::vector<bool> admitted(processes.size(), false);
    std::vector<GanttSegment> timeline;
    int current = -1;
    int quantumElapsed = 0;
    int time = 0;
    int completed = 0;
    int busyTime = 0;
    const int quantum = policy.timeQuantum();

    while (completed < static_cast<int>(processes.size())) {
        for (int index : arrivalOrder) {
            if (!admitted[static_cast<std::size_t>(index)] &&
                processes[static_cast<std::size_t>(index)].getArrivalTime() <= time) {
                admitted[static_cast<std::size_t>(index)] = true;
                processes[static_cast<std::size_t>(index)].setState(ProcessState::READY);
                ready.push_back(index);
            }
        }

        if (current != -1 && quantum > 0 && quantumElapsed >= quantum) {
            processes[static_cast<std::size_t>(current)].setState(ProcessState::READY);
            ready.push_back(current);
            current = -1;
            quantumElapsed = 0;
        }

        if (current != -1 && policy.isPreemptive() && quantum == 0) {
            std::vector<int> candidates = ready;
            candidates.push_back(current);
            int selected = policy.selectProcess(candidates, processes);
            if (selected != current) {
                processes[static_cast<std::size_t>(current)].setState(ProcessState::READY);
                ready.push_back(current);
                eraseCandidate(ready, selected);
                current = selected;
                quantumElapsed = 0;
            }
        }

        if (current == -1 && !ready.empty()) {
            current = policy.selectProcess(ready, processes);
            eraseCandidate(ready, current);
            quantumElapsed = 0;
        }

        if (current == -1) {
            appendSegment(timeline, "IDLE", time, time + 1);
            ++time;
            continue;
        }

        Process& running = processes[static_cast<std::size_t>(current)];
        running.setState(ProcessState::RUNNING);
        if (running.getFirstStartTime() == -1) running.setFirstStartTime(time);

        appendSegment(timeline, running.getId(), time, time + 1);
        running.setRemainingTime(running.getRemainingTime() - 1);
        ++busyTime;
        ++time;
        ++quantumElapsed;

        if (running.getRemainingTime() == 0) {
            running.setCompletionTime(time);
            running.setState(ProcessState::COMPLETED);
            current = -1;
            quantumElapsed = 0;
            ++completed;
        }
    }

    SimulationResult result;
    result.algorithmName = policy.getName();
    result.timeline = timeline;
    result.processes.reserve(processes.size());

    double totalWaiting = 0.0;
    double totalTurnaround = 0.0;
    double totalResponse = 0.0;

    for (const Process& process : processes) {
        if (process.getCompletionTime() < 0 || process.getFirstStartTime() < 0) {
            throw std::logic_error("Simulation ended with an incomplete process.");
        }

        int turnaround = process.getCompletionTime() - process.getArrivalTime();
        int waiting = turnaround - process.getBurstTime();
        int response = process.getFirstStartTime() - process.getArrivalTime();

        result.processes.push_back({
            process.getId(),
            process.getArrivalTime(),
            process.getBurstTime(),
            process.getPriority(),
            process.getFirstStartTime(),
            process.getCompletionTime(),
            turnaround,
            waiting,
            response
        });

        totalWaiting += waiting;
        totalTurnaround += turnaround;
        totalResponse += response;
    }

    const double count = static_cast<double>(processes.size());
    result.metrics.averageWaitingTime = totalWaiting / count;
    result.metrics.averageTurnaroundTime = totalTurnaround / count;
    result.metrics.averageResponseTime = totalResponse / count;
    result.metrics.busyTime = busyTime;
    result.metrics.totalTime = time;
    result.metrics.cpuUtilization = time == 0 ? 0.0 : 100.0 * static_cast<double>(busyTime) / time;
    result.metrics.throughput = time == 0 ? 0.0 : count / time;

    return result;
}
