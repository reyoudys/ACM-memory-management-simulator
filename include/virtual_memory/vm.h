#ifndef VM_H
#define VM_H

#include <cstddef>
#include <unordered_map>
#include <queue>

class VirtualMemory {
    size_t pageSize;
    size_t numFrames;

    std::unordered_map<size_t, size_t> pageTable;
    std::queue<size_t> fifo;

    size_t pageFaults;
    size_t hits;

public:
    VirtualMemory(size_t pageSize, size_t physMemSize);

    size_t translate(size_t virtualAddr);
    void stats() const;
};

#endif
