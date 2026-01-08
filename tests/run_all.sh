#!/bin/bash

echo "=== Memory Basic ==="
./memsim < tests/memory_basic.txt

echo "=== Fragmentation ==="
./memsim < tests/memory_fragmentation.txt

echo "=== Fit Strategies ==="
./memsim < tests/fit_strategies.txt

echo "=== Buddy Allocator ==="
./memsim < tests/buddy_basic.txt

echo "=== Cache Sequential ==="
./memsim < tests/cache_sequential.txt

echo "=== Cache Conflict ==="
./memsim < tests/cache_conflict.txt

echo "=== Mixed Workload ==="
./memsim < tests/memory_cache_mix.txt
