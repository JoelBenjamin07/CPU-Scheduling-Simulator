#pragma once
#include "Process.h"
#include <string>
#include <vector>

class SchedulingPolicy {
public:
    virtual ~SchedulingPolicy() = default;
    virtual std::string getName() const = 0;
    virtual int selectProcess(const std::vector<int>& candidates, const std::vector<Process>& processes) const = 0;
    virtual bool isPreemptive() const = 0;
    virtual int timeQuantum() const;
};

class FCFSPolicy : public SchedulingPolicy {
public:
    std::string getName() const override;
    int selectProcess(const std::vector<int>& candidates, const std::vector<Process>& processes) const override;
    bool isPreemptive() const override;
};

class SJFPolicy : public SchedulingPolicy {
public:
    std::string getName() const override;
    int selectProcess(const std::vector<int>& candidates, const std::vector<Process>& processes) const override;
    bool isPreemptive() const override;
};

class SRTFPolicy : public SchedulingPolicy {
public:
    std::string getName() const override;
    int selectProcess(const std::vector<int>& candidates, const std::vector<Process>& processes) const override;
    bool isPreemptive() const override;
};

class PriorityPolicy : public SchedulingPolicy {
public:
    std::string getName() const override;
    int selectProcess(const std::vector<int>& candidates, const std::vector<Process>& processes) const override;
    bool isPreemptive() const override;
};

class RoundRobinPolicy : public SchedulingPolicy {
private:
    int quantum_;

public:
    explicit RoundRobinPolicy(int quantum);
    std::string getName() const override;
    int selectProcess(const std::vector<int>& candidates, const std::vector<Process>& processes) const override;
    bool isPreemptive() const override;
    int timeQuantum() const override;
};
