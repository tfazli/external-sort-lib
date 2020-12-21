#include "TempFileManager.h"

TempFileManager::TempFileManager(std::string tempFileNamePrefix)
    : mTempFileNamePrefix(tempFileNamePrefix) {
}

bool TempFileManager::checkFileExist(const std::string &filename) const {
    std::ifstream file(filename.c_str());
    return file.good();
}

void TempFileManager::removeTempFiles(size_t maxPostfixCounter) const {
    for (size_t i = 1; i <= maxPostfixCounter; ++i) {
        std::string fileName {mTempFileNamePrefix};
        fileName += std::to_string(i);
        if (checkFileExist(fileName)) {
            std::remove(fileName.c_str());
        }
    }
}

void TempFileManager::recoverInitialFileState(std::fstream &file) const {
    if (!file.good()) {
        file.clear();
    }
    file.seekg(0, std::ios::beg);
    file.seekp(0, std::ios::beg);
}
