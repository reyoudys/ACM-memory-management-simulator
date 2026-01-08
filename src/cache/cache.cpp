#include "../../include/cache/cache.h"
#include <iostream>

// ================= Constructor =================
Cache::Cache(size_t numSets,
             size_t associativity,
             size_t blockSize,
             const std::string& policy,
             const std::string& name)
    : name(name),
      numSets(numSets),
      associativity(associativity),
      blockSize(blockSize),
      policy(policy),
      sets(numSets, std::vector<CacheLine>(associativity)),
      timer(0),
      accesses(0),
      hits(0),
      misses(0),
      evictions(0)
{
    for (auto& set : sets) {
        for (auto& line : set) {
            line.valid = false;
            line.tag = 0;
            line.age = 0;
        }
    }
}

// ================= Helpers =================
size_t Cache::getSetIndex(size_t addr) const {
    return (addr / blockSize) % numSets;
}

size_t Cache::getTag(size_t addr) const {
    return (addr / blockSize) / numSets;
}

// Select victim based on replacement policy (FIFO or LRU)
int Cache::selectVictim(size_t setIndex) {
    int victim = 0;
    size_t bestAge = sets[setIndex][0].age;

    for (size_t i = 1; i < associativity; i++) {
        if (sets[setIndex][i].age < bestAge) {
            bestAge = sets[setIndex][i].age;
            victim = static_cast<int>(i);
        }
    }
    return victim;
}

// ================= Access =================
bool Cache::access(size_t addr) {
    accesses++;
    timer++;

    size_t setIdx = getSetIndex(addr);
    size_t tag = getTag(addr);

    // ---------- HIT ----------
    for (auto& line : sets[setIdx]) {
        if (line.valid && line.tag == tag) {
            hits++;
            if (policy == "LRU") {
                line.age = timer;
            }
            return true;
        }
    }

    // ---------- MISS ----------
    misses++;

    // ---------- EMPTY SLOT ----------
    for (auto& line : sets[setIdx]) {
        if (!line.valid) {
            line.valid = true;
            line.tag = tag;
            line.age = timer;
            return false;
        }
    }

    // ---------- EVICTION ----------
    int victim = selectVictim(setIdx);
    evictions++;

    sets[setIdx][victim].valid = true;
    sets[setIdx][victim].tag = tag;
    sets[setIdx][victim].age = timer;

    return false;
}

// ================= Reset =================
void Cache::reset() {
    accesses = 0;
    hits = 0;
    misses = 0;
    evictions = 0;
    timer = 0;

    for (auto& set : sets) {
        for (auto& line : set) {
            line.valid = false;
            line.tag = 0;
            line.age = 0;
        }
    }
}

// ================= Stats =================
void Cache::stats() const {
    std::cout << "Cache Stats";
    if (!name.empty())
        std::cout << " (" << name << ")";
    std::cout << "\n";

    std::cout << "Accesses : " << accesses << "\n";
    std::cout << "Hits     : " << hits << "\n";
    std::cout << "Misses   : " << misses << "\n";
    std::cout << "Evictions: " << evictions << "\n";

    double hitRate = (accesses == 0)
        ? 0.0
        : (double)hits / accesses * 100.0;

    std::cout << "Hit Rate : " << hitRate << "%\n";
}

// ================= Getters =================
size_t Cache::getAccesses() const {
    return accesses;
}

size_t Cache::getHits() const {
    return hits;
}

size_t Cache::getMisses() const {
    return misses;
}

size_t Cache::getEvictions() const {
    return evictions;
}
