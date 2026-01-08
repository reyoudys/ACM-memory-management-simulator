#ifndef MEMORY_H
#define MEMORY_H

#include <cstddef>

enum class AllocatorType {
    FIRST_FIT,
    BEST_FIT,
    WORST_FIT
};

struct Block {
    size_t start;
    size_t size;
    bool free;
    int id;
    Block* next;
};

class Memory {
private:
    Block* head;
    size_t totalMemory;
    int nextId;
    AllocatorType allocator;

    Block* findBlock(size_t size);
    void splitBlock(Block* block, size_t size);
    void coalesce();

    int allocSuccess;
    int allocFail;

public:
    Memory();

    void init(size_t size);
    void setAllocator(AllocatorType type);

    // IMPORTANT: return address for cache access
    size_t mallocBlock(size_t size);
    size_t freeBlock(int id);

    void dump();
    void stats();
};

#endif
