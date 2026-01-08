#include "../../include/virtual_memory/vm.h"
#include <iostream>

VirtualMemory::VirtualMemory(size_t pSize, size_t physSize)
    : pageSize(pSize),
      pageFaults(0),
      hits(0)
{
    numFrames = physSize / pageSize;
}

size_t VirtualMemory::translate(size_t vAddr) {
    size_t page = vAddr / pageSize;
    size_t offset = vAddr % pageSize;

    if (pageTable.count(page)) {
        hits++;
        return pageTable[page] * pageSize + offset;
    }

    pageFaults++;

    if (pageTable.size() >= numFrames) {
        size_t victim = fifo.front();
        fifo.pop();
        pageTable.erase(victim);
    }

    size_t frame = pageTable.size();
    pageTable[page] = frame;
    fifo.push(page);

    return frame * pageSize + offset;
}

void VirtualMemory::stats() const {
    std::cout << "Page hits: " << hits << "\n";
    std::cout << "Page faults: " << pageFaults << "\n";
}
