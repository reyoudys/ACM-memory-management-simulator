#ifndef BUDDY_H
#define BUDDY_H

#include <cstddef>
#include <map>
#include <set>
#include <vector>

class BuddyAllocator {
public:
    struct Block {
        size_t addr;           // starting address
        int order;             // block order (2^order)
        size_t requestedSize;  // original request size
    };

private:
    size_t totalSize;
    int maxOrder;
    int nextId;

    // total internal fragmentation in bytes
    size_t internalFragmentation;

    // allocated blocks indexed by allocation ID
    std::map<int, Block> allocated;

    // free lists per order
    std::vector<std::set<size_t>> freeLists;

    // ---------- Helpers ----------
    int sizeToOrder(size_t size) const;
    size_t orderToSize(int order) const;
    size_t buddyOf(size_t addr, int order) const;

public:
    BuddyAllocator();

    // initialize allocator with power-of-two memory size
    void init(size_t size);

    // allocate block, assigns unique ID internally
    void mallocBlock(size_t size);

    // free block using allocation ID
    void freeBlock(int id);

    // dump free lists
    void dump() const;

    // ---------- Stats ----------
    size_t getInternalFragmentation() const;
};

#endif
