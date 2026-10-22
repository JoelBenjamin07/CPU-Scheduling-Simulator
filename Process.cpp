#include "Process.h"
#include <stdexcept>

Process::Process(const std::string& id, int arrivalTime, int burstTime, int priority)
    : id_(id),
      arrivalTime_(arrivalTime),
      burstTime_(burstTime),
      remainingTime_(burstTime),
      priority_(priority),
      firstStartTime_(-1),
      completionTime_(-1),
      state_(ProcessState::NEW) {
    if (id_.empty()) {
        throw std::invalid_argument("Process ID cannot be empty.");
    }
    if (arrivalTime_ < 0) {
        throw std::invalid_argument("Arrival time cannot be negative.");
    }
    if (burstTime_ <= 0) {
        throw std::invalid_argument("Burst time must be positive.");
    }
}

const std::string& Process::getId() const { return id_; }
int Process::getArrivalTime() const { return arrivalTime_; }
int Process::getBurstTime() const { return burstTime_; }
int Process::getRemainingTime() const { return remainingTime_; }
int Process::getPriority() const { return priority_; }
int Process::getFirstStartTime() const { return firstStartTime_; }
int Process::getCompletionTime() const { return completionTime_; }
ProcessState Process::getState() const { return state_; }

void Process::setRemainingTime(int value) {
    if (value < 0 || value > burstTime_) {
        throw std::invalid_argument("Remaining time is outside the valid range.");
    }
    remainingTime_ = value;
}

void Process::setState(ProcessState value) { state_ = value; }

void Process::setFirstStartTime(int value) {
    if (value < 0) {
        throw std::invalid_argument("First start time cannot be negative.");
    }
    firstStartTime_ = value;
}

void Process::setCompletionTime(int value) {
    if (value < 0) {
        throw std::invalid_argument("Completion time cannot be negative.");
    }
    completionTime_ = value;
}

void Process::reset() {
    remainingTime_ = burstTime_;
    firstStartTime_ = -1;
    completionTime_ = -1;
    state_ = ProcessState::NEW;
}
