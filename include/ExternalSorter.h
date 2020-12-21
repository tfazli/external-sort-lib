#ifndef EXTERNALSORTER_H
#define EXTERNALSORTER_H

#include "DataTypeWrapper.h"
#include "TempFileManager.h"
#include <string>
#include <fstream>
#include <vector>
#include <thread>
#include <iterator>
#include <algorithm>
#include <cmath>

namespace {
constexpr size_t kMegabyteMultiplier = 1024 * 1024;
constexpr size_t kConstantlyUsedRAM = 1 * kMegabyteMultiplier;
constexpr char kTempFileNamePrefix[] = "temp_";
}

template <typename T>
class ExternalSorter {
public:
    ExternalSorter(size_t availableRamMegabytes, size_t availableThreadsCount,
                   std::string inputFileName, std::string outputFileName);
    ~ExternalSorter() = default;
    ExternalSorter(const ExternalSorter &rhs) = delete;
    ExternalSorter(ExternalSorter &&rhs) = delete;
    ExternalSorter& operator=(const ExternalSorter &rhs) = delete;
    ExternalSorter& operator=(ExternalSorter &&rhs) = delete;

    void sort();

private:
    void multiFileMergeWrapper(std::fstream &sortedFile1, std::fstream &sortedFile2, std::fstream &outputFile);
    void sortWrapper(typename std::vector<T>::iterator begin, typename std::vector<T>::iterator end);
    void inRamMergeWrapper(typename std::vector<T>::iterator begin, typename std::vector<T>::iterator med,
                           typename std::vector<T>::iterator end, typename std::vector<T>::iterator outputBegin);
    void divideToPartsAndSort(std::vector<T> &inputBuffer,
                              std::vector<std::thread> &threadPool, size_t &itemsPerThread);
    void mergeParts(std::vector<T> &inputBuffer, std::vector<std::thread> &threadPool, size_t &itemsPerThread);
    void singleBlockMergeSort(std::vector<T> &inputBuffer);
    void inRamFileSegmentsSort(std::vector<std::fstream> &tempFilesPool, size_t recursionsToRead);
    void multiFileMergeSort(std::vector<std::fstream> &inputBufFiles);

private:
    const size_t mAvailableRam;
    const size_t mAvailableThreads;
    const std::string mOutputFileName;
    const size_t mMaxBufferSize;
    std::fstream mInputFile;
    size_t mFileSize;
    size_t mCreatedTempFilesCount;
    TempFileManager mTempFilesManager;
};

/* ################################################################################################## */
/*                                 EXTERNAL SORTER METHODS DEFINITION                                 */
/* ################################################################################################## */

template<typename T>
ExternalSorter<T>::ExternalSorter(size_t availableRamMegabytes, size_t availableThreadsCount,
                               std::string inputFileName, std::string outputFileName)
    : mAvailableRam(availableRamMegabytes * kMegabyteMultiplier - kConstantlyUsedRAM),
      mAvailableThreads(availableThreadsCount), mOutputFileName(outputFileName),
      mMaxBufferSize((availableRamMegabytes * kMegabyteMultiplier - kConstantlyUsedRAM) / sizeof(T) / 2),
      mInputFile(inputFileName.c_str(), std::ios::in | std::ios::binary | std::ios::ate),
      mFileSize(0), mCreatedTempFilesCount(0), mTempFilesManager(kTempFileNamePrefix) {
    mFileSize = static_cast<size_t>(mInputFile.tellg());
    mInputFile.seekg(0, std::ios::beg);
}

