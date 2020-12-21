#include "ExternalSorter.h"
#include <thread>

namespace {
constexpr size_t kAvailableRamMegabytes = 128;
constexpr char kInputFileName[] = "input";
constexpr char kOutputFileName[] = "output";
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[]) {
    std::ifstream file(kInputFileName);
    if (!file.good()) {
        return 1;
    }
    file.close();
    size_t maxSupportedThreads = std::thread::hardware_concurrency();
    ExternalSorter<uint32_t> externalSorter(kAvailableRamMegabytes, maxSupportedThreads,
                                                 kInputFileName, kOutputFileName);
    externalSorter.sort();
    return 0;
}
