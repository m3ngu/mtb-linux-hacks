#!/bin/bash

DELAY=5
DEMO_ARGS="125000 3"

echo "Launching vmstat and running test script in normal mode with args" $DEMO_ARGS
./set_cache_normal
vmstat -S K $DELAY > vmstat.tmp &
time ./swap-central $DEMO_ARGS
sleep $DELAY
killall vmstat
echo "Pageins   Pageouts"
perl -lane 'printf "%7d   %8d\n", @F[6,7] if $. > 2' vmstat.tmp
rm vmstat.tmp

echo Switching to MRU
./set_cache_mru
echo "Launching vmstat and re-running test script"
vmstat -S K $DELAY > vmstat.tmp &
time ./swap-central $DEMO_ARGS
sleep $DELAY
killall vmstat
echo "Pageins   Pageouts"
perl -lane 'printf "%7d   %8d\n", @F[6,7] if $. > 2' vmstat.tmp
rm vmstat.tmp
