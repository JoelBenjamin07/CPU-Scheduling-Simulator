#include "SchedulingPolicy.h"
#include <algorithm>
#include <stdexcept>

int SchedulingPolicy::timeQuantum() const { return 0; }

std::string FCFSPolicy::getName() const { return "FCFS"; }

int FCFSPolicy::selectProcess(const std::vector<int>& candidates, const std::vector<Process>&) const {
    return candidates.empty() ? -1 : candidates.front();
}

bool FCFSPolicy::isPreemptive() const { return false; }

std::string SJFPolicy::getName() const { return "SJF (Non-preemptive)"; }

int SJFPolicy::selectProcess(const std::vector<int>& candidates, const std::vector<Process>& processes) const {
    if (candidates.empty()) return -1;
    return *std::min_element(candidates.begin(), candidates.end(), [&](int a, int b) {
        const Process& left = processes.at(static_cast<std::size_t>(a));
        const Process& right = processes.at(static_cast<std::size_t>(b));
        if (left.getBurstTime() != right.getBurstTime()) return left.getBurstTime() < right.getBurstTime();
        if (left.getArrivalTime() != right.getArrivalTime()) return left.getArrivalTime() < right.getArrivalTime();
        return left.getId() < right.getId();
    });
}

bool SJFPolicy::isPreemptive() const { return false; }

std::string SRTFPolicy::getName() const { return "SRTF"; }

int SRTFPolicy::selectProcess(const std::vector<int>& candidates, const std::vector<Process>& processes) const {
    if (candidates.empty()) return -1;
    return *std::min_element(candidates.begin(), candidates.end(), [&](int a, int b) {
        const Process& left = processes.at(static_cast<std::size_t>(a));
        const Process& right = processes.at(static_cast<std::size_t>(b));
        if (left.getRemainingTime() != right.getRemainingTime()) return left.getRemainingTime() < right.getRemainingTime();
        if (left.getArrivalTime() != right.getArrivalTime()) return left.getArrivalTime() < right.getArrivalTime();
        return left.getId() < right.getId();
    });
}

bool SRTFPolicy::isPreemptive() const { return true; }

std::string PriorityPolicy::getName() const { return "Priority (Non-preemptive)"; }

int PriorityPolicy::selectProcess(const std::vector<int>& candidates, const std::vector<Process>& processes) const {
    if (candidates.empty()) return -1;
    return *std::min_element(candidates.begin(), candidates.end(), [&](int a, int b) {
        const Process& left = processes.at(static_cast<std::size_t>(a));
        const Process& right = processes.at(static_cast<std::size_t>(b));
        if (left.getPriority() != right.getPriority()) return left.getPriority() < right.getPriority();
        if (left.getArrivalTime() != right.getArrivalTime()) return left.getArrivalTime() < right.getArrivalTime();
        return left.getId() < right.getId();
    });
}

bool PriorityPolicy::isPreemptive() const { return false; }

RoundRobinPolicy::RoundRobinPolicy(int quantum) : quantum_(quantum) {
    if (quantum_ <= 0) {
        throw std::invalid_argument("Round Robin time quantum must be positive.");
    }
}

std::string RoundRobinPolicy::getName() const { return "Round Robin (q=" + std::to_string(quantum_) + ")"; }

int RoundRobinPolicy::selectProcess(const std::vector<int>& candidates, const std::vector<Process>&) const {
    return candidates.empty() ? -1 : candidates.front();
}

bool RoundRobinPolicy::isPreemptive() const { return true; }

int RoundRobinPolicy::timeQuantum() const { return quantum_; }