template<typename T>
void ExternalSorter<T>::sort() {
    if (!mAvailableRam) {
        return;
    }
    size_t recursionsToRead = mFileSize / (mMaxBufferSize * sizeof(T));
    if (mFileSize % (mMaxBufferSize * sizeof(T)) != 0) {
        ++recursionsToRead;
    }
    std::vector<std::fstream> tempFilesPool(recursionsToRead);
    inRamFileSegmentsSort(tempFilesPool, recursionsToRead);
    if (mCreatedTempFilesCount >= 2) {
        multiFileMergeSort(tempFilesPool);
    } else {
        std::string tempFileName {kTempFileNamePrefix};
        tempFileName += "1";
        if (mTempFilesManager.checkFileExist(mOutputFileName)) {
            std::remove(mOutputFileName.c_str());
            std::rename(tempFileName.c_str(), mOutputFileName.c_str());
        } else {
            std::rename(tempFileName.c_str(), mOutputFileName.c_str());
        }
    }
    mTempFilesManager.removeTempFiles(mCreatedTempFilesCount);
    mCreatedTempFilesCount = 0;
}

template<typename T>
void ExternalSorter<T>::multiFileMergeWrapper(std::fstream &sortedFile1,
                                              std::fstream &sortedFile2, std::fstream &outputFile) {
    std::merge(std::istream_iterator<DataTypeWrapper<T>>(sortedFile1),
               std::istream_iterator<DataTypeWrapper<T>>(),
               std::istream_iterator<DataTypeWrapper<T>>(sortedFile2),
               std::istream_iterator<DataTypeWrapper<T>>(),
               std::ostream_iterator<DataTypeWrapper<T>>(outputFile));
}

template<typename T>
void ExternalSorter<T>::sortWrapper(typename std::vector<T>::iterator begin, typename std::vector<T>::iterator end) {
    std::sort(begin, end);
}

template<typename T>
void ExternalSorter<T>::inRamMergeWrapper(typename std::vector<T>::iterator begin, typename std::vector<T>::iterator med,
                                          typename std::vector<T>::iterator end, typename std::vector<T>::iterator outputBegin) {
    std::merge(begin, med, med, end, outputBegin);
    std::swap_ranges(begin, end, outputBegin);
}

template<typename T>
void ExternalSorter<T>::divideToPartsAndSort(std::vector<T> &inputBuffer,
                                             std::vector<std::thread> &threadPool, size_t &itemsPerThread) {
    for (size_t i = 0; i < mAvailableThreads; ++i) {
        auto begin = inputBuffer.begin() + i * itemsPerThread;
        auto end = inputBuffer.begin() + (i + 1) * itemsPerThread;
        if (i == mAvailableThreads - 1) {
            end = inputBuffer.end();
        }
        threadPool.at(i) = std::thread(&ExternalSorter::sortWrapper, this, begin, end);
    }
    for (auto &thread : threadPool) {
        thread.join();
    }
}

template<typename T>
void ExternalSorter<T>::mergeParts(std::vector<T> &inputBuffer,
                                   std::vector<std::thread> &threadPool, size_t &itemsPerThread) {
    std::vector<T> outputBuffer(inputBuffer.size());
    auto currentTreadsCount = mAvailableThreads;
    size_t multiplier (1);
    for (size_t iteration = 1; iteration <= std::log2(mAvailableThreads); ++iteration) {
        currentTreadsCount /= 2;
        threadPool.resize(currentTreadsCount);
        for (size_t i = 0; i < currentTreadsCount ; ++i) {
            auto begin = inputBuffer.begin() + multiplier * i * 2 * itemsPerThread;
            auto med = inputBuffer.begin() + multiplier * (i * 2 + 1) * itemsPerThread;
            auto end = inputBuffer.begin() + multiplier * (i * 2 + 2) * itemsPerThread;
            auto outputBegin = outputBuffer.begin() + iteration * i * 2 * itemsPerThread;
            if (i == currentTreadsCount - 1) {
                end = inputBuffer.end();
            }
            threadPool.at(i) = std::thread(&ExternalSorter::inRamMergeWrapper, this,
                                           begin, med, end, outputBegin);
        }
        for (auto &thread : threadPool) {
            thread.join();
        }
        multiplier *= 2;
    }
}

template<typename T>
void ExternalSorter<T>::singleBlockMergeSort(std::vector<T> &inputBuffer) {
    std::vector<std::thread> threadPool(mAvailableThreads);
    size_t itemsPerThread = inputBuffer.size() / mAvailableThreads;
    divideToPartsAndSort(inputBuffer, threadPool, itemsPerThread);
    mergeParts(inputBuffer, threadPool, itemsPerThread);
}

