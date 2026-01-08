# Memory Management Simulator

## Design and Implementation

---

## 1. Memory Layout and Assumptions

### Memory Model

* Physical memory is simulated as a **contiguous array of bytes**.
* All addresses are **byte-addressable** and displayed in **hexadecimal format**.
* The simulator runs entirely in **user space** and does not manage real system memory.
* Memory accesses and execution timing are **symbolic** and intended for correctness modeling only.

### Assumptions

* Single-process execution model.
* Memory size is **fixed at initialization time**.
* No alignment constraints beyond byte granularity.
* No hardware interaction, interrupts, or concurrency.

---

## 2. Contiguous Memory Allocation Strategies

Dynamic memory allocation is implemented using a **linked-list–based free list**.

### Block Representation

Each memory block contains:

* Start address
* Block size (bytes)
* Allocation status (free / used)
* Unique block ID
* Pointer to the next block in memory order

### Supported Allocation Strategies

#### First Fit

* Traverses the free list in address order.
* Allocates the **first block** large enough for the request.
* Fast allocation, but may cause early fragmentation.

#### Best Fit

* Scans all free blocks.
* Allocates the **smallest suitable block**.
* Reduces immediate waste but can increase long-term fragmentation.

#### Worst Fit

* Scans all free blocks.
* Allocates the **largest available block**.
* Attempts to preserve medium-sized blocks.

### Block Management

* Blocks are **split** when larger than required.
* **Adjacent free blocks are coalesced** during deallocation to reduce external fragmentation.

### Metrics Reported

* Total memory
* Used memory
* Free memory
* External fragmentation
* Memory utilization
* Allocation success and failure counts

---

## 3. Buddy Memory Allocation System

An optional **Buddy Allocator** is implemented as an alternative allocation strategy.

### Design Characteristics

* Total memory size **must be a power of two**.
* Allocation requests are **rounded up to the nearest power of two**.
* Free blocks are maintained in **order-indexed free lists**, where order *k* corresponds to blocks of size `2^k` bytes.

### Buddy Computation

* Buddy addresses are computed using:

```text
buddy = address XOR block_size
```

* Allocation uses **recursive splitting**.
* Deallocation uses **recursive merging** when both buddies are free.

### Fragmentation Tracking

* **Internal fragmentation** is explicitly tracked:

```text
internal_fragmentation += allocated_block_size − requested_size
```

* Fragmentation is reduced when blocks are freed.

### Buddy Allocator Properties

* Associativity: **Not applicable**
* Replacement policy: **None**
* Focus: deterministic merging and fast allocation/deallocation

---

## 4. Multilevel Cache Simulation

A **two-level CPU cache hierarchy** is implemented.

### Cache Levels

#### L1 Cache

* Sets: **8**
* Associativity: **2-way set associative**
* Block size: **32 bytes**
* Replacement policy: **LRU**

#### L2 Cache

* Sets: **16**
* Associativity: **4-way set associative**
* Block size: **32 bytes**
* Replacement policy: **FIFO**

### Cache Structure

* Each cache is divided into sets.
* Each set contains multiple cache lines.
* Each cache line stores:

  * Valid bit
  * Tag
  * Age/timestamp (used for FIFO and LRU)

### Access Semantics

* Cache accesses use **physical addresses only**.
* On an L1 miss, the request is propagated to L2.
* L2 misses represent symbolic main memory access.

### Metrics Tracked (per level)

* Total accesses
* Cache hits
* Cache misses
* Evictions
* Hit rate

---

## 5. Virtual Memory Status

> Virtual memory and paging were part of the original design but are **not implemented** in the current version.

* No page tables
* No virtual-to-physical translation
* No page faults or page replacement

All operations work directly with **physical addresses**.

---

## 6. System Access Flow (Implemented)

The enforced access order is:

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

---

## Summary

This simulator provides a faithful software model of:

* Contiguous dynamic memory allocation
* Buddy system allocation with internal fragmentation tracking
* Multilevel cache hierarchy with realistic replacement policies

---

Just tell me.
