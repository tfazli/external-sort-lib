#ifndef TEMPFILEMANAGER_H
#define TEMPFILEMANAGER_H

#include <string>
#include <fstream>

class TempFileManager {
public:
    explicit TempFileManager(std::string tempFileNamePrefix);
    [[nodiscard]] bool checkFileExist (const std::string &filename) const;
    void removeTempFiles(size_t maxPostfixCounter) const;
    void recoverInitialFileState(std::fstream &file) const;

private:
    std::string mTempFileNamePrefix;
};

#endif // TEMPFILEMANAGER_H
