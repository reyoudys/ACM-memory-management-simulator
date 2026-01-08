#include "../include/memory.h"
#include <iostream>
#include <limits>

Memory::Memory() {
    head = nullptr;
    totalMemory = 0;
    nextId = 1;
    allocator = AllocatorType::FIRST_FIT;
    allocSuccess = 0;
    allocFail = 0;
}

void Memory::init(size_t size) {
    // reset old list if re-initialized
    head = new Block{0, size, true, -1, nullptr};

    totalMemory = size;
    nextId = 1;
    allocSuccess = 0;
    allocFail = 0;

    std::cout << "Memory initialized: " << size << " bytes\n";
}

void Memory::setAllocator(AllocatorType type) {
    allocator = type;
    std::cout << "Allocator set\n";
}

Block* Memory::findBlock(size_t size) {
    Block* curr = head;
    Block* best = nullptr;

    if (allocator == AllocatorType::FIRST_FIT) {
        while (curr) {
            if (curr->free && curr->size >= size)
                return curr;
            curr = curr->next;
        }
    }

    if (allocator == AllocatorType::BEST_FIT) {
        size_t diff = std::numeric_limits<size_t>::max();
        while (curr) {
            if (curr->free && curr->size >= size &&
                curr->size - size < diff) {
                diff = curr->size - size;
                best = curr;
            }
            curr = curr->next;
        }
        return best;
    }

    if (allocator == AllocatorType::WORST_FIT) {
        size_t maxSize = 0;
        while (curr) {
            if (curr->free && curr->size >= size &&
                curr->size > maxSize) {
                maxSize = curr->size;
                best = curr;
            }
            curr = curr->next;
        }
        return best;
    }

    return nullptr;
}

void Memory::splitBlock(Block* block, size_t size) {
    if (block->size == size)
        return;

    Block* newBlock = new Block{
        block->start + size,
        block->size - size,
        true,
        -1,
        block->next
    };

    block->size = size;
    block->next = newBlock;
}

size_t Memory::mallocBlock(size_t size) {
    Block* block = findBlock(size);
    if (!block) {
        allocFail++;
        std::cout << "Allocation failed\n";
        return static_cast<size_t>(-1);
    }

    splitBlock(block, size);

    block->free = false;
    block->id = nextId++;
    allocSuccess++;

    std::cout << "Allocated block id=" << block->id
              << " at address=0x"
              << std::hex << block->start << std::dec << "\n";

    return block->start;
}

size_t Memory::freeBlock(int id) {
    Block* curr = head;

    while (curr) {
        if (!curr->free && curr->id == id) {
            curr->free = true;
            curr->id = -1;

            size_t addr = curr->start;
            coalesce();

            std::cout << "Block " << id << " freed and merged\n";
            return addr;
        }
        curr = curr->next;
    }

    std::cout << "Invalid block id\n";
    return static_cast<size_t>(-1);
}

void Memory::coalesce() {
    Block* curr = head;

    while (curr && curr->next) {
        if (curr->free && curr->next->free) {
            curr->size += curr->next->size;
            curr->next = curr->next->next;
        } else {
            curr = curr->next;
        }
    }
}

void Memory::dump() {
    Block* curr = head;

    while (curr) {
        size_t end = curr->start + curr->size - 1;

        if (curr->free) {
            std::cout << "[0x" << std::hex << curr->start
                      << " - 0x" << end << "] FREE\n";
        } else {
            std::cout << "[0x" << std::hex << curr->start
                      << " - 0x" << end << "] USED (id="
                      << std::dec << curr->id << ")\n";
        }

        curr = curr->next;
    }
}

void Memory::stats() {
    size_t used = 0;
    size_t free = 0;
    size_t largestFree = 0;

    Block* curr = head;

    while (curr) {
        if (curr->free) {
            free += curr->size;
            if (curr->size > largestFree)
                largestFree = curr->size;
        } else {
            used += curr->size;
        }
        curr = curr->next;
    }

    double extFrag = free == 0
        ? 0.0
        : (1.0 - (double)largestFree / free) * 100.0;

    std::cout << "Total memory: " << totalMemory << "\n";
    std::cout << "Used memory: " << used << "\n";
    std::cout << "Free memory: " << free << "\n";
    std::cout << "External fragmentation: " << extFrag << "%\n";
    std::cout << "Memory utilization: "
              << (double)used / totalMemory * 100.0 << "%\n";
    std::cout << "Allocation success: " << allocSuccess << "\n";
    std::cout << "Allocation failure: " << allocFail << "\n";
}
