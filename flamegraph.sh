#!/bin/sh
flamegraph  -F 1000  -- -g --call-graph dwarf -e cpu-clock $1
