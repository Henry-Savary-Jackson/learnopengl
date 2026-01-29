#!/bin/sh
flamegraph  -F 1000  -- -g --call-graph dwarf -e task-clock $1
