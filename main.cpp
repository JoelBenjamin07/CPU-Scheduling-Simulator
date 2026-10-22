#include "ConsoleUI.h"
#include <filesystem>
#include <iostream>

namespace {
std::filesystem::path resolveProjectRoot(const char* executablePath) {
    std::filesystem::path current = std::filesystem::current_path();
    if (std::filesystem::exists(current / "data")) return current;
    if (std::filesystem::exists(current.parent_path() / "data")) return current.parent_path();

    std::filesystem::path executable = std::filesystem::absolute(executablePath).parent_path();
    if (std::filesystem::exists(executable / "data")) return executable;
    if (std::filesystem::exists(executable.parent_path() / "data")) return executable.parent_path();

    return current;
}
}

int main(int argc, char* argv[]) {
    try {
        std::filesystem::path root = resolveProjectRoot(argc > 0 ? argv[0] : "KernelSim");
        ConsoleUI application(root);
        application.run();
    } catch (const std::exception& error) {
        std::cerr << "KernelSim could not start: " << error.what() << "\n";
        return 1;
    }
    return 0;
}
