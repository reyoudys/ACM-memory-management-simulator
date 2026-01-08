#include "../../include/buddy/buddy.h"
#include <iostream>
#include <cmath>
#include <algorithm>

// ---------- Constructor ----------
BuddyAllocator::BuddyAllocator()
    : totalSize(0),
      maxOrder(0),
      nextId(1),
      internalFragmentation(0) {}

// ---------- Init ----------
void BuddyAllocator::init(size_t size) {
    totalSize = size;
    nextId = 1;
    internalFragmentation = 0;

    maxOrder = static_cast<int>(std::log2(size));

    freeLists.clear();
    freeLists.resize(maxOrder + 1);
    allocated.clear();

    freeLists[maxOrder].insert(0);

    std::cout << "Buddy memory initialized: " << size << " bytes\n";
}

// ---------- Helpers ----------
int BuddyAllocator::sizeToOrder(size_t size) const {
    int order = 0;
    size_t blockSize = 1;
    while (blockSize < size) {
        blockSize <<= 1;
        order++;
    }
    return order;
}

size_t BuddyAllocator::orderToSize(int order) const {
    return static_cast<size_t>(1) << order;
}

size_t BuddyAllocator::buddyOf(size_t addr, int order) const {
    return addr ^ orderToSize(order);
}

// ---------- Malloc ----------
void BuddyAllocator::mallocBlock(size_t size) {
    int order = sizeToOrder(size);

    for (int i = order; i <= maxOrder; i++) {
        if (!freeLists[i].empty()) {
            size_t addr = *freeLists[i].begin();
            freeLists[i].erase(addr);

            // split blocks
            while (i > order) {
                i--;
                size_t buddy = addr + orderToSize(i);
                freeLists[i].insert(buddy);
            }

            size_t allocatedSize = orderToSize(order);
            internalFragmentation += (allocatedSize - size);

            allocated[nextId] = {
                addr,
                order,
                size
            };

            std::cout << "Allocated block id=" << nextId
                      << " at address=0x"
                      << std::hex << addr
                      << std::dec
                      << " size=" << allocatedSize
                      << "\n";

            nextId++;
            return;
        }
    }

    std::cout << "Allocation failed\n";
}

// ---------- Free ----------
void BuddyAllocator::freeBlock(int id) {
    auto it = allocated.find(id);
    if (it == allocated.end()) {
        std::cout << "Invalid block id\n";
        return;
    }

    Block blk = it->second;
    allocated.erase(it);

    size_t addr = blk.addr;
    int order = blk.order;

    size_t allocatedSize = orderToSize(order);
    internalFragmentation -= (allocatedSize - blk.requestedSize);

    // merge buddies
    while (order < maxOrder) {
        size_t buddy = buddyOf(addr, order);
        auto freeIt = freeLists[order].find(buddy);

        if (freeIt == freeLists[order].end())
            break;

        freeLists[order].erase(freeIt);
        addr = std::min(addr, buddy);
        order++;
    }

    freeLists[order].insert(addr);

    std::cout << "Block " << id << " freed and merged\n";
}

// ---------- Dump ----------
void BuddyAllocator::dump() const {
    for (int i = 0; i <= maxOrder; i++) {
        for (size_t addr : freeLists[i]) {
            std::cout << "[0x"
                      << std::hex << addr
                      << " - 0x"
                      << (addr + orderToSize(i) - 1)
                      << "] FREE\n";
        }
    }
}

// ---------- Stats ----------
size_t BuddyAllocator::getInternalFragmentation() const {
    return internalFragmentation;
}
