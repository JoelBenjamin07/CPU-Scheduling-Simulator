#include "ResourceManager.h"
#include <algorithm>
#include <stdexcept>

namespace {
bool lessThanOrEqual(const std::vector<int>& left, const std::vector<int>& right) {
    if (left.size() != right.size()) return false;
    for (std::size_t i = 0; i < left.size(); ++i) {
        if (left[i] > right[i]) return false;
    }
    return true;
}
}

ResourceManager::ResourceManager(
    const std::vector<std::string>& processIds,
    const std::vector<std::string>& resourceNames,
    const std::vector<int>& total,
    const std::vector<std::vector<int>>& allocation,
    const std::vector<std::vector<int>>& maximum
) : processIds_(processIds),
    resourceNames_(resourceNames),
    total_(total),
    allocation_(allocation),
    maximum_(maximum) {
    if (processIds_.empty() || resourceNames_.empty()) {
        throw std::invalid_argument("At least one process and one resource type are required.");
    }
    if (total_.size() != resourceNames_.size() ||
        allocation_.size() != processIds_.size() ||
        maximum_.size() != processIds_.size()) {
        throw std::invalid_argument("Resource data dimensions do not match.");
    }

    for (std::size_t j = 0; j < total_.size(); ++j) {
        if (total_[j] < 0) throw std::invalid_argument("Total resources cannot be negative.");
    }

    for (std::size_t i = 0; i < processIds_.size(); ++i) {
        if (processIds_[i].empty() || allocation_[i].size() != total_.size() ||
            maximum_[i].size() != total_.size()) {
            throw std::invalid_argument("Invalid process ID or resource matrix dimensions.");
        }
        for (std::size_t j = 0; j < total_.size(); ++j) {
            if (allocation_[i][j] < 0 || maximum_[i][j] < 0 ||
                allocation_[i][j] > maximum_[i][j]) {
                throw std::invalid_argument("Allocation and maximum claims are inconsistent.");
            }
        }
    }

    std::vector<int> available = calculateAvailable(allocation_);
    for (int value : available) {
        if (value < 0) throw std::invalid_argument("Allocated resources exceed total resources.");
    }

    for (std::size_t i = 0; i < processIds_.size(); ++i) {
        for (std::size_t j = i + 1; j < processIds_.size(); ++j) {
            if (processIds_[i] == processIds_[j]) {
                throw std::invalid_argument("Process IDs must be unique.");
            }
        }
    }

    for (std::size_t i = 0; i < resourceNames_.size(); ++i) {
        for (std::size_t j = i + 1; j < resourceNames_.size(); ++j) {
            if (resourceNames_[i] == resourceNames_[j]) {
                throw std::invalid_argument("Resource names must be unique.");
            }
        }
    }
}

const std::vector<std::string>& ResourceManager::getProcessIds() const { return processIds_; }
const std::vector<std::string>& ResourceManager::getResourceNames() const { return resourceNames_; }
const std::vector<int>& ResourceManager::getTotal() const { return total_; }
std::vector<int> ResourceManager::getAvailable() const { return calculateAvailable(allocation_); }
std::vector<std::vector<int>> ResourceManager::getAllocation() const { return allocation_; }
std::vector<std::vector<int>> ResourceManager::getMaximum() const { return maximum_; }
std::vector<std::vector<int>> ResourceManager::getNeed() const { return calculateNeed(allocation_); }

std::vector<int> ResourceManager::calculateAvailable(const std::vector<std::vector<int>>& allocation) const {
    std::vector<int> available = total_;
    for (const auto& row : allocation) {
        for (std::size_t j = 0; j < available.size(); ++j) available[j] -= row[j];
    }
    return available;
}

std::vector<std::vector<int>> ResourceManager::calculateNeed(const std::vector<std::vector<int>>& allocation) const {
    std::vector<std::vector<int>> need = maximum_;
    for (std::size_t i = 0; i < need.size(); ++i) {
        for (std::size_t j = 0; j < need[i].size(); ++j) need[i][j] -= allocation[i][j];
    }
    return need;
}

int ResourceManager::findProcess(const std::string& processId) const {
    for (std::size_t i = 0; i < processIds_.size(); ++i) {
        if (processIds_[i] == processId) return static_cast<int>(i);
    }
    return -1;
}

SafetyResult ResourceManager::checkSafety(const std::vector<std::vector<int>>& allocation) const {
    SafetyResult result;
    std::vector<int> work = calculateAvailable(allocation);
    std::vector<std::vector<int>> need = calculateNeed(allocation);
    std::vector<bool> finished(processIds_.size(), false);

    bool progress = true;
    while (progress) {
        progress = false;
        for (std::size_t i = 0; i < processIds_.size(); ++i) {
            if (!finished[i] && lessThanOrEqual(need[i], work)) {
                for (std::size_t j = 0; j < work.size(); ++j) work[j] += allocation[i][j];
                finished[i] = true;
                result.safeSequence.push_back(processIds_[i]);
                progress = true;
            }
        }
    }

    result.safe = result.safeSequence.size() == processIds_.size();
    if (!result.safe) result.safeSequence.clear();
    return result;
}

SafetyResult ResourceManager::evaluateSafety() const {
    return checkSafety(allocation_);
}

ResourceRequestResult ResourceManager::requestResources(const std::string& processId, const std::vector<int>& request) {
    ResourceRequestResult result;
    int index = findProcess(processId);
    if (index < 0) {
        result.message = "Unknown process ID.";
        return result;
    }
    if (request.size() != total_.size()) {
        result.message = "Request vector has the wrong number of resource values.";
        return result;
    }
    for (int value : request) {
        if (value < 0) {
            result.message = "Resource requests cannot be negative.";
            return result;
        }
    }

    std::vector<std::vector<int>> need = calculateNeed(allocation_);
    std::vector<int> available = calculateAvailable(allocation_);
    if (!lessThanOrEqual(request, need[static_cast<std::size_t>(index)])) {
        result.message = "Request exceeds the process's declared remaining need.";
        return result;
    }
    if (!lessThanOrEqual(request, available)) {
        result.message = "Request exceeds currently available resources.";
        return result;
    }

    std::vector<std::vector<int>> candidate = allocation_;
    for (std::size_t j = 0; j < request.size(); ++j) {
        candidate[static_cast<std::size_t>(index)][j] += request[j];
    }

    SafetyResult candidateSafety = checkSafety(candidate);
    if (!candidateSafety.safe) {
        result.message = "Request rejected: granting it would leave the system in an unsafe state. Allocation unchanged.";
        result.safety = candidateSafety;
        return result;
    }

    allocation_ = candidate;
    result.accepted = true;
    result.message = "Request granted. The resulting state is safe.";
    result.safety = candidateSafety;
    return result;
}
