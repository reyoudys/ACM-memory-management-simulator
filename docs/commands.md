# Memory Simulator – Command Reference

This document lists all supported commands in the Memory Management Simulator, grouped by subsystem.

---

## 1. Global Commands

| Command | Description                                                       |
| ------- | ----------------------------------------------------------------- |
| `exit`  | Terminates the simulator                                          |
| `dump`  | Dumps current memory layout (physical or buddy depending on mode) |
| `stats` | Displays statistics for current memory mode                       |
| `cache` | Displays cache hierarchy statistics (L1 and L2)                   |

---

## 2. Initialization Commands

### Physical Memory Mode

```
init memory <size>
```

**Description**

* Initializes contiguous physical memory of `<size>` bytes
* Resets allocator state
* Resets cache hierarchy (L1 + L2)
* Switches simulator to **Physical Memory mode**

**Example**

```
init memory 4096
```

---

### Buddy Allocator Mode

```
init buddy <size>
```

**Description**

* Initializes Buddy Allocator with total memory `<size>`
* `<size>` **must be a power of two**
* Resets buddy free lists
* Resets cache hierarchy (not used in buddy mode)
* Switches simulator to **Buddy mode**

**Example**

```
init buddy 2048
```

---

## 3. Allocation Strategy Commands (Physical Memory Only)

```
set allocator <strategy>
```

### Supported Strategies

| Strategy    | Description                      |
| ----------- | -------------------------------- |
| `first_fit` | First free block large enough    |
| `best_fit`  | Smallest free block large enough |
| `worst_fit` | Largest free block               |

**Notes**

* Ignored in Buddy mode
* Only valid after `init memory`

**Example**

```
set allocator best_fit
```

---

## 4. Allocation & Deallocation Commands

### Allocate Memory

```
malloc <size>
```

**Physical Memory Mode**

* Allocates `<size>` bytes using current allocation strategy
* Returns:

  * Block ID
  * Starting physical address
* Triggers cache hierarchy access (L1 → L2 → Memory)

**Buddy Mode**

* Allocates nearest power-of-two block
* Tracks internal fragmentation
* Returns block ID and allocated size

**Example**

```
malloc 64
```

---

### Free Memory

```
free <block_id>
```

**Description**

* Frees previously allocated block by ID
* Coalesces adjacent free blocks (physical memory)
* Recursively merges buddies (buddy allocator)
* **Does not access cache**

**Example**

```
free 3
```

---

## 5. Memory Inspection Commands

### Dump Memory State

```
dump
```

**Physical Memory Mode**

* Prints address ranges
* Indicates FREE / USED blocks
* Displays block IDs

**Buddy Mode**

* Prints free blocks grouped by order (block size)

**Example**

```
dump
```

---

### Memory Statistics

```
stats
```

**Physical Memory Mode**

* Total memory
* Used memory
* Free memory
* External fragmentation
* Memory utilization
* Allocation success / failure counts

**Buddy Mode**

* Internal fragmentation (bytes wasted due to power-of-two rounding)

**Example**

```
stats
```

---

## 6. Cache System Commands

### Cache Statistics

```
cache
```

**Displays**

* Per-level cache statistics:

  * Accesses
  * Hits
  * Misses
  * Evictions
  * Hit rate

**Cache Architecture**

* L1: 8 sets, 2-way, 32-byte blocks, LRU
* L2: 16 sets, 4-way, 32-byte blocks, FIFO

**Example**

```
cache
```

---

## 7. Mode-Specific Behavior Summary

| Feature                | Physical Memory | Buddy Allocator |
| ---------------------- | --------------- | --------------- |
| `set allocator`        | ✅ Enabled       | ❌ Ignored       |
| Cache Access           | ✅ Yes           | ❌ No            |
| External Fragmentation | ✅ Yes           | ❌ No            |
| Internal Fragmentation | ❌ No            | ✅ Yes           |
| Block Coalescing       | Adjacent blocks | Buddy merging   |

---

## 8. Error Handling

| Condition                   | Behavior                    |
| --------------------------- | --------------------------- |
| Invalid command             | Prints `Invalid command`    |
| Invalid block ID            | Prints error message        |
| Buddy init non-power-of-two | Initialization rejected     |
| Allocation failure          | Allocation fails gracefully |

---

## 9. Example Session

```
init memory 4096
set allocator first_fit
malloc 64
malloc 128
dump
cache
stats
free 1
dump
exit
```

---

