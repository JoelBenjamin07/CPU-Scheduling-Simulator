#pragma once
#include <string>

enum class ProcessState {
    NEW,
    READY,
    RUNNING,
    COMPLETED
};

class Process {
private:
    std::string id_;
    int arrivalTime_;
    int burstTime_;
    int remainingTime_;
    int priority_;
    int firstStartTime_;
    int completionTime_;
    ProcessState state_;

public:
    Process(const std::string& id, int arrivalTime, int burstTime, int priority);

    const std::string& getId() const;
    int getArrivalTime() const;
    int getBurstTime() const;
    int getRemainingTime() const;
    int getPriority() const;
    int getFirstStartTime() const;
    int getCompletionTime() const;
    ProcessState getState() const;

    void setRemainingTime(int value);
    void setState(ProcessState value);
    void setFirstStartTime(int value);
    void setCompletionTime(int value);
    void reset();
};
