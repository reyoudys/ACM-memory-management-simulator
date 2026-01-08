#include "../include/memory.h"
#include "../include/cache/cache.h"
#include "../include/buddy/buddy.h"

#include <iostream>
#include <sstream>
#include <string>

// ---------- helper ----------
bool isPowerOfTwo(size_t x) {
    return x && !(x & (x - 1));
}

int main() {
    Memory mem;
    BuddyAllocator buddy;

    bool useBuddy = false;

    // L1 Cache: 8 sets, 2-way, block size 32 bytes, LRU
    Cache l1(8, 2, 32, "LRU", "L1");

    // L2 Cache: 16 sets, 4-way, block size 32 bytes, FIFO
    Cache l2(16, 4, 32, "FIFO", "L2");

    std::string line;

    while (true) {
        std::cout << "> ";
        std::getline(std::cin, line);

        std::stringstream ss(line);
        std::string cmd;
        ss >> cmd;

        // ---------- INIT ----------
        if (cmd == "init") {
            std::string type;
            size_t size;
            ss >> type >> size;

            if (type == "memory") {
                mem.init(size);
                useBuddy = false;
                l1.reset();
                l2.reset();
            }
            else if (type == "buddy") {
                if (!isPowerOfTwo(size)) {
                    std::cout
                        << "Error: Buddy allocator requires size to be power of two\n";
                    continue;
                }
                buddy.init(size);
                useBuddy = true;
                l1.reset();
                l2.reset();
            }
            else {
                std::cout
                    << "Usage: init memory <size> | init buddy <size>\n";
            }
        }

        // ---------- SET ----------
        else if (cmd == "set") {
            std::string sub, type;
            ss >> sub >> type;

            if (!useBuddy && sub == "allocator") {
                if (type == "first_fit")
                    mem.setAllocator(AllocatorType::FIRST_FIT);
                else if (type == "best_fit")
                    mem.setAllocator(AllocatorType::BEST_FIT);
                else if (type == "worst_fit")
                    mem.setAllocator(AllocatorType::WORST_FIT);
                else
                    std::cout << "Unknown allocator\n";
            }
            else if (useBuddy) {
                std::cout << "Allocator setting ignored in Buddy mode\n";
            }
            else {
                std::cout
                    << "Usage: set allocator <first_fit|best_fit|worst_fit>\n";
            }
        }

        // ---------- MALLOC ----------
        else if (cmd == "malloc") {
            size_t size;
            ss >> size;

            if (useBuddy) {
                buddy.mallocBlock(size);
            }
            else {
                size_t addr = mem.mallocBlock(size);
                if (addr != static_cast<size_t>(-1)) {

                    // ---------- Cache hierarchy ----------
                    if (!l1.access(addr)) {
                        if (!l2.access(addr)) {
                            // Miss in both → bring from memory
                            // (already counted as misses)
                        }
                        // Fill L1 after L2 access
                        l1.access(addr);
                    }
                }
            }
        }

        // ---------- FREE ----------
        else if (cmd == "free") {
            int id;
            ss >> id;

            if (useBuddy) {
                buddy.freeBlock(id);
            }
            else {
                mem.freeBlock(id);
                // No cache access on free
            }
        }

        // ---------- DUMP ----------
        else if (cmd == "dump") {
            if (useBuddy)
                buddy.dump();
            else
                mem.dump();
        }

        // ---------- STATS ----------
        else if (cmd == "stats") {
            if (useBuddy) {
                std::cout << "Buddy Internal Fragmentation: "
                          << buddy.getInternalFragmentation()
                          << " bytes\n";
            }
            else {
                mem.stats();
            }
        }

        // ---------- CACHE ----------
        else if (cmd == "cache") {
            std::cout << "\n=== Cache Hierarchy ===\n";
            l1.stats();
            std::cout << "\n";
            l2.stats();
        }

        // ---------- EXIT ----------
        else if (cmd == "exit") {
            break;
        }

        // ---------- INVALID ----------
        else {
            std::cout << "Invalid command\n";
        }
    }

    return 0;
}
