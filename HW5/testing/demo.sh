#!/bin/bash

DEMO_ARGS="125000 3"

echo "Running test script in normal mode with args" $DEMO_ARGS
./set_cache_normal
time ./swap-central $DEMO_ARGS

echo Switching to MRU
./set_cache_mru
echo "Re-running test script"
time ./swap-central $DEMO_ARGS
