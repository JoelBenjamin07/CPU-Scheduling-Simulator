#pragma once
#include <string>
#include <vector>

struct SafetyResult {
    bool safe = false;
    std::vector<std::string> safeSequence;
};

struct ResourceRequestResult {
    bool accepted = false;
    std::string message;
    SafetyResult safety;
};

class ResourceManager {
private:
    std::vector<std::string> processIds_;
    std::vector<std::string> resourceNames_;
    std::vector<int> total_;
    std::vector<std::vector<int>> allocation_;
    std::vector<std::vector<int>> maximum_;

    std::vector<int> calculateAvailable(const std::vector<std::vector<int>>& allocation) const;
    std::vector<std::vector<int>> calculateNeed(const std::vector<std::vector<int>>& allocation) const;
    SafetyResult checkSafety(const std::vector<std::vector<int>>& allocation) const;
    int findProcess(const std::string& processId) const;

public:
    ResourceManager(
        const std::vector<std::string>& processIds,
        const std::vector<std::string>& resourceNames,
        const std::vector<int>& total,
        const std::vector<std::vector<int>>& allocation,
        const std::vector<std::vector<int>>& maximum
    );

    const std::vector<std::string>& getProcessIds() const;
    const std::vector<std::string>& getResourceNames() const;
    const std::vector<int>& getTotal() const;
    std::vector<int> getAvailable() const;
    std::vector<std::vector<int>> getAllocation() const;
    std::vector<std::vector<int>> getMaximum() const;
    std::vector<std::vector<int>> getNeed() const;

    SafetyResult evaluateSafety() const;
    ResourceRequestResult requestResources(const std::string& processId, const std::vector<int>& request);
};
