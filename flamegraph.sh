#!/bin/sh

sudo  perf record -F 200 -a -g -- ./build/Debug/output
sudo perf script > out.perf
sudo stackcollapse-perf.pl out.perf > out.folded
flamegraph.pl out.folded > flame.svg
