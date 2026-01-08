#ifndef CACHE_H
#define CACHE_H

#include <vector>
#include <string>
#include <cstddef>
#include <iostream>

// ================= Cache Line =================
struct CacheLine {
    bool valid;
    size_t tag;
    size_t age;   // timestamp for LRU / FIFO
};

// ================= Cache =================
class Cache {
    // ---------- Identity ----------
    std::string name;

    // ---------- Configuration ----------
    size_t numSets;
    size_t associativity;
    size_t blockSize;
    std::string policy;   // "LRU" or "FIFO"

    // ---------- Storage ----------
    std::vector<std::vector<CacheLine>> sets;

    // ---------- Global timer ----------
    size_t timer;

    // ---------- Statistics ----------
    size_t accesses;
    size_t hits;
    size_t misses;
    size_t evictions;

    // ---------- Helpers ----------
    size_t getSetIndex(size_t addr) const;
    size_t getTag(size_t addr) const;
    int selectVictim(size_t setIndex);

public:
    // ---------- Constructor ----------
    Cache(size_t numSets,
          size_t associativity,
          size_t blockSize,
          const std::string& policy,
          const std::string& name = "");

    // ---------- Core operation ----------
    // returns true on HIT, false on MISS
    bool access(size_t addr);

    // ---------- Control ----------
    void reset();

    // ---------- Reporting ----------
    void stats() const;

    // ---------- Getters (for hierarchy / GUI / grading) ----------
    size_t getAccesses() const;
    size_t getHits() const;
    size_t getMisses() const;
    size_t getEvictions() const;
};

#endif
