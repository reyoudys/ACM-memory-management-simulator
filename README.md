# Design Document

## Memory Management Simulator

---

## 1. Memory Layout and Assumptions

### Memory Layout

* Physical memory is simulated as a **contiguous region of bytes**.
* Addresses are **byte-addressable** and displayed in **hexadecimal**.
* Memory is initialized once using the `init` command and remains fixed in size.
* The simulator does **not** interact with real hardware memory.

### Assumptions

* Single-process execution model.
* No concurrency or interrupts.
* No alignment constraints beyond byte granularity.
* All memory accesses are **symbolic**, not timed.
* Cache and memory behavior are modeled for **correctness**, not performance accuracy.

---

## 2. Allocation Strategy Implementations

The simulator supports **dynamic memory allocation** using a linked-list–based free list.

### Block Representation

Each memory block stores:

* Start address
* Block size (bytes)
* Allocation status (free / used)
* Unique block ID
* Pointer to the next block

Blocks are maintained in **address order**.

---

### Supported Allocation Strategies

#### First Fit

* Traverses the free list from the beginning.
* Allocates the first block large enough.
* Fast but can cause early fragmentation.

#### Best Fit

* Scans all free blocks.
* Chooses the smallest block that fits.
* Reduces immediate waste but may increase fragmentation.

#### Worst Fit

* Scans all free blocks.
* Chooses the largest free block.
* Attempts to preserve medium-sized blocks.

---

### Block Management

* **Splitting**: Free blocks larger than requested size are split.
* **Coalescing**: Adjacent free blocks are merged on deallocation.

### Metrics Tracked

* Used memory
* Free memory
* External fragmentation
* Memory utilization
* Allocation successes and failures

---

## 3. Buddy System Design

An optional **Buddy Allocator** is implemented as an alternative to the free-list allocator.

### Design Characteristics

* Total memory size **must be a power of two**.
* Allocation sizes are **rounded up to the nearest power of two**.
* Free memory is managed using **order-indexed free lists**.

```text
Order k → Block size = 2^k bytes
```

### Buddy Computation

Buddy addresses are computed using:

```text
buddy = address XOR block_size
```

### Allocation Flow

1. Find the smallest free block that can satisfy the request.
2. Recursively split larger blocks.
3. Allocate a block of the required order.

### Deallocation Flow

1. Free the block.
2. Check if its buddy is free.
3. Merge recursively until no further merge is possible.

### Fragmentation Tracking

* **Internal fragmentation** is explicitly tracked:

```text
internal_fragmentation += allocated_block_size − requested_size
```

* Fragmentation is reduced when blocks are freed.

### Buddy System Properties

* Associativity: Not applicable
* Replacement policy: None
* Focus: fast allocation and deterministic merging

---

## 4. Cache Hierarchy and Replacement Policy

A **two-level CPU cache hierarchy** is implemented.

---

### L1 Cache

* Sets: **8**
* Associativity: **2-way set associative**
* Block size: **32 bytes**
* Replacement policy: **LRU (Least Recently Used)**

### L2 Cache

* Sets: **16**
* Associativity: **4-way set associative**
* Block size: **32 bytes**
* Replacement policy: **FIFO (First-In First-Out)**

---

### Cache Structure

Each cache is organized as:

```text
Cache
 ├── Set 0 → CacheLine[0..N]
 ├── Set 1 → CacheLine[0..N]
 └── ...
```

Each cache line contains:

* Valid bit
* Tag
* Age counter (used for LRU/FIFO)

---

### Cache Access Behavior

* Cache accesses use **physical addresses only**.
* On an **L1 miss**, the request propagates to L2.
* L2 misses represent symbolic main memory access.

### Metrics Tracked (Per Level)

* Accesses
* Hits
* Misses
* Evictions
* Hit rate

---

## 5. Virtual Memory Model

**Not implemented in the current version.**

* No virtual addresses
* No page tables
* No page faults
* No page replacement

All operations occur directly on **physical addresses**.

---

## 6. Address Translation and Access Flow

### Implemented Access Flow

```text
Memory Allocation / Free
        ↓
Physical Address
        ↓
L1 Cache
        ↓
L2 Cache
        ↓
Main Memory (symbolic)
```

* Cache hierarchy operates strictly on physical addresses.
* Memory allocation and caching are independent modules.

---

## 7. Limitations and Simplifications

* No virtual memory or paging.
* No timing or latency modeling.
* No write-back or write-through cache policies.
* No multithreading or synchronization.
* Cache coherence is not modeled.
* No real hardware interaction.

---

## Conclusion

This simulator models key operating system and computer architecture concepts:

* Dynamic memory allocation
* Buddy system allocation
* Multilevel cache hierarchies
* Replacement policies (LRU, FIFO)
* Fragmentation analysis

The design prioritizes **clarity, correctness, and educational value** over hardware-level accuracy.

---
