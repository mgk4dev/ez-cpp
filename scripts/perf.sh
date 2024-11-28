#!/usr/bin/env bash

echo "++++ perf $@"
echo $1

perf stat -e \
cache-references,\
cache-misses,\
L1-dcache-prefetches,\
instructions,\
cpu-cycles,\
branches,\
branch-misses,\
duration_time $@