template<typename T>
void ExternalSorter<T>::inRamFileSegmentsSort(std::vector<std::fstream> &tempFilesPool, size_t recursionsToRead) {
    std::vector<T> inputBuffer(mMaxBufferSize);
    for (size_t iteration = 1; iteration <= recursionsToRead; ++iteration) {
        mInputFile.read(reinterpret_cast<char*>(inputBuffer.data()), mMaxBufferSize * sizeof(T));
        auto readedCount = mInputFile.gcount();
        if (static_cast<size_t>(readedCount) < mMaxBufferSize * sizeof(T)) {
            inputBuffer.resize(readedCount / sizeof(T));
            mInputFile.clear();
        }
        singleBlockMergeSort(inputBuffer);
        std::string tempFileName {kTempFileNamePrefix};
        tempFileName += std::to_string(iteration);
        std::fstream tempFile {tempFileName.c_str(), std::ios::trunc | std::ios::in | std::ios::out | std::ios::binary};
        tempFile.write(reinterpret_cast<char*>(inputBuffer.data()), inputBuffer.size() * sizeof(T));
        mTempFilesManager.recoverInitialFileState(tempFile);
        tempFilesPool.at(iteration - 1) = std::move(tempFile);
        ++mCreatedTempFilesCount;
    }
}

template<typename T>
void ExternalSorter<T>::multiFileMergeSort(std::vector<std::fstream> &inputBufFiles) {
    std::vector<std::thread> threadPool;
    std::vector<std::fstream> outputBufFiles;
    for (size_t i = 1; i <= inputBufFiles.size() / 2; ++i) {
        std::string bufFileName {kTempFileNamePrefix};
        bufFileName += std::to_string(inputBufFiles.size() + i);
        std::fstream bufFile {bufFileName.c_str(), std::ios::trunc | std::ios::in | std::ios::out | std::ios::binary};
        outputBufFiles.emplace_back(std::move(bufFile));
        ++mCreatedTempFilesCount;
    }

    bool oddElementsCountFlag;
    size_t iterationsPassed (0);
    while(inputBufFiles.size() > 2) {
        inputBufFiles.size() % 2 == 0 ? oddElementsCountFlag = false : oddElementsCountFlag = true;
        size_t pairsLeft = inputBufFiles.size() / 2;
        while (pairsLeft != 0) {
            size_t threadsToCreate = std::min(mAvailableThreads, pairsLeft);
            threadPool.reserve(threadsToCreate);
            for (size_t i = 0; i < threadsToCreate; ++i) {
                threadPool.emplace_back(std::thread(&ExternalSorter::multiFileMergeWrapper, this,
                                        std::ref(inputBufFiles.at((iterationsPassed * mAvailableThreads + i) * 2)),
                                        std::ref(inputBufFiles.at((iterationsPassed * mAvailableThreads + i) * 2 + 1)),
                                        std::ref(outputBufFiles.at(iterationsPassed * mAvailableThreads + i))));
            }
            for (auto &thread : threadPool) {
                if (thread.joinable()) {
                    thread.join();
                }
            }
            threadPool.clear();
            pairsLeft -= threadsToCreate;
            ++iterationsPassed;
        }
        for (auto &file : inputBufFiles) {
            mTempFilesManager.recoverInitialFileState(file);
        }
        for (auto &file : outputBufFiles) {
            mTempFilesManager.recoverInitialFileState(file);
        }
        if (oddElementsCountFlag) {
            outputBufFiles.emplace_back(std::move(inputBufFiles.back()));
        }
        std::swap(inputBufFiles, outputBufFiles);
        outputBufFiles.resize(inputBufFiles.size() / 2);
        iterationsPassed = 0;
    }
    std::fstream outputFile {mOutputFileName, std::ios::trunc | std::ios::in | std::ios::out | std::ios::binary};
    multiFileMergeWrapper(inputBufFiles.at(0), inputBufFiles.at(1), outputFile);
}

#endif // EXTERNALSORTER_H
